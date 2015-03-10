/*
 * overviewplugin.c - This file is part of the Geany Overview plugin
 *
 * Copyright (c) 2015 Matthew Brush <mbrush@codebrainz.ca>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */

#include "overviewplugin.h"
#include "overviewscintilla.h"
#include "overviewprefs.h"
#include "overviewprefspanel.h"
#include <errno.h>

GeanyPlugin    *geany_plugin;
GeanyData      *geany_data;
GeanyFunctions *geany_functions;

PLUGIN_VERSION_CHECK (211)

PLUGIN_SET_INFO (
  "Overview",
  "Provides an overview of the active document",
  "0.01",
  "Matthew Brush <matt@geany.org>")

static OverviewPrefs   *overview_prefs    = NULL;
static gboolean         overview_visible  = TRUE;

static GtkWidget *
hijack_scintilla (GeanyDocument  *doc,
                  GtkPositionType pos)
{
  GtkWidget *sci    = GTK_WIDGET (doc->editor->sci);
  GtkWidget *parent = gtk_widget_get_parent (sci);
  GtkWidget *cont   = gtk_hbox_new (FALSE, 0);
  GtkWidget *overview;

  overview = overview_scintilla_new (SCINTILLA (sci));
  overview_prefs_bind_scintilla (overview_prefs, G_OBJECT (overview));
  gtk_widget_set_no_show_all (overview, TRUE);

  gtk_container_remove (GTK_CONTAINER (parent), sci);
  if (pos == GTK_POS_LEFT)
    {
      gtk_box_pack_start (GTK_BOX (cont), overview, FALSE, TRUE, 0);
      gtk_box_pack_start (GTK_BOX (cont), sci, TRUE, TRUE, 0);
    }
  else
    {
      gtk_box_pack_start (GTK_BOX (cont), sci, TRUE, TRUE, 0);
      gtk_box_pack_start (GTK_BOX (cont), overview, FALSE, TRUE, 0);
    }
  gtk_container_add (GTK_CONTAINER (parent), cont);

  g_object_set_data (G_OBJECT (sci), "overview", overview);

  gtk_widget_show_all (cont);
  gtk_widget_set_visible (overview, overview_visible);

  return overview;
}

static void
hijack_all_scintillas (GtkPositionType pos)
{
  guint i = 0;
  foreach_document (i)
    hijack_scintilla (documents[i], pos);
}

static void
restore_scintilla (GeanyDocument *doc)
{
  GtkWidget *sci    = GTK_WIDGET (doc->editor->sci);
  GtkWidget *cont   = gtk_widget_get_parent (sci);
  GtkWidget *parent = gtk_widget_get_parent (cont);

  if (GTK_IS_WIDGET (sci))
    {
      g_object_ref (sci);
      gtk_container_remove (GTK_CONTAINER (cont), sci);
      gtk_container_remove (GTK_CONTAINER (parent), cont);
      gtk_container_add (GTK_CONTAINER (parent), sci);
      g_object_unref (sci);
    }
}

static void
restore_all_scintillas (void)
{
  guint i = 0;
  foreach_document (i)
    restore_scintilla (documents[i]);
}

static void
on_document_open_new (G_GNUC_UNUSED GObject *unused,
                      GeanyDocument         *doc,
                      G_GNUC_UNUSED gpointer user_data)
{
  GtkPositionType pos = GTK_POS_RIGHT;
  g_object_get (overview_prefs, "position", &pos, NULL);
  hijack_scintilla (doc, pos);
}

static void
on_document_activate (G_GNUC_UNUSED GObject *unused,
                      GeanyDocument         *doc,
                      G_GNUC_UNUSED gpointer user_data)
{
}

static void
on_document_reload (G_GNUC_UNUSED GObject *unused,
                    GeanyDocument         *doc,
                    G_GNUC_UNUSED gpointer user_data)
{
}

static void
on_document_close (G_GNUC_UNUSED GObject *unused,
                   GeanyDocument         *doc,
                   G_GNUC_UNUSED gpointer user_data)
{
  restore_scintilla (doc);
}

static gchar *
get_config_file (void)
{
  gchar              *dir;
  gchar              *fn;
  static const gchar *def_config = OVERVIEW_PREFS_DEFAULT_CONFIG;

  dir = g_build_filename (geany_data->app->configdir, "plugins", "overview", NULL);
  fn = g_build_filename (dir, "prefs.conf", NULL);

  if (! g_file_test (fn, G_FILE_TEST_IS_DIR))
    {
      if (g_mkdir_with_parents (dir, 0755) != 0)
        {
          g_critical ("failed to create config dir '%s': %s", dir, g_strerror (errno));
          g_free (dir);
          g_free (fn);
          return NULL;
        }
    }

  g_free (dir);

  if (! g_file_test (fn, G_FILE_TEST_EXISTS))
    {
      GError *error = NULL;
      if (!g_file_set_contents (fn, def_config, -1, &error))
        {
          g_critical ("failed to save default config to file '%s': %s",
                      fn, error->message);
          g_error_free (error);
          g_free (fn);
          return NULL;
        }
    }

  return fn;
}

static void
swap_scintillas (GtkPositionType pos)
{
  guint        i;
  gint         orig_page;
  GtkNotebook *nb = GTK_NOTEBOOK (geany_data->main_widgets->notebook);

  orig_page = gtk_notebook_get_current_page (nb);

  foreach_document (i)
    {
      GeanyDocument     *doc = documents[i];
      ScintillaObject   *sci;
      OverviewScintilla *overview;
      GtkWidget         *parent;
      gint               this_page;

      sci       = g_object_ref (doc->editor->sci);
      overview  = g_object_ref (g_object_get_data (G_OBJECT (sci), "overview"));
      parent    = gtk_widget_get_parent (GTK_WIDGET (sci));
      this_page = document_get_notebook_page (doc);

      gtk_container_remove (GTK_CONTAINER (parent), GTK_WIDGET (sci));
      gtk_container_remove (GTK_CONTAINER (parent), GTK_WIDGET (overview));

      if (pos == GTK_POS_LEFT)
        {
          gtk_box_pack_start (GTK_BOX (parent), GTK_WIDGET (overview), FALSE, TRUE, 0);
          gtk_box_pack_start (GTK_BOX (parent), GTK_WIDGET (sci), TRUE, TRUE, 0);
        }
      else
        {
          gtk_box_pack_start (GTK_BOX (parent), GTK_WIDGET (sci), TRUE, TRUE, 0);
          gtk_box_pack_start (GTK_BOX (parent), GTK_WIDGET (overview), FALSE, TRUE, 0);
        }

      gtk_widget_show_all (GTK_WIDGET (parent));
      gtk_widget_set_visible (GTK_WIDGET (overview), overview_visible);

      g_object_unref (sci);
      g_object_unref (overview);

      gtk_notebook_set_current_page (nb, this_page);
    }

  gtk_notebook_set_current_page (nb, orig_page);
}

static void
toggle_overviews_visiblity (void)
{
  guint i;
  overview_visible = ! overview_visible;
  foreach_document (i)
    {
      GeanyDocument *doc = documents[i];
      GtkWidget     *overview;
      overview = g_object_get_data (G_OBJECT (doc->editor->sci), "overview");
      gtk_widget_set_visible (overview, overview_visible);
    }
}

static void
on_position_pref_notify (OverviewPrefs *prefs,
                         GParamSpec    *pspec,
                         gpointer       user_data)
{
  GtkPositionType pos = GTK_POS_RIGHT;
  g_object_get (prefs, "position", &pos, NULL);
  swap_scintillas (pos);
}

enum
{
  KB_TOGGLE_VISIBLE,
  KB_TOGGLE_POSITION,
  KB_TOGGLE_INVERTED,
  NUM_KB
};

static gboolean
on_kb_activate (guint keybinding_id)
{
  switch (keybinding_id)
    {
    case KB_TOGGLE_VISIBLE:
      {
        toggle_overviews_visiblity ();
        break;
      }
    case KB_TOGGLE_POSITION:
      {
        GtkPositionType pos;
        g_object_get (overview_prefs, "position", &pos, NULL);
        pos = (pos == GTK_POS_LEFT) ? GTK_POS_RIGHT : GTK_POS_LEFT;
        g_object_set (overview_prefs, "position", pos, NULL);
        break;
      }
    case KB_TOGGLE_INVERTED:
      {
        gboolean inv = FALSE;
        g_object_get (overview_prefs, "overlay-inverted", &inv, NULL);
        g_object_set (overview_prefs, "overlay-inverted", !inv, NULL);
        break;
      }
    default:
      break;
    }
  return TRUE;
}

void
plugin_init (G_GNUC_UNUSED GeanyData *data)
{
  gchar          *conf_file;
  GError         *error = NULL;
  GeanyKeyGroup  *key_group;
  GtkPositionType pos = GTK_POS_RIGHT;

  plugin_module_make_resident (geany_plugin);

  key_group = plugin_set_key_group (geany_plugin,
                                    "overview",
                                    NUM_KB,
                                    on_kb_activate);

  keybindings_set_item (key_group,
                        KB_TOGGLE_VISIBLE,
                        NULL, 0, 0,
                        "toggle-visibility",
                        "Toggle Visibility",
                        NULL);

  keybindings_set_item (key_group,
                        KB_TOGGLE_POSITION,
                        NULL, 0, 0,
                        "toggle-position",
                        "Toggle Left/Right Position",
                        NULL);

  keybindings_set_item (key_group,
                        KB_TOGGLE_INVERTED,
                        NULL, 0, 0,
                        "toggle-inverted",
                        "Toggle Overlay Inversion",
                        NULL);

  overview_prefs = overview_prefs_new ();
  conf_file = get_config_file ();
  if (! overview_prefs_load (overview_prefs, conf_file, &error))
    {
      g_critical ("failed to load preferences file '%s': %s", conf_file, error->message);
      g_error_free (error);
    }
  g_free (conf_file);

  g_object_get (G_OBJECT (overview_prefs), "position", &pos, NULL);
  hijack_all_scintillas (pos);
  g_signal_connect (overview_prefs, "notify::position", G_CALLBACK (on_position_pref_notify), NULL);

  plugin_signal_connect (geany_plugin, NULL, "document-new", TRUE, G_CALLBACK (on_document_open_new), NULL);
  plugin_signal_connect (geany_plugin, NULL, "document-open", TRUE, G_CALLBACK (on_document_open_new), NULL);
  plugin_signal_connect (geany_plugin, NULL, "document-activate", TRUE, G_CALLBACK (on_document_activate), NULL);
  plugin_signal_connect (geany_plugin, NULL, "document-reload", TRUE, G_CALLBACK (on_document_reload), NULL);
}

void
plugin_cleanup (void)
{
  restore_all_scintillas ();

  if (OVERVIEW_IS_PREFS (overview_prefs))
    g_object_unref (overview_prefs);
  overview_prefs = NULL;
}

static void
on_prefs_stored (OverviewPrefsPanel *panel,
                 OverviewPrefs      *prefs,
                 gpointer            user_data)
{
  gchar  *conf_file;
  GError *error = NULL;
  conf_file = get_config_file ();
  if (! overview_prefs_save (overview_prefs, conf_file, &error))
    {
      g_critical ("failed to save preferences to file '%s': %s", conf_file, error->message);
      g_error_free (error);
    }
  g_free (conf_file);
}

GtkWidget *
plugin_configure (GtkDialog *dialog)
{
  GtkWidget *panel;
  panel = overview_prefs_panel_new (overview_prefs, dialog);
  g_signal_connect (panel, "prefs-stored", G_CALLBACK (on_prefs_stored), NULL);
  return panel;
}

#include "overviewplugin.h"
#include "overviewscintilla.h"

GeanyPlugin    *geany_plugin;
GeanyData      *geany_data;
GeanyFunctions *geany_functions;

#define OVERVIEW_WIDTH 160
#define OVERVIEW_ZOOM  0.025

PLUGIN_VERSION_CHECK (211)

PLUGIN_SET_INFO (
  "Overview",
  "Provides an overview of the active document",
  "0.01",
  "Matthew Brush <matt@geany.org>")

static GtkWidget *active_overview = NULL;

static inline GtkWidget *
document_get_overview (GeanyDocument *doc)
{
  if (DOC_VALID (doc))
    return g_object_get_data (G_OBJECT (doc->editor->sci), "overview");
  return NULL;
}

static GtkWidget *
hijack_scintilla (GeanyDocument *doc)
{
  GtkWidget *sci    = GTK_WIDGET (doc->editor->sci);
  GtkWidget *parent = gtk_widget_get_parent (sci);
  GtkWidget *cont   = gtk_hbox_new (FALSE, 0);
  GtkWidget *overview;

  overview = overview_scintilla_new (SCINTILLA (sci));
  gtk_widget_set_size_request (overview, OVERVIEW_WIDTH, -1);

  gtk_container_remove (GTK_CONTAINER (parent), sci);
  gtk_box_pack_start (GTK_BOX (cont), sci, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (cont), overview, FALSE, TRUE, 0);
  gtk_container_add (GTK_CONTAINER (parent), cont);

  g_object_set_data (G_OBJECT (sci), "overview", overview);
  g_object_set_data (G_OBJECT (sci), "document", doc);

  gtk_widget_show_all (cont);

  return overview;
}

static void
hijack_all_scintillas (void)
{
  guint i = 0;
  foreach_document (i)
    hijack_scintilla (documents[i]);
  active_overview = document_get_overview (document_get_current ());
}

static void
restore_scintilla (GeanyDocument *doc)
{
  GtkWidget *sci    = g_object_ref (doc->editor->sci);
  GtkWidget *cont   = gtk_widget_get_parent (sci);
  GtkWidget *parent = gtk_widget_get_parent (cont);

  gtk_container_remove (GTK_CONTAINER (cont), sci);
  gtk_container_remove (GTK_CONTAINER (parent), cont);

  if (IS_SCINTILLA (sci))
    {
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
  active_overview = hijack_scintilla (doc);
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

void
plugin_init (G_GNUC_UNUSED GeanyData *data)
{
  hijack_all_scintillas ();
  plugin_module_make_resident (geany_plugin);
  plugin_signal_connect (geany_plugin, NULL, "document-new", TRUE, G_CALLBACK (on_document_open_new), NULL);
  plugin_signal_connect (geany_plugin, NULL, "document-open", TRUE, G_CALLBACK (on_document_open_new), NULL);
  plugin_signal_connect (geany_plugin, NULL, "document-activate", TRUE, G_CALLBACK (on_document_activate), NULL);
  plugin_signal_connect (geany_plugin, NULL, "document-reload", TRUE, G_CALLBACK (on_document_reload), NULL);
}

void
plugin_cleanup (void)
{
  restore_all_scintillas ();
  active_overview = NULL;
}

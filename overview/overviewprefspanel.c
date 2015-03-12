/*
 * overviewprefspanel.c - This file is part of the Geany Overview plugin
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

#include "overviewprefspanel.h"
#include "overviewcolor.h"

struct OverviewPrefsPanel_
{
  GtkFrame       parent;
  OverviewPrefs *prefs;
  GtkWidget     *prefs_table;
  GtkWidget     *width_spin;
  GtkWidget     *zoom_spin;
  GtkWidget     *show_tt_yes;
  GtkWidget     *show_sb_yes;
  GtkWidget     *dbl_buf_yes;
  GtkWidget     *scr_lines_spin;
  GtkWidget     *ovl_en_yes;
  GtkWidget     *ovl_clr_btn;
  GtkWidget     *out_clr_btn;
  GtkWidget     *ovl_inv_yes;
  GtkWidget     *pos_left_check;
};

struct OverviewPrefsPanelClass_
{
  GtkFrameClass parent_class;
};

static void overview_prefs_panel_finalize (GObject *object);

G_DEFINE_TYPE (OverviewPrefsPanel, overview_prefs_panel, GTK_TYPE_FRAME)

static void
overview_prefs_panel_class_init (OverviewPrefsPanelClass *klass)
{
  GObjectClass *g_object_class;

  g_object_class = G_OBJECT_CLASS (klass);

  g_object_class->finalize = overview_prefs_panel_finalize;

  g_signal_new ("prefs-stored",
                G_TYPE_FROM_CLASS (g_object_class),
                G_SIGNAL_RUN_FIRST,
                0, NULL, NULL,
                g_cclosure_marshal_VOID__OBJECT,
                G_TYPE_NONE,
                1, OVERVIEW_TYPE_PREFS);

  g_signal_new ("prefs-loaded",
                G_TYPE_FROM_CLASS (g_object_class),
                G_SIGNAL_RUN_FIRST,
                0, NULL, NULL,
                g_cclosure_marshal_VOID__OBJECT,
                G_TYPE_NONE,
                1, OVERVIEW_TYPE_PREFS);
}

static void
overview_prefs_panel_finalize (GObject *object)
{
  OverviewPrefsPanel *self;

  g_return_if_fail (OVERVIEW_IS_PREFS_PANEL (object));

  self = OVERVIEW_PREFS_PANEL (object);

  g_object_unref (self->prefs);

  G_OBJECT_CLASS (overview_prefs_panel_parent_class)->finalize (object);
}

static GtkWidget *
builder_get_widget (GtkBuilder  *builder,
                    const gchar *name)
{
  GObject *result = NULL;
  gchar   *real_name;

  real_name = g_strdup_printf ("overview-%s", name);
  result    = gtk_builder_get_object (builder, real_name);

  if (! G_IS_OBJECT (result))
    g_critical ("unable to find widget '%s' in UI file", real_name);
  else if (! GTK_IS_WIDGET (result))
    g_critical ("object '%s' in UI file is not a widget", real_name);

  g_free (real_name);

  return (GtkWidget*) result;
}

static void
overview_prefs_panel_store_prefs (OverviewPrefsPanel *self)
{
  gint          zoom      = 0;
  guint         width     = 0;
  guint         scr_lines = 0;
  gboolean      show_tt   = FALSE;
  gboolean      show_sb   = FALSE;
  gboolean      dbl_buf   = FALSE;
  gboolean      pos_left  = FALSE;
  gboolean      ovl_en    = FALSE;
  gboolean      ovl_inv   = FALSE;
  OverviewColor ovl_clr   = {0,0,0,0};
  OverviewColor out_clr   = {0,0,0,0};

  width     = gtk_spin_button_get_value (GTK_SPIN_BUTTON (self->width_spin));
  zoom      = gtk_spin_button_get_value (GTK_SPIN_BUTTON (self->zoom_spin));
  scr_lines = gtk_spin_button_get_value (GTK_SPIN_BUTTON (self->scr_lines_spin));
  show_tt   = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (self->show_tt_yes));
  show_sb   = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (self->show_sb_yes));
  dbl_buf   = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (self->dbl_buf_yes));
  ovl_en    = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (self->ovl_en_yes));
  ovl_inv   = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (self->ovl_inv_yes));
  pos_left  = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (self->pos_left_check));
  overview_color_from_color_button (&ovl_clr, GTK_COLOR_BUTTON (self->ovl_clr_btn));
  overview_color_from_color_button (&out_clr, GTK_COLOR_BUTTON (self->out_clr_btn));

  g_object_set (self->prefs,
                "width", width,
                "zoom", zoom,
                "show-tooltip", show_tt,
                "show-scrollbar", show_sb,
                "double-buffered", dbl_buf,
                "scroll-lines", scr_lines,
                "position", pos_left ? GTK_POS_LEFT : GTK_POS_RIGHT,
                "overlay-enabled", ovl_en,
                "overlay-inverted", ovl_inv,
                "overlay-color", &ovl_clr,
                "overlay-outline-color", &out_clr,
                NULL);

  g_signal_emit_by_name (self, "prefs-stored", self->prefs);
}

static void
overview_prefs_panel_load_prefs (OverviewPrefsPanel *self)
{
  gint            zoom      = 0;
  guint           width     = 0;
  guint           scr_lines = 0;
  gboolean        show_tt   = FALSE;
  gboolean        show_sb   = FALSE;
  gboolean        dbl_buf   = FALSE;
  gboolean        ovl_en    = FALSE;
  gboolean        ovl_inv   = FALSE;
  GtkPositionType pos       = FALSE;
  OverviewColor  *ovl_clr   = NULL;
  OverviewColor  *out_clr   = NULL;

  g_object_get (self->prefs,
                "width", &width,
                "zoom", &zoom,
                "show-tooltip", &show_tt,
                "show-scrollbar", &show_sb,
                "double-buffered", &dbl_buf,
                "scroll-lines", &scr_lines,
                "position", &pos,
                "overlay-enabled", &ovl_en,
                "overlay-inverted", &ovl_inv,
                "overlay-color", &ovl_clr,
                "overlay-outline-color", &out_clr,
                NULL);

  gtk_spin_button_set_value (GTK_SPIN_BUTTON (self->width_spin), width);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (self->zoom_spin), zoom);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (self->show_tt_yes), show_tt);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (self->show_sb_yes), show_sb);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (self->dbl_buf_yes), dbl_buf);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (self->scr_lines_spin), scr_lines);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (self->pos_left_check), pos == GTK_POS_LEFT);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (self->ovl_inv_yes), ovl_inv);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (self->ovl_en_yes), ovl_en);
  overview_color_to_color_button (ovl_clr, GTK_COLOR_BUTTON (self->ovl_clr_btn));
  overview_color_to_color_button (out_clr, GTK_COLOR_BUTTON (self->out_clr_btn));

  overview_color_free (ovl_clr);
  overview_color_free (out_clr);

  g_signal_emit_by_name (self, "prefs-loaded", self->prefs);
}

static void
overview_prefs_panel_init (OverviewPrefsPanel *self)
{
  GtkBuilder *builder;
  GError     *error = NULL;
  GtkWidget  *ovl_clr_lbl;
  GtkWidget  *out_clr_lbl;
  GtkWidget  *ovl_inv_lbl;
  GtkWidget  *ovl_inv_hbox;

  builder = gtk_builder_new ();
  if (! gtk_builder_add_from_file (builder, OVERVIEW_PREFS_UI_FILE, &error))
    {
      g_critical ("failed to open UI file '%s': %s", OVERVIEW_PREFS_UI_FILE, error->message);
      g_error_free (error);
      g_object_unref (builder);
      return;
    }

  self->prefs_table    = builder_get_widget (builder, "prefs-table");
  self->width_spin     = builder_get_widget (builder, "width-spin");
  self->zoom_spin      = builder_get_widget (builder, "zoom-spin");
  self->show_tt_yes    = builder_get_widget (builder, "show-tooltip-yes-check");
  self->show_sb_yes    = builder_get_widget (builder, "show-scrollbar-yes-check");
  self->dbl_buf_yes    = builder_get_widget (builder, "double-buffered-yes-check");
  self->scr_lines_spin = builder_get_widget (builder, "scroll-lines-spin");
  self->ovl_en_yes     = builder_get_widget (builder, "overlay-enabled-yes-check");
  self->ovl_clr_btn    = builder_get_widget (builder, "overlay-color");
  self->out_clr_btn    = builder_get_widget (builder, "overlay-outline-color");
  self->ovl_inv_yes    = builder_get_widget (builder, "overlay-inverted-yes-check");
  self->pos_left_check = builder_get_widget (builder, "position-left-check");

  ovl_clr_lbl  = builder_get_widget (builder, "overlay-color-label");
  out_clr_lbl  = builder_get_widget (builder, "overlay-outline-label");
  ovl_inv_lbl  = builder_get_widget (builder, "overlay-inverted-label");
  ovl_inv_hbox = builder_get_widget (builder, "overlay-inverted-hbox");

  g_object_bind_property (self->ovl_en_yes, "active", ovl_clr_lbl, "sensitive", G_BINDING_SYNC_CREATE);
  g_object_bind_property (self->ovl_en_yes, "active", out_clr_lbl, "sensitive", G_BINDING_SYNC_CREATE);
  g_object_bind_property (self->ovl_en_yes, "active", ovl_inv_lbl, "sensitive", G_BINDING_SYNC_CREATE);
  g_object_bind_property (self->ovl_en_yes, "active", ovl_inv_hbox, "sensitive", G_BINDING_SYNC_CREATE);

  gtk_widget_show_all (self->prefs_table);
  gtk_container_add (GTK_CONTAINER (self), self->prefs_table);
  g_object_unref (builder);
}

static void
on_host_dialog_response (GtkDialog          *dialog,
                         gint                response_id,
                         OverviewPrefsPanel *self)
{
  switch (response_id)
    {
    case GTK_RESPONSE_APPLY:
    case GTK_RESPONSE_OK:
      overview_prefs_panel_store_prefs (self);
      break;
    case GTK_RESPONSE_CANCEL:
    default:
      break;
    }
}

GtkWidget *
overview_prefs_panel_new (OverviewPrefs *prefs,
                          GtkDialog     *host_dialog)
{
  OverviewPrefsPanel *self;
  self = g_object_new (OVERVIEW_TYPE_PREFS_PANEL, NULL);
  self->prefs = g_object_ref (prefs);
  g_signal_connect (host_dialog, "response", G_CALLBACK (on_host_dialog_response), self);
  overview_prefs_panel_load_prefs (self);
  return GTK_WIDGET (self);
}

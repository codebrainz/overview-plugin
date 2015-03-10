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
  GtkWidget     *ovl_clr_lbl;
  GtkWidget     *ovl_clr_btn;
  GtkWidget     *out_clr_lbl;
  GtkWidget     *out_clr_btn;
  GtkWidget     *ovl_inv_lbl;
  GtkWidget     *ovl_inv_hbox;
  GtkWidget     *ovl_inv_yes;
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
  guint         uval;
  gint          ival;
  OverviewColor cval;
  gboolean      bval;

  uval = gtk_spin_button_get_value (GTK_SPIN_BUTTON (self->width_spin));
  g_object_set (self->prefs, "width", uval, NULL);
  ival = gtk_spin_button_get_value (GTK_SPIN_BUTTON (self->zoom_spin));
  g_object_set (self->prefs, "zoom", ival, NULL);
  bval = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (self->show_tt_yes));
  g_object_set (self->prefs, "show-tooltip", bval, NULL);
  bval = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (self->show_sb_yes));
  g_object_set (self->prefs, "show-scrollbar", bval, NULL);
  bval = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (self->dbl_buf_yes));
  g_object_set (self->prefs, "double-buffered", bval, NULL);
  uval = gtk_spin_button_get_value (GTK_SPIN_BUTTON (self->scr_lines_spin));
  g_object_set (self->prefs, "scroll-lines", uval, NULL);
  bval = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (self->ovl_en_yes));
  g_object_set (self->prefs, "overlay-enabled", bval, NULL);
  bval = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (self->ovl_inv_yes));
  g_object_set (self->prefs, "overlay-inverted", bval, NULL);
  overview_color_from_color_button (&cval, GTK_COLOR_BUTTON (self->ovl_clr_btn));
  g_object_set (self->prefs, "overlay-color", &cval, NULL);
  overview_color_from_color_button (&cval, GTK_COLOR_BUTTON (self->out_clr_btn));
  g_object_set (self->prefs, "overlay-outline-color", &cval, NULL);

  g_signal_emit_by_name (self, "prefs-stored", self->prefs);
}

static void
on_overlay_enable_toggled (GtkToggleButton    *button,
                           OverviewPrefsPanel *self)
{
  gboolean active;
  active = gtk_toggle_button_get_active (button);
  gtk_widget_set_sensitive (self->ovl_clr_lbl, active);
  gtk_widget_set_sensitive (self->ovl_clr_btn, active);
  gtk_widget_set_sensitive (self->out_clr_lbl, active);
  gtk_widget_set_sensitive (self->out_clr_btn, active);
  gtk_widget_set_sensitive (self->ovl_inv_lbl, active);
  gtk_widget_set_sensitive (self->ovl_inv_hbox, active);
}

static void
overview_prefs_panel_load_prefs (OverviewPrefsPanel *self)
{
  guint          uval;
  gint           ival;
  OverviewColor *cval = NULL;
  gboolean       bval;

  g_object_get (self->prefs, "width", &uval, NULL);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (self->width_spin), uval);
  g_object_get (self->prefs, "zoom", &ival, NULL);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (self->zoom_spin), ival);
  g_object_get (self->prefs, "show-tooltip", &bval, NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (self->show_tt_yes), bval);
  g_object_get (self->prefs, "show-scrollbar", &bval, NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (self->show_sb_yes), bval);
  g_object_get (self->prefs, "double-buffered", &bval, NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (self->dbl_buf_yes), bval);
  g_object_get (self->prefs, "scroll-lines", &uval, NULL);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (self->scr_lines_spin), uval);
  g_object_get (self->prefs, "overlay-enabled", &bval, NULL);
  g_object_freeze_notify (G_OBJECT (self->ovl_en_yes));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (self->ovl_en_yes), bval);
  g_object_thaw_notify (G_OBJECT (self->ovl_en_yes));
  gtk_widget_set_sensitive (self->ovl_clr_lbl, bval);
  gtk_widget_set_sensitive (self->ovl_clr_btn, bval);
  gtk_widget_set_sensitive (self->out_clr_lbl, bval);
  gtk_widget_set_sensitive (self->out_clr_btn, bval);
  gtk_widget_set_sensitive (self->ovl_inv_lbl, bval);
  gtk_widget_set_sensitive (self->ovl_inv_hbox, bval);
  g_object_get (self->prefs, "overlay-color", &cval, NULL);
  overview_color_to_color_button (cval, GTK_COLOR_BUTTON (self->ovl_clr_btn));
  overview_color_free (cval);
  cval = NULL;
  g_object_get (self->prefs, "overlay-outline-color", &cval, NULL);
  overview_color_to_color_button (cval, GTK_COLOR_BUTTON (self->out_clr_btn));
  overview_color_free (cval);
  g_object_get (self->prefs, "overlay-inverted", &bval, NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (self->ovl_inv_yes), bval);

  g_signal_emit_by_name (self, "prefs-loaded", self->prefs);
}

static void
overview_prefs_panel_init (OverviewPrefsPanel *self)
{
  GtkBuilder *builder;
  GError     *error = NULL;

  builder = gtk_builder_new ();
  if (! gtk_builder_add_from_file (builder, OVERVIEW_PREFS_UI_FILE, &error))
    {
      g_critical ("failed to open UI file '%s': %s", OVERVIEW_PREFS_UI_FILE, error->message);
      g_error_free (error);
      g_object_unref (builder);
      return;
    }

  self->prefs_table    = g_object_ref (builder_get_widget (builder, "prefs-table"));
  self->width_spin     = builder_get_widget (builder, "width-spin");
  self->zoom_spin      = builder_get_widget (builder, "zoom-spin");
  self->show_tt_yes    = builder_get_widget (builder, "show-tooltip-yes-check");
  self->show_sb_yes    = builder_get_widget (builder, "show-scrollbar-yes-check");
  self->dbl_buf_yes    = builder_get_widget (builder, "double-buffered-yes-check");
  self->scr_lines_spin = builder_get_widget (builder, "scroll-lines-spin");
  self->ovl_en_yes     = builder_get_widget (builder, "overlay-enabled-yes-check");
  self->ovl_clr_lbl    = builder_get_widget (builder, "overlay-color-label");
  self->ovl_clr_btn    = builder_get_widget (builder, "overlay-color");
  self->out_clr_lbl    = builder_get_widget (builder, "overlay-outline-label");
  self->out_clr_btn    = builder_get_widget (builder, "overlay-outline-color");
  self->ovl_inv_lbl    = builder_get_widget (builder, "overlay-inverted-label");
  self->ovl_inv_hbox   = builder_get_widget (builder, "overlay-inverted-hbox");
  self->ovl_inv_yes    = builder_get_widget (builder, "overlay-inverted-yes-check");

  g_object_unref (builder);

  gtk_widget_show_all (self->prefs_table);
  gtk_container_add (GTK_CONTAINER (self), self->prefs_table);
  g_object_unref (self->prefs_table);

  g_signal_connect (self->ovl_en_yes, "toggled", G_CALLBACK (on_overlay_enable_toggled), self);
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

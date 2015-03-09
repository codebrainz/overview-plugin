#include "overviewplugin.h"
#include "overviewscintilla.h"
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

static GtkWidget  *active_overview = NULL;
static GtkWidget  *prefs_table     = NULL;
static GtkBuilder *prefs_builder   = NULL;

static struct OverviewPrefs
{
  gint          width;
  gint          zoom;
  gboolean      show_tooltip;
  gboolean      double_buffered;
  gint          scroll_lines;
  gboolean      show_scrollbar;
  gboolean      overlay_enabled;
  OverviewColor overlay_color;
  OverviewColor outline_color;
}
overview_prefs =
{
  .width           = 120,
  .zoom            = -20,
  .show_tooltip    = TRUE,
  .double_buffered = TRUE,
  .scroll_lines    = 1,
  .show_scrollbar  = TRUE,
  .overlay_enabled = TRUE,
  .overlay_color   = { 0.0, 0.0, 0.0, 0.25 },
  .outline_color   = { 0.0, 0.0, 0.0, 0.75 },
};

static void
apply_prefs (OverviewScintilla *overview)
{
  g_object_set (overview,
                "zoom", overview_prefs.zoom,
                "show-tooltip", overview_prefs.show_tooltip,
                "double-buffered", overview_prefs.double_buffered,
                "scroll-lines", overview_prefs.scroll_lines,
                "show-scrollbar", overview_prefs.show_scrollbar,
                "overlay-enabled", overview_prefs.overlay_enabled,
                "overlay-color", &overview_prefs.overlay_color,
                "overlay-outline-color", &overview_prefs.outline_color,
                NULL);
  gtk_widget_set_size_request (GTK_WIDGET (overview), overview_prefs.width, -1);
  overview_scintilla_queue_draw (overview);
}

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
  apply_prefs (OVERVIEW_SCINTILLA (overview));

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

static const gchar *def_config =
  "[overview]\n"
  "width = 120\n"
  "zoom = -20\n"
  "show-tooltip = true\n"
  "double-buffered = true\n"
  "scroll-lines = 1\n"
  "show-scrollbar = true\n"
  "overlay-enabled = true\n"
  "overlay-color = #000\n"
  "overlay-color-alpha = 0.25\n"
  "overlay-outline-color = #000\n"
  "overlay-outline-color-alpha = 0.75\n"
  "\n";

static gchar *
get_config_file (void)
{
  gchar *dir;
  gchar *fn;

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
load_prefs (void)
{
  gchar    *cfg_file;
  GKeyFile *kf;
  GError   *error = NULL;
  gchar    *color;
  gdouble   alpha;
  GdkColor  gdk_color;

  cfg_file = get_config_file ();
  kf = g_key_file_new ();
  if (! g_key_file_load_from_file (kf, cfg_file,
                                   G_KEY_FILE_KEEP_COMMENTS |
                                     G_KEY_FILE_KEEP_TRANSLATIONS,
                                   &error))
    {
      g_critical ("failed to read config keyfile '%s': %s",
                  cfg_file, error->message);
      g_error_free (error);
      g_free (cfg_file);
      return;
    }

  if (!g_key_file_has_group (kf, "overview"))
    {
      g_warning ("no prefs found in config file '%s'", cfg_file);
      g_key_file_free (kf);
      g_free (cfg_file);
      return;
    }

  g_free (cfg_file);

  overview_prefs.width = g_key_file_get_integer (kf, "overview", "width", NULL);
  overview_prefs.zoom = g_key_file_get_integer (kf, "overview", "zoom", NULL);
  overview_prefs.show_tooltip = g_key_file_get_boolean (kf, "overview", "show-tooltip", NULL);
  overview_prefs.double_buffered = g_key_file_get_boolean (kf, "overview", "double-buffered", NULL);
  overview_prefs.scroll_lines = g_key_file_get_integer (kf, "overview", "scroll-lines", NULL);
  overview_prefs.show_scrollbar = g_key_file_get_boolean (kf, "overview", "show-scrollbar", NULL);
  overview_prefs.overlay_enabled = g_key_file_get_boolean (kf, "overview", "overlay-enabled", NULL);

  color = g_key_file_get_string (kf, "overview", "overlay-color", NULL);
  if (gdk_color_parse (color, &gdk_color))
    {
      overview_prefs.overlay_color.red   = (gdouble) gdk_color.red / (gdouble) G_MAXUINT16;
      overview_prefs.overlay_color.green = (gdouble) gdk_color.green / (gdouble) G_MAXUINT16;
      overview_prefs.overlay_color.blue  = (gdouble) gdk_color.blue / (gdouble) G_MAXUINT16;
    }
  else
    {
      g_warning ("failed to parse overlay color '%s'", color);
    }
  g_free (color);
  overview_prefs.overlay_color.alpha = g_key_file_get_double (kf, "overview", "overlay-color-alpha", NULL);

  color = g_key_file_get_string (kf, "overview", "overlay-outline-color", NULL);
  if (gdk_color_parse (color, &gdk_color))
    {
      overview_prefs.outline_color.red   = (gdouble) gdk_color.red / (gdouble) G_MAXUINT16;
      overview_prefs.outline_color.green = (gdouble) gdk_color.green / (gdouble) G_MAXUINT16;
      overview_prefs.outline_color.blue  = (gdouble) gdk_color.blue / (gdouble) G_MAXUINT16;
    }
  else
    {
      g_warning ("failed to parse overlay outline color '%s'", color);
    }
  g_free (color);
  overview_prefs.outline_color.alpha = g_key_file_get_double (kf, "overview", "overlay-outline-color-alpha", NULL);

  g_key_file_free (kf);

  g_debug ("\n\tOverview Settings\n"
           "\t=================\n"
           "\t  Width: %d\n"
           "\t  Zoom: %d\n"
           "\t  Show Tooltip: %d\n"
           "\t  Double Buffered: %d\n"
           "\t  Scroll Lines: %d\n"
           "\t  Show Scrollbar: %d\n"
           "\t  Overlay Enabled: %d\n"
           "\t  Overlay Color: %g, %g, %g, %g\n"
           "\t  Outline Color: %g, %g, %g, %g\n",
           overview_prefs.width,
           overview_prefs.zoom,
           overview_prefs.show_tooltip,
           overview_prefs.double_buffered,
           overview_prefs.scroll_lines,
           overview_prefs.show_scrollbar,
           overview_prefs.overlay_enabled,
           overview_prefs.overlay_color.red,
           overview_prefs.overlay_color.green,
           overview_prefs.overlay_color.blue,
           overview_prefs.overlay_color.alpha,
           overview_prefs.outline_color.red,
           overview_prefs.outline_color.green,
           overview_prefs.outline_color.blue,
           overview_prefs.outline_color.alpha);

}

void
plugin_init (G_GNUC_UNUSED GeanyData *data)
{
  plugin_module_make_resident (geany_plugin);
  load_prefs ();
  hijack_all_scintillas ();
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

  if (GTK_IS_WIDGET (prefs_table))
    {
      g_object_unref (prefs_table);
      prefs_table = NULL;
    }

  if (GTK_IS_BUILDER (prefs_builder))
    {
      g_object_unref (prefs_builder);
      prefs_builder = NULL;
    }
}

static gchar *
get_ui_file (void)
{
#ifdef OVERVIEW_PREFS_UI_FILE
  return g_strdup (OVERVIEW_PREFS_UI_FILE);
#else
  return g_build_filename (geany_data->app->configdir,
                           "plugins", "overview", "prefs.glade", NULL);
#endif
}

static inline GtkWidget *
get_widget (const gchar *name)
{
  GtkWidget *wid = GTK_WIDGET (gtk_builder_get_object (prefs_builder, name));
  if (!GTK_IS_WIDGET (wid))
    {
      g_critical ("failed to find widget '%s'", name);
    }
  return wid;
}

G_MODULE_EXPORT void
overview_enabled_yes_check_toggled (GtkToggleButton *button,
                                    gpointer         user_data)
{
  GtkWidget *wid;
  gboolean   active = gtk_toggle_button_get_active (button);

  wid = get_widget ("overview-overlay-color");
  gtk_widget_set_sensitive (wid, active);

  wid = get_widget ("overview-overlay-color-label");
  gtk_widget_set_sensitive (wid, active);

  wid = get_widget ("overview-overlay-outline-color");
  gtk_widget_set_sensitive (wid, active);

  wid = get_widget ("overview-overlay-outline-label");
  gtk_widget_set_sensitive (wid, active);
}

static void
setup_prefs_table (void)
{
  GtkWidget *wid;
  GdkColor   color;

  wid = get_widget ("overview-width-spin");
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (wid), overview_prefs.width);

  wid = get_widget ("overview-zoom-spin");
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (wid), overview_prefs.zoom);

  wid = get_widget ("overview-show-tooltip-yes-check");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (wid), overview_prefs.show_tooltip);

  wid = get_widget ("overview-double-buffered-yes-check");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (wid), overview_prefs.double_buffered);

  wid = get_widget ("overview-scroll-lines-spin");
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (wid), overview_prefs.scroll_lines);

  wid = get_widget ("overview-show-scrollbar-yes-check");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (wid), overview_prefs.show_scrollbar);

  wid = get_widget ("overview-overlay-color");
  color.red = (guint16)(overview_prefs.overlay_color.red * G_MAXUINT16);
  color.green = (guint16)(overview_prefs.overlay_color.green * G_MAXUINT16);
  color.blue = (guint16)(overview_prefs.overlay_color.blue * G_MAXUINT16);
  gtk_color_button_set_color (GTK_COLOR_BUTTON (wid), &color);
  gtk_color_button_set_alpha (GTK_COLOR_BUTTON (wid), (guint16)(overview_prefs.overlay_color.alpha * G_MAXUINT16));

  wid = get_widget ("overview-overlay-outline-color");
  color.red = (guint16)(overview_prefs.outline_color.red * G_MAXUINT16);
  color.green = (guint16)(overview_prefs.outline_color.green * G_MAXUINT16);
  color.blue = (guint16)(overview_prefs.outline_color.blue * G_MAXUINT16);
  gtk_color_button_set_color (GTK_COLOR_BUTTON (wid), &color);
  gtk_color_button_set_alpha (GTK_COLOR_BUTTON (wid), (guint16)(overview_prefs.outline_color.alpha * G_MAXUINT16));

  wid = get_widget ("overview-overlay-color-label");
  gtk_widget_set_sensitive (wid, overview_prefs.overlay_enabled);
  wid = get_widget ("overview-overlay-outline-label");
  gtk_widget_set_sensitive (wid, overview_prefs.overlay_enabled);
  wid = get_widget ("overview-overlay-color");
  gtk_widget_set_sensitive (wid, overview_prefs.overlay_enabled);
  wid = get_widget ("overview-overlay-outline-color");
  gtk_widget_set_sensitive (wid, overview_prefs.overlay_enabled);

  wid = get_widget ("overview-overlay-enabled-yes-check");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (wid), overview_prefs.overlay_enabled);
}

static GtkWidget *
create_prefs_table (void)
{
  gchar      *ui_file;
  GError     *error = NULL;

  if (GTK_IS_WIDGET (prefs_table))
    {
      setup_prefs_table ();
      return g_object_ref (prefs_table);
    }

  if (! GTK_IS_BUILDER (prefs_builder))
    {
      ui_file = get_ui_file ();
      prefs_builder = gtk_builder_new();

      if (! gtk_builder_add_from_file (prefs_builder, ui_file, &error))
        {
          g_critical ("failed to load UI file '%s': %s", ui_file, error->message);
          g_error_free (error);
          g_free (ui_file);
          g_object_unref (prefs_builder);
          prefs_builder = NULL;
          return NULL;
        }

        g_free (ui_file);
      }

  plugin_builder_connect_signals (geany_plugin, prefs_builder, NULL);
  prefs_table = g_object_ref (gtk_builder_get_object (prefs_builder, "overview-prefs-table"));
  setup_prefs_table ();

  return g_object_ref (prefs_table);
}

static void
apply_prefs_to_all_overlays (void)
{
  for (guint i = 0; i < documents_array->len; i++)
    {
      GeanyDocument *doc = documents_array->pdata[i];
      if (DOC_VALID (doc))
        {
          OverviewScintilla *sci;
          sci = g_object_get_data (G_OBJECT (doc->editor->sci), "overview");
          if (OVERVIEW_IS_SCINTILLA (sci))
            apply_prefs (sci);
        }
    }
}

static void
save_prefs (void)
{
  GKeyFile *kf;
  gchar    *fn;
  gchar    *clr_str;
  gchar    *content;
  gsize     size = 0;
  GdkColor  color;
  GError   *error = NULL;

  kf = g_key_file_new ();

  g_key_file_set_integer (kf, "overview", "width", overview_prefs.width);
  g_key_file_set_integer (kf, "overview", "zoom", overview_prefs.zoom);
  g_key_file_set_boolean (kf, "overview", "show-tooltip", overview_prefs.show_tooltip);
  g_key_file_set_boolean (kf, "overview", "double-buffered", overview_prefs.double_buffered);
  g_key_file_set_integer (kf, "overview", "scroll-lines", overview_prefs.scroll_lines);
  g_key_file_set_boolean (kf, "overview", "show-scrollbar", overview_prefs.show_scrollbar);
  g_key_file_set_boolean (kf, "overview", "overlay-enabled", overview_prefs.overlay_enabled);
  g_key_file_set_double (kf, "overview", "overlay-color-alpha", overview_prefs.overlay_color.alpha);
  g_key_file_set_double (kf, "overview", "overlay-outline-color-alpha", overview_prefs.outline_color.alpha);

  color.red = (guint16)(overview_prefs.overlay_color.red * G_MAXUINT16);
  color.green = (guint16)(overview_prefs.overlay_color.green * G_MAXUINT16);
  color.blue = (guint16)(overview_prefs.overlay_color.blue * G_MAXUINT16);
  clr_str = gdk_color_to_string (&color);
  g_key_file_set_string (kf, "overview", "overlay-color", clr_str);
  g_free (clr_str);

  color.red = (guint16)(overview_prefs.outline_color.red * G_MAXUINT16);
  color.green = (guint16)(overview_prefs.outline_color.green * G_MAXUINT16);
  color.blue = (guint16)(overview_prefs.outline_color.blue * G_MAXUINT16);
  clr_str = gdk_color_to_string (&color);
  g_key_file_set_string (kf, "overview", "overlay-outline-color", clr_str);
  g_free (clr_str);

  fn = get_config_file ();
  content = g_key_file_to_data (kf, &size, &error);
  if (content == NULL)
    {
      g_warning ("failed to save config data to file '%s': %s",
                 fn, error->message);
      g_error_free (error);
      g_free (fn);
      g_key_file_free (kf);
      return;
    }

  error = NULL;
  if (! g_file_set_contents (fn, content, size, &error))
    {
      g_warning ("failed to store contents of config file '%s': %s",
                 fn, error->message);
      g_error_free (error);
      g_free (fn);
      g_free (content);
      g_key_file_free (kf);
      return;
    }

  g_free (fn);
  g_free (content);
  g_key_file_free (kf);
}

static void
on_prefs_response (GtkDialog *dialog,
                   gint       response_id,
                   gpointer   user_data)
{
  GtkWidget *wid;
  GdkColor   color;

  if (response_id != GTK_RESPONSE_OK && response_id != GTK_RESPONSE_APPLY)
    goto cleanup_after;

  g_return_if_fail (GTK_IS_BUILDER (prefs_builder));

  wid = get_widget ("overview-width-spin");
  overview_prefs.width = gtk_spin_button_get_value (GTK_SPIN_BUTTON (wid));

  wid = get_widget ("overview-zoom-spin");
  overview_prefs.zoom = gtk_spin_button_get_value (GTK_SPIN_BUTTON (wid));

  wid = get_widget ("overview-show-tooltip-yes-check");
  overview_prefs.show_tooltip = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (wid));

  wid = get_widget ("overview-double-buffered-yes-check");
  overview_prefs.double_buffered = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (wid));

  wid = get_widget ("overview-scroll-lines-spin");
  overview_prefs.scroll_lines = gtk_spin_button_get_value (GTK_SPIN_BUTTON (wid));

  wid = get_widget ("overview-show-scrollbar-yes-check");
  overview_prefs.show_scrollbar = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (wid));

  wid = get_widget ("overview-overlay-enabled-yes-check");
  overview_prefs.overlay_enabled = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (wid));

  wid = get_widget ("overview-overlay-color");
  gtk_color_button_get_color (GTK_COLOR_BUTTON (wid), &color);
  overview_prefs.overlay_color.red = (gdouble) color.red / (gdouble) G_MAXUINT16;
  overview_prefs.overlay_color.green = (gdouble) color.green / (gdouble) G_MAXUINT16;
  overview_prefs.overlay_color.blue = (gdouble) color.blue / (gdouble) G_MAXUINT16;
  overview_prefs.overlay_color.alpha = (gdouble) gtk_color_button_get_alpha (GTK_COLOR_BUTTON (wid)) / (gdouble) G_MAXUINT16;

  wid = get_widget ("overview-overlay-outline-color");
  gtk_color_button_get_color (GTK_COLOR_BUTTON (wid), &color);
  overview_prefs.outline_color.red = (gdouble) color.red / (gdouble) G_MAXUINT16;
  overview_prefs.outline_color.green = (gdouble) color.green / (gdouble) G_MAXUINT16;
  overview_prefs.outline_color.blue = (gdouble) color.blue / (gdouble) G_MAXUINT16;
  overview_prefs.outline_color.alpha = (gdouble) gtk_color_button_get_alpha (GTK_COLOR_BUTTON (wid)) / (gdouble) G_MAXUINT16;

  apply_prefs_to_all_overlays ();
  save_prefs ();

  if (response_id == GTK_RESPONSE_OK)
    goto cleanup_after;

  return;

cleanup_after:
  g_object_unref (prefs_table);
  prefs_table = NULL;

  if (GTK_IS_BUILDER (prefs_builder))
    {
      g_object_unref (prefs_builder);
      prefs_builder = NULL;
    }
}

GtkWidget *
plugin_configure (GtkDialog *dialog)
{
  GtkWidget *tab;
  tab = create_prefs_table ();
  g_signal_connect (dialog, "response", G_CALLBACK (on_prefs_response), NULL);
  gtk_widget_show_all (tab);
  return tab;
}

#include "overviewprefs.h"
#include "overviewcolor.h"
#include "overviewscintilla.h"
#include <string.h>
#include <stdarg.h>

#ifndef OVERVIEW_PREFS_UI_FILE
# error "You must define OVERVIEW_PREFS_UI_FILE to the full GtkBuilder UI file path"
#endif

enum
{
  PROP_0,
  PROP_WIDTH,
  PROP_ZOOM,
  PROP_SHOW_TOOLTIP,
  PROP_SHOW_SCROLLBAR,
  PROP_DOUBLE_BUFFERED,
  PROP_SCROLL_LINES,
  PROP_OVERLAY_ENABLED,
  PROP_OVERLAY_COLOR,
  PROP_OVERLAY_OUTLINE_COLOR,
  N_PROPERTIES
};

struct OverviewPrefs_
{
  GObject  parent;

  guint         width;
  gint          zoom;
  gboolean      show_tt;
  gboolean      show_sb;
  gboolean      dbl_buf;
  gint          scr_lines;
  gboolean      ovl_en;
  OverviewColor ovl_clr;
  OverviewColor out_clr;
};

struct OverviewPrefsClass_
{
  GObjectClass parent_class;
};

static void overview_prefs_finalize     (GObject      *object);
static void overview_prefs_set_property (GObject      *object,
                                         guint         prop_id,
                                         const GValue *value,
                                         GParamSpec   *pspec);
static void overview_prefs_get_property (GObject      *object,
                                         guint         prop_id,
                                         GValue       *value,
                                         GParamSpec   *pspec);


static GParamSpec *pspecs[N_PROPERTIES] = { NULL };

G_DEFINE_TYPE (OverviewPrefs, overview_prefs, G_TYPE_OBJECT)

static void
overview_prefs_class_init (OverviewPrefsClass *klass)
{
  GObjectClass *g_object_class;

  g_object_class = G_OBJECT_CLASS (klass);

  g_object_class->finalize     = overview_prefs_finalize;
  g_object_class->set_property = overview_prefs_set_property;
  g_object_class->get_property = overview_prefs_get_property;

  pspecs[PROP_WIDTH] = g_param_spec_uint ("width", "Width", "Width of the overview", 16, 512, 120, G_PARAM_CONSTRUCT | G_PARAM_READWRITE);
  pspecs[PROP_ZOOM] = g_param_spec_int ("zoom", "Zoom", "Zoom level of the view", -10, 20, -10, G_PARAM_CONSTRUCT | G_PARAM_READWRITE);
  pspecs[PROP_SHOW_TOOLTIP] = g_param_spec_boolean ("show-tooltip", "ShowTooltip", "Whether to show informational tooltip over the overview", TRUE, G_PARAM_CONSTRUCT | G_PARAM_READWRITE);
  pspecs[PROP_SHOW_SCROLLBAR] = g_param_spec_boolean ("show-scrollbar", "ShowScrollbar", "Whether to show the normal editor scrollbar", TRUE, G_PARAM_CONSTRUCT | G_PARAM_READWRITE);
  pspecs[PROP_DOUBLE_BUFFERED] = g_param_spec_boolean ("double-buffered", "DoubleBuffered", "Whether the overview drawing is double-buffered", TRUE, G_PARAM_CONSTRUCT | G_PARAM_READWRITE);
  pspecs[PROP_SCROLL_LINES] = g_param_spec_uint ("scroll-lines", "ScrollLines", "The number of lines to scroll the overview by", 1, 512, 1, G_PARAM_CONSTRUCT | G_PARAM_READWRITE);
  pspecs[PROP_OVERLAY_ENABLED] = g_param_spec_boolean ("overlay-enabled", "OverlayEnabled", "Whether an overlay is drawn overtop the overview", TRUE, G_PARAM_CONSTRUCT | G_PARAM_READWRITE);
  pspecs[PROP_OVERLAY_COLOR] = g_param_spec_boxed ("overlay-color", "OverlayColor", "The color of the overlay", OVERVIEW_TYPE_COLOR, G_PARAM_CONSTRUCT | G_PARAM_READWRITE);
  pspecs[PROP_OVERLAY_OUTLINE_COLOR] = g_param_spec_boxed ("overlay-outline-color", "OverlayOutlineColor", "The color of the outlines drawn around the overlay", OVERVIEW_TYPE_COLOR, G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

  g_object_class_install_properties (g_object_class, N_PROPERTIES, pspecs);
}

static void
overview_prefs_finalize (GObject *object)
{
  OverviewPrefs *self;

  g_return_if_fail (OVERVIEW_IS_PREFS (object));

  self = OVERVIEW_PREFS (object);

  G_OBJECT_CLASS (overview_prefs_parent_class)->finalize (object);
}

static void
overview_prefs_set_property (GObject      *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  OverviewPrefs *self = OVERVIEW_PREFS (object);

  switch (prop_id)
    {
    case PROP_WIDTH:
      self->width = g_value_get_uint (value);
      break;
    case PROP_ZOOM:
      self->zoom = g_value_get_int (value);
      break;
    case PROP_SHOW_TOOLTIP:
      self->show_tt = g_value_get_boolean (value);
      break;
    case PROP_SHOW_SCROLLBAR:
      self->show_sb = g_value_get_boolean (value);
      break;
    case PROP_DOUBLE_BUFFERED:
      self->dbl_buf = g_value_get_boolean (value);
      break;
    case PROP_SCROLL_LINES:
      self->scr_lines = g_value_get_uint (value);
      break;
    case PROP_OVERLAY_ENABLED:
      self->ovl_en = g_value_get_boolean (value);
      break;
    case PROP_OVERLAY_COLOR:
    {
      OverviewColor *src = g_value_get_boxed (value);
      memcpy (&self->ovl_clr, src, sizeof (OverviewColor));
      break;
    }
    case PROP_OVERLAY_OUTLINE_COLOR:
    {
      OverviewColor *src = g_value_get_boxed (value);
      memcpy (&self->out_clr, src, sizeof (OverviewColor));
      break;
    }
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
overview_prefs_get_property (GObject      *object,
                             guint         prop_id,
                             GValue       *value,
                             GParamSpec   *pspec)
{
  OverviewPrefs *self = OVERVIEW_PREFS (object);

  switch (prop_id)
    {
    case PROP_WIDTH:
      g_value_set_uint (value, self->width);
      break;
    case PROP_ZOOM:
      g_value_set_int (value, self->zoom);
      break;
    case PROP_SHOW_TOOLTIP:
      g_value_set_boolean (value, self->show_tt);
      break;
    case PROP_SHOW_SCROLLBAR:
      g_value_set_boolean (value, self->show_sb);
      break;
    case PROP_DOUBLE_BUFFERED:
      g_value_set_boolean (value, self->dbl_buf);
      break;
    case PROP_SCROLL_LINES:
      g_value_set_uint (value, self->scr_lines);
      break;
    case PROP_OVERLAY_ENABLED:
      g_value_set_boolean (value, self->ovl_en);
      break;
    case PROP_OVERLAY_COLOR:
      g_value_set_boxed (value, &self->ovl_clr);
      break;
    case PROP_OVERLAY_OUTLINE_COLOR:
      g_value_set_boxed (value, &self->out_clr);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
overview_prefs_init (OverviewPrefs *self)
{
}


OverviewPrefs *
overview_prefs_new (void)
{
  return g_object_new (OVERVIEW_TYPE_PREFS, NULL);
}

gboolean
overview_prefs_load (OverviewPrefs *self,
                     const gchar   *filename,
                     GError       **error)
{
  gchar  *contents = NULL;
  gsize   size = 0;
  g_return_val_if_fail (OVERVIEW_IS_PREFS (self), FALSE);
  g_return_val_if_fail (filename != NULL, FALSE);
  if (! g_file_get_contents (filename, &contents, &size, error))
    return FALSE;
  if (! overview_prefs_from_data (self, contents, size, error))
    {
      g_free (contents);
      return FALSE;
    }
  g_free (contents);
  return TRUE;
}

gboolean
overview_prefs_save (OverviewPrefs *self,
                     const gchar   *filename,
                     GError       **error)

{
  gchar *contents = NULL;
  gsize  size = 0;
  g_return_val_if_fail (OVERVIEW_IS_PREFS (self), FALSE);
  g_return_val_if_fail (filename != NULL, FALSE);
  contents = overview_prefs_to_data (self, &size, error);
  if (contents == NULL)
    return FALSE;
  if (! g_file_set_contents (filename, contents, size, error))
    {
      g_free (contents);
      return FALSE;
    }
  g_free (contents);
  return TRUE;
}

#define GET(T, k, v)                                   \
  do {                                                 \
    v = g_key_file_get_##T (kf, "overview", k, error); \
    if (error != NULL && *error != NULL) {             \
      g_key_file_free (kf);                            \
      return FALSE;                                    \
    } else {                                           \
      g_object_notify (G_OBJECT (self), "k");          \
    }                                                  \
  } while (0)

gboolean
overview_prefs_from_data (OverviewPrefs *self,
                          const gchar   *contents,
                          gssize         size,
                          GError       **error)
{
  GKeyFile *kf;

  g_return_val_if_fail (OVERVIEW_IS_PREFS (self), FALSE);
  g_return_val_if_fail (contents != NULL, FALSE);

  kf = g_key_file_new ();

  if (! g_key_file_load_from_data (kf, contents, size,
                                   G_KEY_FILE_KEEP_COMMENTS |
                                     G_KEY_FILE_KEEP_TRANSLATIONS,
                                  error))
    {
      g_key_file_free (kf);
      return FALSE;
    }

  GET (uint64,  "width",           self->width);
  GET (integer, "zoom",            self->zoom);
  GET (boolean, "show-tooltip",    self->show_tt);
  GET (boolean, "show-scrollbar",  self->show_sb);
  GET (boolean, "double-buffered", self->dbl_buf);
  GET (uint64,  "scroll-lines",    self->scr_lines);
  GET (boolean, "overlay-enabled", self->ovl_en);

  if (! overview_color_from_keyfile (&self->ovl_clr, kf, "overview", "overlay", error))
    {
      g_key_file_free (kf);
      return FALSE;
    }
  g_object_notify (G_OBJECT (self), "overlay-color");

  if (! overview_color_from_keyfile (&self->out_clr, kf, "overview", "overlay-outline", error))
    {
      g_key_file_free (kf);
      return FALSE;
    }
  g_object_notify (G_OBJECT (self), "overlay-outline-color");

  g_key_file_free (kf);

  return TRUE;
}

#define SET(T, k, v) g_key_file_set_##T (kf, "overview", k, v)

gchar *
overview_prefs_to_data (OverviewPrefs *self,
                        gsize         *size,
                        GError       **error)
{
  GKeyFile *kf;
  gchar    *contents;

  g_return_val_if_fail (OVERVIEW_IS_PREFS (self), NULL);

  kf = g_key_file_new ();

  SET (uint64,  "width",           self->width);
  SET (integer, "zoom",            self->zoom);
  SET (boolean, "show-tooltip",    self->show_tt);
  SET (boolean, "show-scrollbar",  self->show_sb);
  SET (boolean, "double-buffered", self->dbl_buf);
  SET (uint64,  "scroll-lines",    self->scr_lines);
  SET (boolean, "overlay-enabled", self->ovl_en);

  overview_color_to_keyfile (&self->ovl_clr, kf, "overview", "overlay");
  overview_color_to_keyfile (&self->out_clr, kf, "overview", "overlay-outline");

  contents = g_key_file_to_data (kf, size, error);
  g_key_file_free (kf);
  return contents;
}

#define BIND(prop) \
  g_object_bind_property (self, prop, sci, prop, G_BINDING_SYNC_CREATE)

void
overview_prefs_bind_scintilla (OverviewPrefs *self,
                               GObject       *sci)
{
  g_return_if_fail (OVERVIEW_IS_PREFS (self));
  g_return_if_fail (OVERVIEW_IS_SCINTILLA (sci));

  BIND ("width");
  BIND ("zoom");
  BIND ("show-tooltip");
  BIND ("show-scrollbar");
  BIND ("double-buffered");
  BIND ("scroll-lines");
  BIND ("overlay-enabled");
  BIND ("overlay-color");
  BIND ("overlay-outline-color");
}

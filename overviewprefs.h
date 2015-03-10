#ifndef OVERVIEWPREFS_H_
#define OVERVIEWPREFS_H_ 1

#include <glib-object.h>

G_BEGIN_DECLS

#define OVERVIEW_TYPE_PREFS            (overview_prefs_get_type ())
#define OVERVIEW_PREFS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), OVERVIEW_TYPE_PREFS, OverviewPrefs))
#define OVERVIEW_PREFS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), OVERVIEW_TYPE_PREFS, OverviewPrefsClass))
#define OVERVIEW_IS_PREFS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OVERVIEW_TYPE_PREFS))
#define OVERVIEW_IS_PREFS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), OVERVIEW_TYPE_PREFS))
#define OVERVIEW_PREFS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), OVERVIEW_TYPE_PREFS, OverviewPrefsClass))

typedef struct OverviewPrefs_      OverviewPrefs;
typedef struct OverviewPrefsClass_ OverviewPrefsClass;

GType          overview_prefs_get_type       (void);
OverviewPrefs *overview_prefs_new            (void);
gboolean       overview_prefs_load           (OverviewPrefs *prefs,
                                              const gchar   *filename,
                                              GError       **error);
gboolean       overview_prefs_save           (OverviewPrefs *prefs,
                                              const gchar   *filename,
                                              GError       **error);
gboolean       overview_prefs_from_data      (OverviewPrefs *prefs,
                                              const gchar   *contents,
                                              gssize         size,
                                              GError       **error);
gchar*         overview_prefs_to_data        (OverviewPrefs *prefs,
                                              gsize         *size,
                                              GError       **error);
void           overview_prefs_bind_scintilla (OverviewPrefs *prefs,
                                              GObject       *sci);

#define OVERVIEW_PREFS_DEFAULT_CONFIG   \
    "[overview]\n"                      \
    "width = 120\n"                     \
    "zoom = -10\n"                      \
    "show-tooltip = true\n"             \
    "double-buffered = true\n"          \
    "scroll-lines = 1\n"                \
    "show-scrollbar = true\n"           \
    "overlay-enabled = true\n"          \
    "overlay-color = #000000\n"         \
    "overlay-alpha = 0.25\n"            \
    "overlay-outline-color = #000000\n" \
    "overlay-outline-alpha = 0.75\n"    \
    "\n"

G_END_DECLS

#endif /* OVERVIEWPREFS_H_ */

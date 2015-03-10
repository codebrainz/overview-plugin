#ifndef OVERVIEWPREFSPANEL_H_
#define OVERVIEWPREFSPANEL_H_ 1

#include "overviewprefs.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define OVERVIEW_TYPE_PREFS_PANEL            (overview_prefs_panel_get_type ())
#define OVERVIEW_PREFS_PANEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), OVERVIEW_TYPE_PREFS_PANEL, OverviewPrefsPanel))
#define OVERVIEW_PREFS_PANEL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), OVERVIEW_TYPE_PREFS_PANEL, OverviewPrefsPanelClass))
#define OVERVIEW_IS_PREFS_PANEL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OVERVIEW_TYPE_PREFS_PANEL))
#define OVERVIEW_IS_PREFS_PANEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), OVERVIEW_TYPE_PREFS_PANEL))
#define OVERVIEW_PREFS_PANEL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), OVERVIEW_TYPE_PREFS_PANEL, OverviewPrefsPanelClass))

typedef struct OverviewPrefsPanel_      OverviewPrefsPanel;
typedef struct OverviewPrefsPanelClass_ OverviewPrefsPanelClass;

GType      overview_prefs_panel_get_type (void);
GtkWidget *overview_prefs_panel_new      (OverviewPrefs *prefs,
                                          GtkDialog     *host_dialog);

G_END_DECLS

#endif /* OVERVIEWPREFSPANEL_H_ */

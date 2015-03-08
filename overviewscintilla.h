#ifndef OVERVIEWSCINTILLA_H_
#define OVERVIEWSCINTILLA_H_ 1

#include <geanyplugin.h>

G_BEGIN_DECLS

#define OVERVIEW_TYPE_SCINTILLA            (overview_scintilla_get_type ())
#define OVERVIEW_SCINTILLA(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), OVERVIEW_TYPE_SCINTILLA, OverviewScintilla))
#define OVERVIEW_SCINTILLA_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), OVERVIEW_TYPE_SCINTILLA, OverviewScintillaClass))
#define OVERVIEW_IS_SCINTILLA(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OVERVIEW_TYPE_SCINTILLA))
#define OVERVIEW_IS_SCINTILLA_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), OVERVIEW_TYPE_SCINTILLA))
#define OVERVIEW_SCINTILLA_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), OVERVIEW_TYPE_SCINTILLA, OverviewScintillaClass))

typedef struct OverviewScintilla_        OverviewScintilla;
typedef struct OverviewScintillaClass_   OverviewScintillaClass;

GType      overview_scintilla_get_type (void);
GtkWidget *overview_scintilla_new      (ScintillaObject *src_sci);

G_END_DECLS

#endif /* OVERVIEWSCINTILLA_H_ */

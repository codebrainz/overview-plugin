#ifndef OVERVIEWSCINTILLA_H_
#define OVERVIEWSCINTILLA_H_ 1

#include "overviewplugin.h"

G_BEGIN_DECLS

#define OVERVIEW_TYPE_SCINTILLA            (overview_scintilla_get_type ())
#define OVERVIEW_SCINTILLA(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), OVERVIEW_TYPE_SCINTILLA, OverviewScintilla))
#define OVERVIEW_SCINTILLA_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), OVERVIEW_TYPE_SCINTILLA, OverviewScintillaClass))
#define OVERVIEW_IS_SCINTILLA(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OVERVIEW_TYPE_SCINTILLA))
#define OVERVIEW_IS_SCINTILLA_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), OVERVIEW_TYPE_SCINTILLA))
#define OVERVIEW_SCINTILLA_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), OVERVIEW_TYPE_SCINTILLA, OverviewScintillaClass))

typedef struct OverviewScintilla_        OverviewScintilla;
typedef struct OverviewScintillaClass_   OverviewScintillaClass;

typedef struct
{
  gdouble red;
  gdouble green;
  gdouble blue;
  gdouble alpha;
}
OverviewColor;

GType overview_color_get_type (void);
#define OVERVIEW_TYPE_COLOR (overview_color_get_type ())

GType         overview_scintilla_get_type                  (void);
GtkWidget    *overview_scintilla_new                       (ScintillaObject     *src_sci);
GdkCursorType overview_scintilla_get_cursor                (OverviewScintilla   *sci);
void          overview_scintilla_set_cursor                (OverviewScintilla   *sci,
                                                            GdkCursorType        cursor_type);
void          overview_scintilla_get_visible_rect          (OverviewScintilla   *sci,
                                                            GdkRectangle        *rect);
void          overview_scintilla_set_visible_rect          (OverviewScintilla   *sci,
                                                            const GdkRectangle  *rect);
gint          overview_scintilla_get_zoom                  (OverviewScintilla   *sci);
void          overview_scintilla_set_zoom                  (OverviewScintilla   *sci,
                                                            gint                 zoom);
gboolean      overview_scintilla_get_show_tooltip          (OverviewScintilla   *sci);
void          overview_scintilla_set_show_tooltip          (OverviewScintilla   *sci,
                                                            gboolean             show);
gboolean      overview_scintilla_get_overlay_enabled       (OverviewScintilla   *sci);
void          overview_scintilla_set_overlay_enabled       (OverviewScintilla   *sci,
                                                            gboolean             enabled);
void          overview_scintilla_get_overlay_color         (OverviewScintilla   *sci,
                                                            OverviewColor       *color);
void          overview_scintilla_set_overlay_color         (OverviewScintilla   *sci,
                                                            const OverviewColor *color);
void          overview_scintilla_get_overlay_outline_color (OverviewScintilla   *sci,
                                                            OverviewColor       *color);
void          overview_scintilla_set_overlay_outline_color (OverviewScintilla   *sci,
                                                            const OverviewColor *color);
gboolean      overview_scintilla_get_double_buffered       (OverviewScintilla   *sci);
void          overview_scintilla_set_double_buffered       (OverviewScintilla   *sci,
                                                            gboolean             enabled);
gint         overview_scintilla_get_scroll_lines           (OverviewScintilla   *sci);
void         overview_scintilla_set_scroll_lines           (OverviewScintilla   *sci,
                                                            gint                 lines);

G_END_DECLS

#endif /* OVERVIEWSCINTILLA_H_ */

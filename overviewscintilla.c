#include "overviewscintilla.h"
#include "overviewplugin.h"

#define OVERVIEW_SCINTILLA_CURSOR GDK_CROSS

enum
{
  PROP_0,
  PROP_SCINTILLA,
  N_PROPERTIES,
};

struct OverviewScintilla_
{
  ScintillaObject  parent;
  ScintillaObject *sci;    // source scintilla
  GtkWidget       *canvas; // internal GtkDrawingArea of scintilla
};

struct OverviewScintillaClass_
{
  ScintillaClass parent_class;
};

static GParamSpec *pspecs[N_PROPERTIES] = { NULL };

static void overview_scintilla_finalize     (GObject           *object);
static void overview_scintilla_set_property (GObject           *object,
                                             guint              prop_id,
                                             const GValue      *value,
                                             GParamSpec        *pspec);
static void overview_scintilla_get_property (GObject           *object,
                                             guint              prop_id,
                                             GValue            *value,
                                             GParamSpec        *pspec);
static void overview_scintilla_set_src_sci  (OverviewScintilla *self,
                                             ScintillaObject   *sci);

G_DEFINE_TYPE (OverviewScintilla, overview_scintilla, scintilla_get_type())

static void
overview_scintilla_class_init (OverviewScintillaClass *klass)
{
  GObjectClass *g_object_class;

  g_object_class = G_OBJECT_CLASS (klass);

  g_object_class->finalize     = overview_scintilla_finalize;
  g_object_class->set_property = overview_scintilla_set_property;
  g_object_class->get_property = overview_scintilla_get_property;

  pspecs[PROP_SCINTILLA] =
    g_param_spec_object ("scintilla",
                         "Scintilla",
                         "The source ScintillaObject",
                         scintilla_get_type (),
                         G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

  g_object_class_install_property (g_object_class, PROP_SCINTILLA, pspecs[PROP_SCINTILLA]);
}

static void
overview_scintilla_finalize (GObject *object)
{
  OverviewScintilla *self;

  g_return_if_fail (OVERVIEW_IS_SCINTILLA (object));

  self = OVERVIEW_SCINTILLA (object);

  g_object_unref (self->sci);

  G_OBJECT_CLASS (overview_scintilla_parent_class)->finalize (object);
}

static gboolean
on_focus_in_event (OverviewScintilla *self,
                   GdkEventFocus     *event,
                   ScintillaObject   *sci)
{

  scintilla_send_message (SCINTILLA (self), SCI_SETREADONLY, 1, 0);
  return FALSE;
}

static gboolean
on_focus_out_event (OverviewScintilla *self,
                    GdkEventFocus     *event,
                    ScintillaObject   *sci)
{
  GeanyDocument *doc = g_object_get_data (G_OBJECT (sci), "document");
  if (DOC_VALID (doc))
    scintilla_send_message (SCINTILLA (self), SCI_SETREADONLY, doc->readonly, 0);
  else
    scintilla_send_message (SCINTILLA (self), SCI_SETREADONLY, 0, 0);
  return FALSE;
}

static gboolean
on_enter_notify_event (OverviewScintilla *self,
                       GdkEventCrossing  *event,
                       ScintillaObject   *sci)
{
  return FALSE;
}

static gboolean
on_leave_notify_event (OverviewScintilla *self,
                       GdkEventCrossing  *event,
                       ScintillaObject   *sci)
{
  return FALSE;
}

static void
on_each_child (GtkWidget *child,
               GList    **list)
{
  *list = g_list_prepend (*list, child);
}

static GList *
gtk_container_get_internal_children_ (GtkContainer *cont)
{
  GList *list = NULL;
  gtk_container_forall (cont, (GtkCallback) on_each_child, &list);
  return g_list_reverse (list);
}

static GtkWidget *
overview_scintilla_find_drawing_area (GtkWidget *root)
{
  GtkWidget *da = NULL;
  if (GTK_IS_DRAWING_AREA (root))
    da = root;
  else if (GTK_IS_CONTAINER (root))
    {
      GList *children = gtk_container_get_internal_children_ (GTK_CONTAINER (root));
      for (GList *iter = children; iter != NULL; iter = g_list_next (iter))
        {
          da = overview_scintilla_find_drawing_area (iter->data);
          if (GTK_IS_DRAWING_AREA (da))
            break;
        }
      g_list_free (children);
    }
  return da;
}

static gboolean
on_map_event (OverviewScintilla *self,
              GdkEventAny       *event,
              ScintillaObject   *sci)
{
  self->canvas = overview_scintilla_find_drawing_area (GTK_WIDGET (self));
  if (GTK_IS_WIDGET (self->canvas))
    {
      gdk_window_set_cursor (gtk_widget_get_window (self->canvas),
                             gdk_cursor_new (OVERVIEW_SCINTILLA_CURSOR));
    }
  return FALSE;
}

#define self_connect(name, cb) \
  g_signal_connect_swapped (self, name, G_CALLBACK (cb), self)

static void
overview_scintilla_init (OverviewScintilla *self)
{
  self->sci    = NULL;
  self->canvas = NULL;
  gtk_widget_add_events (GTK_WIDGET (self),
                         GDK_FOCUS_CHANGE_MASK |
                         GDK_ENTER_NOTIFY_MASK |
                         GDK_STRUCTURE_MASK);
  self_connect ("focus-in-event", on_focus_in_event);
  self_connect ("focus-out-event", on_focus_out_event);
  self_connect ("enter-notify-event", on_enter_notify_event);
  self_connect ("leave-notify-event", on_leave_notify_event);
  self_connect ("map-event", on_map_event);
}

static void
overview_scintilla_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  OverviewScintilla *self = OVERVIEW_SCINTILLA (object);

  switch (prop_id)
    {
    case PROP_SCINTILLA:
      overview_scintilla_set_src_sci (self, g_value_get_object (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
overview_scintilla_get_property (GObject      *object,
                                 guint         prop_id,
                                 GValue       *value,
                                 GParamSpec   *pspec)
{
  OverviewScintilla *self = OVERVIEW_SCINTILLA (object);

  switch (prop_id)
    {
    case PROP_SCINTILLA:
      g_value_set_object (value, self->sci);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

GtkWidget *
overview_scintilla_new (ScintillaObject *src_sci)
{
  return g_object_new (OVERVIEW_TYPE_SCINTILLA, "scintilla", src_sci, NULL);
}

static gchar *
sci_get_font (ScintillaObject *sci,
              gint             style)
{
  gsize  len;
  gchar *name;

  len = scintilla_send_message (sci, SCI_STYLEGETFONT, style, 0);
  name = g_malloc0 (len + 1);
  scintilla_send_message (sci, SCI_STYLEGETFONT, style, (sptr_t) name);

  return name;
}

static void
overview_scintilla_clone_styles (OverviewScintilla *self)
{
  ScintillaObject *sci     = SCINTILLA (self);
  ScintillaObject *src_sci = self->sci;

  for (gint i = 0; i < STYLE_MAX; i++)
    {
      gchar   *font_name = sci_get_font (src_sci, i);
      gint     font_size = scintilla_send_message (src_sci, SCI_STYLEGETSIZE, i, 0);
      gint     weight    = scintilla_send_message (src_sci, SCI_STYLEGETWEIGHT, i, 0);
      gboolean italic    = scintilla_send_message (src_sci, SCI_STYLEGETITALIC, i, 0);
      gint     fg_color  = scintilla_send_message (src_sci, SCI_STYLEGETFORE, i, 0);
      gint     bg_color  = scintilla_send_message (src_sci, SCI_STYLEGETBACK, i, 0);

      scintilla_send_message (sci, SCI_STYLESETFONT, i, (sptr_t) font_name);
      scintilla_send_message (sci, SCI_STYLESETSIZE, i, font_size);
      scintilla_send_message (sci, SCI_STYLESETWEIGHT, i, weight);
      scintilla_send_message (sci, SCI_STYLESETITALIC, i, italic);
      scintilla_send_message (sci, SCI_STYLESETFORE, i, fg_color);
      scintilla_send_message (sci, SCI_STYLESETBACK, i, bg_color);
      scintilla_send_message (sci, SCI_STYLESETCHANGEABLE, i, 0);
      //scintilla_send_message (sci, SCI_STYLESETHOTSPOT, i, 1);

      g_free (font_name);
    }
}

static void
overview_scintilla_update_sci (OverviewScintilla *self)
{
  ScintillaObject *src_sci = self->sci;
  ScintillaObject *sci     = SCINTILLA (self);
  sptr_t           doc_ptr;

  doc_ptr = scintilla_send_message (src_sci, SCI_GETDOCPOINTER, 0, 0);
  scintilla_send_message (sci, SCI_SETDOCPOINTER, 0, doc_ptr);

  overview_scintilla_clone_styles (self);

  for (gint i = 0; i < SC_MAX_MARGIN; i++)
    scintilla_send_message (sci, SCI_SETMARGINWIDTHN, i, 0);

  scintilla_send_message (sci, SCI_SETVIEWEOL, 0, 0);
  scintilla_send_message (sci, SCI_SETVIEWWS, 0, 0);
  scintilla_send_message (sci, SCI_SETHSCROLLBAR, 0, 0);
  scintilla_send_message (sci, SCI_SETVSCROLLBAR, 0, 0);
  scintilla_send_message (sci, SCI_SETZOOM, -20, 0);
  scintilla_send_message (sci, SCI_SETCURSOR, SC_CURSORARROW, 0);
}

static void
overview_scintilla_set_src_sci (OverviewScintilla *self,
                                ScintillaObject   *sci)
{
  if (sci != self->sci)
    {
      if (IS_SCINTILLA (self->sci))
        g_object_unref (self->sci);
      self->sci = g_object_ref (sci);
      overview_scintilla_update_sci (self);
      g_object_notify (G_OBJECT (self), "scintilla");
    }
}

#ifndef OVERVIEW_PLUGIN_H
#define OVERVIEW_PLUGIN_H

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <geanyplugin.h>

extern GeanyData      *geany_data;
extern GeanyPlugin    *geany_plugin;
extern GeanyFunctions *geany_functions;

#ifndef _
# define _(s) (s)
#endif

#endif // OVERVIEW_PLUGIN_H

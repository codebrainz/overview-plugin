plugindir = $(GEANY_PLUGINDIR)
plugin_LTLIBRARIES = overview.la

AM_CFLAGS = \
	$(GEANY_CFLAGS) \
	-I$(top_srcdir) \
	-DOVERVIEW_PREFS_UI_FILE=\""$(pkgdatadir)/prefs.ui"\"

overview_la_SOURCES = \
	overviewcolor.c \
	overviewcolor.h \
	overviewplugin.c \
	overviewplugin.h \
	overviewprefs.c \
	overviewprefs.h \
	overviewprefspanel.c \
	overviewprefspanel.h \
	overviewscintilla.c \
	overviewscintilla.h

overview_la_LDFLAGS = -module -avoid-version $(GEANY_LIBS)

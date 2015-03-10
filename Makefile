PREFIX = ${HOME}/.config/geany/plugins
PLUGINDIR = $(PREFIX)/overview

sources = \
	overviewplugin.c \
	overviewscintilla.c \
	overviewcolor.c \
	overviewprefs.c \
	overviewprefspanel.c

objects = $(sources:.c=.o)

cflags = $(CFLAGS) -g -fPIC -std=c99 $(shell pkg-config --cflags geany) \
	-DOVERVIEW_PREFS_UI_FILE=\""$(PLUGINDIR)/prefs.ui"\" \
	-DOVERVIEW_PREFS_CONFIG_FILE=\""$(PLUGINDIR)/prefs.conf"\"

ldflags = $(LDFLAGS) $(shell pkg-config --libs geany)

all: overview.so

clean:
	$(RM) *.o *.so

install:
	mkdir -p $(PLUGINDIR)/
	cp overview.so $(PREFIX)/
	cp prefs.ui $(PLUGINDIR)/

overview.so: $(objects)
	$(CC) -shared $(cflags) -o $@ $(objects) $(ldflags)

.c.o:
	$(CC) -c $(cflags) -o $@ $<

-include Makefile.deps
Makefile.deps: $(sources)
	$(CC) -MM $(cflags) $(sources) > $@

.PHONY: all clean

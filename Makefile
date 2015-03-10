PREFIX = ~/.config/geany/plugins/
PLUGINDIR = $(PREFIX)/overview

sources = \
	overviewplugin.c \
	overviewscintilla.c \
	overviewcolor.c \
	overviewprefs.c \
	overviewprefspanel.c

objects = $(sources:.c=.o)

cflags = $(CFLAGS) -g -fPIC -std=c99 $(shell pkg-config --cflags geany) \
	-DOVERVIEW_PREFS_UI_FILE=\""$(PLUGINDIR)/prefs.glade"\" \
	-DOVERVIEW_PREFS_CONFIG_FILE=\""$(PLUGINDIR)/prefs.conf"\"

ldflags = $(LDFLAGS) $(shell pkg-config --libs geany)

all: overview.so

clean:
	$(RM) *.o *.so

install:
	cp overview.so $(PREFIX)/
	cp prefs.glade $(PLUGINDIR)/

overview.so: $(objects)
	$(CC) -shared $(cflags) -o $@ $(objects) $(ldflags)

.c.o:
	$(CC) -c $(cflags) -o $@ $<

.PHONY: all clean

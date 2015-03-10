sources = \
	overviewplugin.c \
	overviewscintilla.c \
	overviewcolor.c \
	overviewprefs.c

objects = $(sources:.c=.o)
cflags = $(CFLAGS) -g -fPIC -std=c99 $(shell pkg-config --cflags geany) \
	-DOVERVIEW_PREFS_UI_FILE=\""~/.config/geany/plugins/overview/prefs.glade"\"
ldflags = $(LDFLAGS) $(shell pkg-config --libs geany)

all: overview.so

clean:
	$(RM) *.o *.so

install:
	cp overview.so ~/.config/geany/plugins/
	cp prefs.glade ~/.config/geany/plugins/overview/

overview.so: $(objects)
	$(CC) -shared $(cflags) -o $@ $(objects) $(ldflags)

.c.o:
	$(CC) -c $(cflags) -o $@ $<

.PHONY: all clean

sources = overviewplugin.c overviewscintilla.c
objects = $(sources:.c=.o)
cflags = $(CFLAGS) -g -fPIC -std=c99 $(shell pkg-config --cflags geany)
ldflags = $(LDFLAGS) $(shell pkg-config --libs geany)

all: overview.so

clean:
	$(RM) *.o *.so

install:
	cp overview.so ~/.config/geany/plugins/

overview.so: $(objects)
	$(CC) -shared $(cflags) -o $@ $(objects) $(ldflags)

.c.o:
	$(CC) -c $(cflags) -o $@ $<

.PHONY: all clean

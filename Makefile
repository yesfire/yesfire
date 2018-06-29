VERSION = 0.6

PREFIX = /usr/local
MANPREFIX = $(PREFIX)/man

#CPPFLAGS = -DDEBUG
#CFLAGS = -g
LDLIBS = -lcurses
DISTFILES = yesfire.c strlcat.c strlcpy.c util.h config.def.h\
    yesfire.1 Makefile README LICENSE
OBJ = yesfire.o strlcat.o strlcpy.o
BIN = yesfire

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS) $(LDLIBS)

yesfire.o: util.h config.h
strlcat.o: util.h
strlcpy.o: util.h

config.h:
	cp config.def.h $@

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f $(BIN) $(DESTDIR)$(PREFIX)/bin
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	cp -f $(BIN).1 $(DESTDIR)$(MANPREFIX)/man1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(BIN)
	rm -f $(DESTDIR)$(MANPREFIX)/man1/$(BIN).1

dist:
	mkdir -p yesfire-$(VERSION)
	cp $(DISTFILES) yesfire-$(VERSION)
	tar -cf yesfire-$(VERSION).tar yesfire-$(VERSION)
	gzip yesfire-$(VERSION).tar
	rm -rf yesfire-$(VERSION)

clean:
	rm -f $(BIN) $(OBJ) yesfire-$(VERSION).tar.gz

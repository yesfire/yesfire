NAME = "Yesfire"
VERSION = 42.0 #Version number is not an Answer. It shows the best time to use it.
URL = "http://github.com/yesfire/yesfire"
DESCRIPTION = "Minimalistic yet powerful multicolumn file manager"

DISTFILES = yesfire.c yesfire.h\
            yesfire.1 Makefile README LICENSE
OBJ = yesfire.o
BIN = yesfire

LDLIBS = -lcurses
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/man

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS) $(LDLIBS)

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


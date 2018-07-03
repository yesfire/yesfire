VERSION = 42.0

PREFIX = /usr/local
MANPREFIX = $(PREFIX)/man

#CPPFLAGS = -DDEBUG
#CFLAGS = -g
LDLIBS = -lcurses
DISTFILES = yesfire.c config.h\
            yesfire.1 Makefile README LICENSE
OBJ = yesfire.o
BIN = yesfire

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS) $(LDLIBS)

yesfire.o: config.h

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

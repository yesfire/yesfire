NAME = "Yesfire"
VERSION = 42.0 #Version number is not an Answer. It shows the best time to use it.
URL = "https://github.com/yesfire/yesfire"
DESCRIPTION = "Minimalistic yet powerful multicolumn file manager"
SUCCESS = "Build completed successfully. Feuer Frei!"

DISTFILES = yesfire.c yesfire.h config.h\
            yesfire.1 Makefile README LICENSE
OBJ = yesfire.o
BIN = yesfire

CCOUTPUTFLAG = -o
LDLIBS = -lcurses
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/man

MKDIR = mkdir -p
CP = cp -f
RM = rm -f
TAR = tar -cf
ZIP = gzip
RMDIR = rm -rf
ECHO = @echo

BINDIR = /bin
MANDIR = /man1

TAREXT = .tar
ZIPEXT = .gz
MANEXT = .1

DVERSION = -$(VERSION)
DBIN = /$(BIN)

all: $(BIN) report

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(CCOUTPUTFLAG) $@ $(OBJ) $(LDFLAGS) $(LDLIBS)

report:
	$(ECHO) $(NAME) $(VERSION)
	$(ECHO) $(DESCRIPTION)
	$(ECHO) $(URL)
	$(ECHO) $(SUCCESS)

install: all
	$(MKDIR) $(DESTDIR)$(PREFIX)$(BINDIR)
	$(CP) $(BIN) $(DESTDIR)$(PREFIX)$(BINDIR)
	$(MKDIR) $(DESTDIR)$(MANPREFIX)$(MANDIR)
	$(CP) $(BIN)$(MANEXT) $(DESTDIR)$(MANPREFIX)$(MANDIR)

uninstall:
	$(RM) $(DESTDIR)$(PREFIX)$(BINDIR)$(DBIN)
	$(RM) $(DESTDIR)$(MANPREFIX)$(MANDIR)$(DBIN)$(MANEXT)

dist:
	$(MKDIR) $(BIN)$(DVERSION)
	$(CM) $(DISTFILES) $(BIN)$(DVERSION)
	$(TAR) $(BIN)$(DVERSION)$(TAREXT) $(DBIN)$(DVERSION)
	$(ZIP) $(BIN)$(DVERSION)$(TAREXT)
	$(RMDIR) $(BIN)$(DVERSION)

clean:
	$(RM) $(BIN) $(OBJ) $(BIN)$(DVERSION)$(TAREXT)$(ZIPEXT)


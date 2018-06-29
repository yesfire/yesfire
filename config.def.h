/* See LICENSE file for copyright and license details. */
#ifndef CONFIG_H
#define CONFIG_H

#define CWD   "You are here: "
#define CURSR " -> "
#define EMPTY "   "

int mtimeorder  = 1; /* Set to 1 to sort by time modified */
int idletimeout = 0; /* Screensaver timeout in seconds, 0 to disable */
int showhidden  = 1; /* Set to 1 to show hidden files by default */
char *idlecmd   = "fortune | cowsay"; /* The screensaver program */
#define NCOLS 4 /* Number of columns */

struct assoc assocs[] = {
	{ "\\.(avi|mp4|mkv|mp3|ogg|flac|mov)$", "mplayer" },
	{ "\\.(png|jpg|jpeg|gif|bmp)$", "feh" },
	{ "\\.(html|htm|svg)$", "firefox" },
	{ "\\.(djvu|fb2)$", "FBReader" },
	{ "\\.pdf$", "mupdf" },
	{ "\\.(sh|zsh)$", "zsh" },
	{ ".", "less" },
};

char* fast_dirs[] = {
    "/home",
    "/root",
    "/usr/ports",
    "/var/log",
    "/var/www",
    "/",
    "/usr/src",
    "/mnt",
    "/tmp",
    "/etc"
};

struct key bindings[] = {
	/* Quit */
	{ 'q',            SEL_QUIT },
	/* Back */
	{ KEY_BACKSPACE,  SEL_BACK },
	{ KEY_LEFT,       SEL_BACK },
	{ 'h',            SEL_BACK },
	{ CONTROL('H'),   SEL_BACK },
	/* Inside */
	{ KEY_ENTER,      SEL_GOIN },
	{ '\r',           SEL_GOIN },
	{ KEY_RIGHT,      SEL_GOIN },
	{ 'l',            SEL_GOIN },
	/* Filter */
	{ '/',            SEL_FLTR },
	{ '&',            SEL_FLTR },
	/* Next */
	{ 'j',            SEL_NEXT },
	{ KEY_DOWN,       SEL_NEXT },
	{ CONTROL('N'),   SEL_NEXT },
	/* Previous */
	{ 'k',            SEL_PREV },
	{ KEY_UP,         SEL_PREV },
	{ CONTROL('P'),   SEL_PREV },
	/* Page down */
	{ KEY_NPAGE,      SEL_PGDN },
	{ CONTROL('D'),   SEL_PGDN },
	/* Page up */
	{ KEY_PPAGE,      SEL_PGUP },
	{ CONTROL('U'),   SEL_PGUP },
	/* Home */
	{ KEY_HOME,       SEL_HOME },
	{ META('<'),      SEL_HOME },
	{ '^',            SEL_HOME },
	/* End */
	{ KEY_END,        SEL_END },
	{ META('>'),      SEL_END },
	{ '$',            SEL_END },
	/* Change dir */
	{ 'c',            SEL_CD },
	{ '~',            SEL_CDHOME },
	/* Toggle hide .dot files */
	{ '.',            SEL_TOGGLEDOT },
	/* Toggle sort by time */
	{ 't',            SEL_MTIME },
	{ CONTROL('L'),   SEL_REDRAW },
	/* Next col */
    { ' ', SEL_NEXTCOL},
    { 'L', SEL_NEXTCOL},
    { 'H', SEL_PREVCOL},
    { '0',            SEL_FASTDIR},
    { '1',            SEL_FASTDIR},
    { '2',            SEL_FASTDIR},
    { '3',            SEL_FASTDIR},
    { '4',            SEL_FASTDIR},
    { '5',            SEL_FASTDIR},
    { '6',            SEL_FASTDIR},
    { '7',            SEL_FASTDIR},
    { '8',            SEL_FASTDIR},
    { '9',            SEL_FASTDIR},
	/* Run command */
	{ 'z',            SEL_RUN, "htop" },
	{ 'i',            SEL_RUN, "zsh", "SHELL" },
	/* Run command with argument */
	{ 'e',            SEL_RUNARG, "gvim", "EDITOR" },
	{ 'p',            SEL_RUNARG, "less", "PAGER" },
};
#endif /* CONFIG_H */

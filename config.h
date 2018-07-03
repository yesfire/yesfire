/* See LICENSE file for copyright and license details. */
#ifndef CONFIG_H
#define CONFIG_H

#define CWD   "You are here: "
#define CURSR " -> "
#define NTFY  "[!] "
#define EMPTY "    "

int mtimeorder  = 1; /* Set to 1 to sort by time modified */
int autosorttimeout = 0; /* Set colum autosort timeout in seconds */
int idletimeout = 0; /* Screensaver timeout in seconds, 0 to disable */
int showhidden  = 1; /* Set to 1 to show hidden files by default */
char *idlecmd   = "screenfetch"; /* The screensaver program */

char* status_line_format = "$MDATE $NAME $OWNER $PERMISSION $SIZE"; /* */

#define NCOLS 4 /* Default number of columns */
#define MAX_COLS 32 /* Max number of columns */
#define STACK_SIZE 500 /* Max stack size for STACK MODE*/

int file_ops_style = DEFAULT_FILE_OPS; /* File operations style: DEFAULT_FILE_OPS, USE_SYSTEM_BINARIES, BRUTEFORCE666*/
int ongoin = YF_VIEW; /* Default behaviour: YF_VIEW YF_EDIT YF_RUN */

assoc_t assocs_view[] = {
	{ "\\.(avi|mp4|mkv|mp3|ogg|flac|mov|mpeg)$", {"mplayer", "mpv", "vlc","mplayer2", "xine"}},
	{ "\\.(png|jpg|jpeg|gif|bmp|svg|tga|apng)$", {"feh"}},
	{ "\\.(html|htm)$", {"firefox", "chrome", "uzbl", "lynx", "falkon"}},
	{ "\\.(djvu|fb2)$", {"FBReader"}},
	{ "\\.pdf$", {"mupdf"}},
	{ "\\.dvi$", {"xdvi"}},
	{ "\\.(sh|zsh|py|c|h|php|cpp|txt|asm|js|css)$", {"vi", "vim", "nano", "emacs", "ed"}},
};

assoc_t assocs_edit[] = {
	{ "\\.(avi|mp4|mkv|mp3|ogg|flac|mov)$", {"mplayer", "mpv", "vlc","mplayer2", "xine"}},
	{ "\\.(png|jpg|jpeg|gif|bmp|svg)$", {"feh", "gimp", "inkscape"}},
	{ "\\.(html|htm)$", {"firefox", "chrome", "uzbl", "lynx"}},
	{ "\\.(djvu|fb2)$", {"FBReader"}},
	{ "\\.pdf$", {"mupdf"}},
	{ "\\.dvi$", {"xdvi"}},
	{ "\\.(sh|zsh|py|c|h|php|cpp|txt|asm|js|css|)$",{"vi", "vim", "nano", "emacs", "ed"}},
};

assoc_t assocs_run[] = {
	{ "\\.(sh|zsh)$",  {"zsh", "bash", "csh", "sh", "ksh"}},
	{ "\\.(py)$", {"pyhon"}},
	{ "\\.(pl)$", {"perl"}},
    { "\\.(c)$",  {"gcc", "clang", "cc", "tcc"}},
    { "\\.(erl)$", {"erlang"}},
	{ "\\.(php)$", {"php"}},
	{ "\\.(asm)$", {"fasm"}},
	{ "\\.(js)$", {"", "", "", "", ""}},
	{ "\\.(o)$", {"ld"}},
	{ "\\.(exe|bat)$", {""}},
    {"(Makefile)$", {"make", "gmake", "portmaster"}},
};

sysfileops_t binaryassocs[] = {
    {SYS_RMFILE, "rm"},
    {SYS_RMDIR, "rm -r"},
    {SYS_RENAME, "mv"},
    {SYS_MOVE, "mv"},
    {SYS_MKDIR, "mkdir"},
    {SYS_TOUCH, "touch"},
    {SYS_COPYFILE, "cp"},
    {SYS_COPYDIR, "cp -r"},
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

yf_key_t bindings[] = {
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
	/* Columns manipulation */
    { ' ',            SEL_NEXTCOL},
    { META('N'),      SEL_NEXTCOL},
    { 'L',            SEL_NEXTCOL},
    { 'H',            SEL_PREVCOL},
    { META('P'),      SEL_PREVCOL},
    { 'a',            SEL_ADDCOL},
    { 'o',            SEL_ADDCOL},
    { 'x',            SEL_REMOVECOL},
    { 'f',            SEL_SORTCOL},
    /* Fast directories */
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
    /* Stack mode */
	{ '\t',           SEL_STACKMODE},
	{ 'd',            SEL_STACKMODE},
	/* Run command */
	{ 'z',            SEL_RUN, "htop" },
	{ 'i',            SEL_RUN, "zsh", "SHELL" },
	/* Run command with argument */
	{ 'e',            SEL_RUNARG, "gvim", "EDITOR" },
	{ 'p',            SEL_RUNARG, "less", "PAGER" },
};

yf_smkey_t smbindings[] = {
    /* File operations */
    {'d',             STACK_DELFILE},
    {'D',             STACK_DELDIR},
    {'r',             STACK_RENAMEFILE},
    {'R',             STACK_RENAMEDIR},
    {'m',             STACK_MOVEFILE},
    {'M',             STACK_MOVEDIR},
    {'D',             STACK_DELDIR},
    {'D',             STACK_DELDIR},
    {'D',             STACK_DELDIR},

    /* Standard stack primitives */
    { 'S',            STACK_SWAPDIR},
    { 's',            STACK_SWAPFILE},
    { '>',            STACK_DROPDIR},
    { '.',            STACK_DROPFILE},
    { 'U',            STACK_DUPDIR},
    { 'u',            STACK_DUPFILE},
    { 'O',            STACK_OVERDIR},
    { 'o',            STACK_OVERFILE},
    { 'T',            STACK_ROTDIR},
    { 't',            STACK_ROTFILE},
    { 'p',            STACK_PICKDIR},
    { 'P',            STACK_PICKFILE},
    /* Meta */
    {'v',             STACK_SHOWCWDSEL},
    {'f',             STACK_NORMALMODE},
    {'\t',            STACK_NORMALMODE},
    {'\n',            STACK_NORMALMODE},
    {' ',             STACK_NORMALMODE},

};

#endif /* CONFIG_H */

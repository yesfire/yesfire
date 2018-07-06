/* See LICENSE file for copyright and license details. */
#ifndef CONFIG_H
#define CONFIG_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CWD   "You are here: "
#define CURSR " -> "
#define NTFY  "[!] "
#define EMPTY "    "

int mtimeorder  = 1; /* Set to 1 to sort by time modified */
int autosorttimeout = 0; /* Set colum autosort timeout in seconds */
int idletimeout = 0; /* Screensaver timeout in seconds, 0 to disable */
int showhidden  = 1; /* Set to 1 to show hidden files by default */

char *favourite_shell = "zsh"; /* Your favourite shell */
char *idlecmd   = "screenfetch"; /* The screensaver program */
char* status_line_format = "$MDATE $NAME $OWNER $PERMISSION $SIZE :ereht era yehT"; /* */

#define NCOLS 4 /* Default number of columns */
#define STACK_SIZE 500 /* Max stack size for STACK MODE*/

int file_ops_style = DEFAULT_FILE_OPS; /* File operations style: DEFAULT_FILE_OPS, USE_SYSTEM_BINARIES, BRUTEFORCE666*/
int favourite_behaviour = YF_VIEW; /* Default behaviour: YF_VIEW YF_EDIT YF_RUN */

assoc_t assocs_view[] = {
	{ "\\.(avi|mp4|mkv|mp3|ogg|flac|mov|mpeg)$", {"mplayer", "mpv", "vlc","mplayer2", "xine"}},
	{ "\\.(png|jpg|jpeg|gif|bmp|svg|tga|apng|ico)$", {"feh"}},
	{ "\\.(html|htm)$", {"firefox", "chrome", "uzbl", "lynx", "falkon"}},
	{ "\\.(djvu|fb2)$", {"FBReader"}},
	{ "\\.pdf$", {"mupdf"}},
	{ "\\.dvi$", {"xdvi"}},
	{ "\\.(sh|zsh|py|c|h|php|cpp|txt|asm|js|css)$", {"vi", "vim", "nano", "emacs", "ed"}},
	{ ".", {"less"}},
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
	{ 'q',            SEL_QUIT  ,0},
	/* Back */
	{ KEY_BACKSPACE,  SEL_BACK  ,0},
	{ KEY_LEFT,       SEL_BACK  ,0},
	{ 'h',            SEL_BACK  ,0},
	{ CONTROL('H'),   SEL_BACK  ,0},
	/* Inside */
	{ KEY_ENTER,      SEL_GOIN  ,0},
	{ '\r',           SEL_GOIN  ,0},
	{ KEY_RIGHT,      SEL_GOIN  ,0},
	{ 'l',            SEL_GOIN  ,0},
	/* Filter */
	{ '/',            SEL_FLTR  ,0},
	{ '&',            SEL_FLTR  ,0},
	/* Next */
	{ 'j',            SEL_NEXT  ,0},
	{ KEY_DOWN,       SEL_NEXT  ,0},
	{ CONTROL('N'),   SEL_NEXT  ,0},
	/* Previous */
	{ 'k',            SEL_PREV  ,0},
	{ KEY_UP,         SEL_PREV  ,0},
	{ CONTROL('P'),   SEL_PREV  ,0},
	/* Page down */
	{ KEY_NPAGE,      SEL_PGDN  ,0},
	{ CONTROL('D'),   SEL_PGDN  ,0},
	/* Page up */
	{ KEY_PPAGE,      SEL_PGUP  ,0},
	{ CONTROL('U'),   SEL_PGUP  ,0},
	/* Home */
	{ KEY_HOME,       SEL_HOME  ,0},
	{ META('<'),      SEL_HOME  ,0},
	{ '^',            SEL_HOME  ,0},
	/* End */
	{ KEY_END,        SEL_END  ,0},
	{ META('>'),      SEL_END  ,0},
	{ '$',            SEL_END  ,0},
	/* Change dir */
	{ 'c',            SEL_CD  ,0},
	{ '~',            SEL_CDHOME  ,0},
	/* Toggle hide .dot files */
	{ '.',            SEL_TOGGLEDOT  ,0},
	/* Toggle sort by time */
	{ 't',            SEL_MTIME  ,0},
	{ CONTROL('L'),   SEL_REDRAW  ,0},
	/* Columns manipulation */
    { ' ',            SEL_NEXTCOL ,0},
    { META('N'),      SEL_NEXTCOL ,0},
    { 'L',            SEL_NEXTCOL ,0},
    { 'H',            SEL_PREVCOL ,0},
    { META('P'),      SEL_PREVCOL ,0},
    { 'a',            SEL_ADDCOL ,0},
    { 'o',            SEL_ADDCOL ,0},
    { 'x',            SEL_REMOVECOL ,0},
    { 'f',            SEL_SORTCOL ,0},
    /* Fast directories */
    { '0',            SEL_FASTDIR ,0},
    { '1',            SEL_FASTDIR ,0},
    { '2',            SEL_FASTDIR ,0},
    { '3',            SEL_FASTDIR ,0},
    { '4',            SEL_FASTDIR ,0},
    { '5',            SEL_FASTDIR ,0},
    { '6',            SEL_FASTDIR ,0},
    { '7',            SEL_FASTDIR ,0},
    { '8',            SEL_FASTDIR ,0},
    { '9',            SEL_FASTDIR ,0},
    /* Stack mode */
	{ '\t',           SEL_STACKMODE ,0},
	{ 'd',            SEL_STACKMODE ,0},
	/* Main action command */
	{ 'v',            SEL_VIEW   ,0},
	{ '!',            SEL_RUN   ,0},
	{ 'e',            SEL_EDIT   ,0},
	{ 'i',            SEL_SHELL   ,0},
	/* Run command with argument */
};

binbinding_t binbindings[] = {
    {'z', "htop"},
    {'m', "mutt"},
    {'M', "irssi"},

};

yf_key_t smbindings[] = {
    /* File operations */
    {'d',0,             STACK_DELFILE},
    {'D',0,             STACK_DELDIR},
    {'r',0,             STACK_RENAMEFILE},
    {'R',0,             STACK_RENAMEDIR},
    {'m',0,             STACK_MOVEFILE},
    {'M',0,             STACK_MOVEDIR},
    {'D',0,             STACK_DELDIR},
    {'D',0,             STACK_DELDIR},
    {'D',0,             STACK_DELDIR},

    /* Standard stack primitives */
    { 'S',0,            STACK_SWAPDIR},
    { 's',0,            STACK_SWAPFILE},
    { '>',0,            STACK_DROPDIR},
    { '.',0,            STACK_DROPFILE},
    { 'U',0,            STACK_DUPDIR},
    { 'u',0,            STACK_DUPFILE},
    { 'O',0,            STACK_OVERDIR},
    { 'o',0,            STACK_OVERFILE},
    { 'T',0,            STACK_ROTDIR},
    { 't',0,            STACK_ROTFILE},
    { 'p',0,            STACK_PICKDIR},
    { 'P',0,            STACK_PICKFILE},
    /* Meta */
    {'v',0,             STACK_SHOWCWDSEL},
    {'f',0,             STACK_NORMALMODE},
    {'\t',0,            STACK_NORMALMODE},
    {'\n',0,            STACK_NORMALMODE},
    {' ',0,             STACK_NORMALMODE},

};

#endif /* CONFIG_H */


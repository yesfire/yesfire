/* See LICENSE file for copyright and license details. */
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <curses.h>

#include <unistd.h>
#include <dirent.h>

#include <fcntl.h>
#include <libgen.h>

#include <regex.h>

#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#ifdef DEBUG
#define DEBUG_FD 8
#define DPRINTF_D(x) dprintf(DEBUG_FD, #x "=%d\n", x)
#define DPRINTF_U(x) dprintf(DEBUG_FD, #x "=%u\n", x)
#define DPRINTF_S(x) dprintf(DEBUG_FD, #x "=%s\n", x)
#define DPRINTF_P(x) dprintf(DEBUG_FD, #x "=0x%p\n", x)
#else
#define DPRINTF_D(x)
#define DPRINTF_U(x)
#define DPRINTF_S(x)
#define DPRINTF_P(x)
#endif /* DEBUG */

#define LEN(x) (sizeof(x) / sizeof(*(x)))
#undef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define ISODD(x) ((x) & 1)
#define CONTROL(c) ((c) ^ 0x40)
#define META(c) ((c) ^ 0x80)

#define SWAP(x,y) do \
   { unsigned char swap_temp[sizeof(x) == sizeof(y) ? (signed)sizeof(x) : -1]; \
     memcpy(swap_temp,&y,sizeof(x)); \
     memcpy(&y,&x,       sizeof(x)); \
     memcpy(&x,swap_temp,sizeof(x)); \
    } while(0)


#ifndef PATH_MAX
#define PATH_MAX 256
#endif
#ifndef LINE_MAX
#define LINE_MAX 128
#endif

#define MAX_ASSOCS_RESERVE 10
typedef struct {
	char *regex; /* Regex to match on filename */
	char *bin[MAX_ASSOCS_RESERVE];   /* Program */
}assoc_t;

/* Supported actions */
typedef enum  {
	SEL_QUIT = 1,
	SEL_BACK,
	SEL_GOIN,
	SEL_FLTR,
	SEL_NEXT,
	SEL_PREV,
	SEL_PGDN,
	SEL_PGUP,
	SEL_HOME,
	SEL_END,
	SEL_CD,
	SEL_CDHOME,
	SEL_TOGGLEDOT,
	SEL_MTIME,
	SEL_REDRAW,
	SEL_RUN,
	SEL_RUNARG,
	SEL_NEXTCOL,
	SEL_PREVCOL,
	SEL_FASTDIR,
	SEL_ADDCOL,
	SEL_REMOVECOL,
	SEL_SORTCOL,
	SEL_STACKMODE,
}action_t;

typedef enum {
    STACK_DELFILE = 1,
    STACK_DELDIR,
    STACK_RENAMEFILE,
    STACK_RENAMEDIR,
    STACK_MOVEFILE,
    STACK_MOVEDIR,
    STACK_MAKEDIR,
    STACK_TOUCHFILE,
    STACK_COPYFILE,
    STACK_COPYDIR,
    STACK_PUSHCOLCWD,
    STACK_PUSHCOLCURDIR,
    STACK_PUSHCOLCURFILE,
    STACK_SWAPFILE,
    STACK_SWAPDIR,
    STACK_DROPDIR,
    STACK_DROPFILE,
    STACK_DUPDIR,
    STACK_DUPFILE,
    STACK_OVERDIR,
    STACK_OVERFILE,
    STACK_ROTDIR,
    STACK_ROTFILE,
    STACK_PICKDIR,
    STACK_PICKFILE,
    STACK_SHOWCWDSEL,
    STACK_NORMALMODE,
}stack_mode_action_t;

typedef enum {
    SYS_RMFILE,
    SYS_RMDIR,
    SYS_RENAME,
    SYS_MOVE,
    SYS_MKDIR,
    SYS_TOUCH,
    SYS_COPYFILE,
    SYS_COPYDIR,
}sysfileops_type_t;

typedef struct {
    sysfileops_type_t type;
    char* bin;
}sysfileops_t;

typedef struct {
	int sym;         /* Key pressed */
	action_t act; /* Action */
	char *run;       /* Program to run */
	char *env;       /* Environment variable to run */
}yf_key_t;

typedef struct {
    int sym;
    stack_mode_action_t act;
}yf_smkey_t;

typedef struct {
    int enumc;
    char keyp;
}key_out_t;

typedef enum {
    DEFAULT_FILE_OPS,
    USE_SYSTEM_BINARIES,
    BRUTEFORCE666
}file_ops_styles_t;

typedef enum {
    YF_VIEW,
    YF_EDIT,
    YF_RUN
}default_behaviour_types_t;

#include "config.h"
typedef struct {
	char name[PATH_MAX];
	time_t t;
	mode_t mode;
	char notify;
	char dummy;
}entry_t;

typedef struct {
    int ndents,cur,total_notifications;
    entry_t *dents;
    char path[PATH_MAX], oldpath[PATH_MAX], newpath[PATH_MAX];
}column_t;


#define MAX_STATUS_LINE 128
typedef struct {
    column_t cols[MAX_COLS];
    short curcol, totalcols;
    int idle;
    time_t lastupdatetimestamp;

	char fltr[LINE_MAX];
	char fastdir[PATH_MAX];

    entry_t directory_stack;
    entry_t files_stack;

    char status_line[MAX_STATUS_LINE];
}filemanager_t;

static char* user_strings[] = {
    "error opening terminal: %s\n",
    "failed to initialize curses\n",
    "minimum 80 cols terminal please, its 4 panel file viewer\n",
    "minimum 20 cols per panel please.\n",
    "No association",
    "Unsupported file",
    "filter(aka regex): ",
    "Change dir where?: ",
    "Yesfire 42.0: Some men just want to watch the fil(r)es OPENED.\n",
    "usage: %s [-e stack_mode_command] [-c num_cols] [dir 0] ... [dir %d]\n",
    "stdin or stdout is not a tty\n",
    "wrong cols number, must be in [1..%d]\n",
};

typedef enum {
    STR_ERR_TERMINAL_OPEN,
    STR_ERR_CURSES_INIT_FAILED,
    STR_ERR_RESIZE_FAILED,
    STR_ERR_RESIZE_FAILED2,
    STR_ERROR_NO_ASSOC,
    STR_ERR_UNSUPPORTED_FILE,
    STR_PROMPT_REGEX,
    STR_PROMPT_CD,
    STR_VERSION,
    STR_USAGE,
    STR_ERR_NOTATTY,
    STR_ERR_COLS_ARG_FAILED
}user_strings_list_t;

void printmsg(char *);
void printwarn(void);
void printerr(int, char *);

static filemanager_t*
get_filemanager()
{
    static filemanager_t fire;
    return &fire;
}

static column_t*
getcurrentcolumn(filemanager_t* fm)
{
    return &fm->cols[fm->curcol];
}

static int
getcurrentcolumnndents(filemanager_t* fm)
{
    return fm->cols[fm->curcol].ndents;
}


static entry_t*
getcurrentry(filemanager_t* fm)
{
    column_t* c = getcurrentcolumn(fm);
    return &c->dents[c->cur];
}


static char*
getcurrentryname(filemanager_t* fm)
{
    column_t* c = getcurrentcolumn(fm);
    return c->dents[c->cur].name;
}

static mode_t
getcurrentrymode(filemanager_t* fm)
{
    column_t* c = getcurrentcolumn(fm);
    return c->dents[c->cur].mode;
}


static int
gettotalcols()
{
    filemanager_t* fm = get_filemanager();
    return fm->totalcols;
}


/* ========================================================================= */
/*
 * Copyright (c) 1998, 2015 Todd C. Miller <Todd.Miller@courtesan.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Appends src to string dst of size dsize (unlike strncat, dsize is the
 * full size of dst, not space left).  At most dsize-1 characters
 * will be copied.  Always NUL terminates (unless dsize <= strlen(dst)).
 * Returns strlen(src) + MIN(dsize, strlen(initial dst)).
 * If retval >= dsize, truncation occurred.
 */

size_t
strlcat(char *dst, const char *src, size_t dsize)
{
	const char *odst = dst;
	const char *osrc = src;
	size_t n = dsize;
	size_t dlen;

	/* Find the end of dst and adjust bytes left but don't go past end. */
	while (n-- != 0 && *dst != '\0')
		dst++;
	dlen = dst - odst;
	n = dsize - dlen;

	if (n-- == 0)
		return(dlen + strlen(src));
	while (*src != '\0') {
		if (n != 0) {
			*dst++ = *src;
			n--;
		}
		src++;
	}
	*dst = '\0';

	return(dlen + (src - osrc));	/* count does not include NUL */
}

/*
 * Copy string src to buffer dst of size dsize.  At most dsize-1
 * chars will be copied.  Always NUL terminates (unless dsize == 0).
 * Returns strlen(src); if retval >= dsize, truncation occurred.
 */
size_t
strlcpy(char *dst, const char *src, size_t dsize)
{
	const char *osrc = src;
	size_t nleft = dsize;

	/* Copy as many bytes as will fit. */
	if (nleft != 0) {
		while (--nleft != 0) {
			if ((*dst++ = *src++) == '\0')
				break;
		}
	}

	/* Not enough room in dst, add NUL and traverse rest of src. */
	if (nleft == 0) {
		if (dsize != 0)
			*dst = '\0';		/* NUL-terminate dst */
		while (*src++)
			;
	}

	return(src - osrc - 1);	/* count does not include NUL */
}
/* ========================================================================= */

#undef dprintf
int
dprintf(int fd, const char *fmt, ...)
{
	char buf[BUFSIZ];
	int r;
	va_list ap;

	va_start(ap, fmt);
	r = vsnprintf(buf, sizeof(buf), fmt, ap);
	if (r > 0)
		write(fd, buf, r);
	va_end(ap);
	return r;
}

void *
xmalloc(size_t size)
{
	void *p;

	p = malloc(size);
	if (p == NULL)
		printerr(1, "malloc");
	return p;
}

void *
xrealloc(void *p, size_t size)
{
	p = realloc(p, size);
	if (p == NULL)
		printerr(1, "realloc");
	return p;
}

char *
xstrdup(const char *s)
{
	char *p;

	p = strdup(s);
	if (p == NULL)
		printerr(1, "strdup");
	return p;
}

/* Some implementations of dirname(3) may modify `path' and some
 * return a pointer inside `path'. */
char *
xdirname(const char *path)
{
	static char out[PATH_MAX];
	char tmp[PATH_MAX], *p;

	strlcpy(tmp, path, sizeof(tmp));
	p = dirname(tmp);
	if (p == NULL)
		printerr(1, "dirname");
	strlcpy(out, p, sizeof(out));
	return out;
}

void
spawn(char *file, char *arg, char *dir)
{
	pid_t pid;
	int status;
	pid = fork();
	if (pid == 0) {
		if (dir != NULL)
			chdir(dir);
		execlp(file, file, arg, NULL);
		_exit(1);
	} else {
		/* Ignore interruptions */
		while (waitpid(pid, &status, 0) == -1)
			DPRINTF_D(status);
		DPRINTF_D(pid);
	}
}

char *
xgetenv(char *name, char *fallback)
{
	char *value;

	if (name == NULL)
		return fallback;
	value = getenv(name);
	return value && value[0] ? value : fallback;
}

char *
openwith(char *file)
{
	regex_t regex;
	char *bin = NULL;
	int i;

	for (i = 0; i < LEN(assocs_view); i++) {
		if (regcomp(&regex, assocs_view[i].regex,
			    REG_NOSUB | REG_EXTENDED | REG_ICASE) != 0)
			continue;
		if (regexec(&regex, file, 0, NULL, 0) == 0) {
			bin = assocs_view[i].bin[0];
			regfree(&regex);
			break;
		}
		regfree(&regex);
	}
	DPRINTF_S(bin);
	return bin;
}

int
setfilter(regex_t *regex, char *filter)
{
	char errbuf[LINE_MAX];
	size_t len;
	int r;

	r = regcomp(regex, filter, REG_NOSUB | REG_EXTENDED | REG_ICASE);
	if (r != 0) {
		len = COLS;
		if (len > sizeof(errbuf))
			len = sizeof(errbuf);
		regerror(r, regex, errbuf, len);
		printmsg(errbuf);
	}
	return r;
}

void
freefilter(regex_t *regex)
{
	regfree(regex);
}

void
initfilter(int dot, char **ifilter)
{
	*ifilter = dot ? "." : "^[^.]";
}

int
visible(regex_t *regex, char *file)
{
	return regexec(regex, file, 0, NULL, 0) == 0;
}

int
entrycmp(const void *va, const void *vb)
{
	const entry_t *a = va, *b = vb;

	if (mtimeorder)
		return b->t - a->t;
	return strcmp(a->name, b->name);
}

void
initcurses(int totalcols)
{
	char *term;

	if (initscr() == NULL) {
		term = getenv("TERM");
		if (term != NULL)
			fprintf(stderr,  user_strings[STR_ERR_TERMINAL_OPEN], term);
		else
			fprintf(stderr, "%s", user_strings[STR_ERR_CURSES_INIT_FAILED]);
		exit(1);
	}
	if (COLS < 80) {fprintf(stderr,"%s", user_strings[STR_ERR_RESIZE_FAILED]); endwin(); exit(1);}
	if (COLS/totalcols < 20) {fprintf(stderr, "%s", user_strings[STR_ERR_RESIZE_FAILED2]); endwin(); exit(1);}

	cbreak();
	noecho();
	nonl();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);
	curs_set(FALSE); /* Hide cursor */
	timeout(1000); /* One second */
}

void
exitcurses(void)
{
	endwin(); /* Restore terminal */
}

/* Messages show up at the bottom */
void
printmsg(char *msg)
{
	move(LINES - 1, 0);
	printw("%s\n", msg);
}

/* Display warning as a message */
void
printwarn(void)
{
	printmsg(strerror(errno));
}

/* Kill curses and display error before exiting */
void
printerr(int ret, char *prefix)
{
	exitcurses();
	fprintf(stderr, "%s: %s\n", prefix, strerror(errno));
	exit(ret);
}

/* Clear the last line */
void
clearprompt(void)
{
	printmsg("");
}

/* Print prompt on the last line */
void
printprompt(char *str)
{
	clearprompt();
	printw(str);
}

int xgetch(void)
{
	int c;
    filemanager_t* fm = get_filemanager();
	c = getch();
	if (c == -1)
		fm->idle++;
	else
		fm->idle = 0;
	return c;
}

/* Returns SEL_* if key is bound and 0 otherwise.
 * Also modifies the run and env pointers (used on SEL_{RUN,RUNARG}) */
key_out_t
nextsel(char **run, char **env)
{
	int c, i;
	key_out_t out;

	c = xgetch();
	if (c == 033)
		c = META(xgetch());

	for (i = 0; i < LEN(bindings); i++)
		if (c == bindings[i].sym) {
			*run = bindings[i].run;
			*env = bindings[i].env;
			out.enumc = bindings[i].act;
		}
	out.keyp = c;
	return out;
}

char *
readln(void)
{
	static char ln[LINE_MAX];

	timeout(-1);
	echo();
	curs_set(TRUE);
	memset(ln, 0, sizeof(ln));
	wgetnstr(stdscr, ln, sizeof(ln) - 1);
	noecho();
	curs_set(FALSE);
	timeout(1000);
	return ln[0] ? ln : NULL;
}

int
canopendir(char *path)
{
	DIR *dirp;

	dirp = opendir(path);
	if (dirp == NULL)
		return 0;
	closedir(dirp);
	return 1;
}

char *
mkpath(char *dir, char *name, char *out, size_t n)
{
	/* Handle absolute path */
	if (name[0] == '/') {
		strlcpy(out, name, n);
	} else {
		/* Handle root case */
		if (strcmp(dir, "/") == 0) {
			strlcpy(out, "/", n);
			strlcat(out, name, n);
		} else {
			strlcpy(out, dir, n);
			strlcat(out, "/", n);
			strlcat(out, name, n);
		}
	}
	return out;
}

int
get_delta(int i)
{
    int delta;
    filemanager_t* fm = get_filemanager();
    int ndents = fm->cols[i].ndents;
    int cur = fm->cols[i].cur;
    int nlines = MIN(LINES - 4, ndents);

    if (cur < nlines / 2) {
        delta = 0;

    } else if (cur >= ndents - nlines / 2) {
        delta = ndents -nlines;

    }
    else {
        delta = cur - nlines /2;
    }

    return delta;
}
void
getentline(entry_t ent[MAX_COLS], int index, int j)
{
    int i = 0;
    int delta = 0;
    filemanager_t* fm = get_filemanager();
    for (i =0;i<fm->totalcols;++i) {
        if (i==fm->curcol) {
            if (j<fm->cols[i].ndents){
                    ent[i] = fm->cols[i].dents[j+get_delta(i)];
            }
            else {
                ent[i].name[0]='\0';
                ent[i].mode=0;
                ent[i].t=0;
                ent[i].dummy=1;
                ent[i].notify=0;
            }
        }
        else {
            if (j<fm->cols[i].ndents){
                delta = get_delta(i);
                ent[i] = fm->cols[i].dents[j+delta];
            }
            else {
                ent[i].name[0]='\0';
                ent[i].mode=0;
                ent[i].t=0;
                ent[i].dummy=1;
                ent[i].notify=0;
            }
        }
    }

    return;
}

void
printentline(entry_t ent[MAX_COLS], int ind)
{
  	char name[PATH_MAX];
    int totalcols = gettotalcols();
	unsigned int maxlen = COLS/totalcols - strlen(CURSR) - 1;
    int i = 0;
    filemanager_t* fm = get_filemanager();
    for (i=0;i<PATH_MAX;++i) name[i]=0;
    int j = 0;
    for (i =0;i<totalcols;++i) {
        /* Copy name locally */
        char cm = 0;

        if (!ent[i].dummy) {
        strlcpy(name, ent[i].name, sizeof(name));
            if (S_ISDIR(ent[i].mode)) {
                cm = '/';
            } else if (S_ISLNK(ent[i].mode)) {
                cm = '@';
            } else if (S_ISSOCK(ent[i].mode)) {
                cm = '=';
            } else if (S_ISFIFO(ent[i].mode)) {
                cm = '|';
            } else if (ent[i].mode & S_IXUSR) {
                cm = '*';
            }
        }

        /* No text wrapping in entries */
        if (strlen(name) > maxlen)
            name[maxlen] = '\0';

        char line[2*maxlen]; for (j=0;j<2*maxlen;++j) line[j]=0;
        char lformat[10]; for (j=0;j<10;++j) lformat[j]=0;
        char align = 0; (i % 2 == 1) ? (align = '+') : (align = '-');

        sprintf(lformat, "%c%c%ds", '%', align, COLS/totalcols - 2);

        if (ent[i].dummy==0) {
            char* cursor;
            if (ind+get_delta(i)==fm->cols[i].cur)
                cursor = CURSR;
            else if (ent[i].notify)
                cursor = NTFY;
            else
                cursor = EMPTY;

            if (cm == 0)
                sprintf(line , "%s%s%c ", cursor , name, ' ');
            else
                sprintf(line , "%s%s%c ", cursor , name, cm);


            if (ind+get_delta(i)==fm->cols[i].cur && i==fm->curcol) attron(A_STANDOUT);

            if ((strlen(line) >=maxlen) && (cm!=0) && (line[maxlen-2]) !=cm)
                line[maxlen-1] = cm;
            line[maxlen]='\0';
            printw(lformat, line);
        }
        else if (ent[i].dummy==1){
            printw(lformat, " ");
        }
        attroff(A_STANDOUT);
        if (i==totalcols - 1) printw ("\n");
    }
}

int
dentfill(char *path, entry_t **dents,
	 int (*filter)(regex_t *, char *), regex_t *re)
{
	char newpath[PATH_MAX];
	size_t i = 0; for (i=0;i<PATH_MAX;++i) newpath[i]=0;
	DIR *dirp;
	struct dirent *dp;
	struct stat sb;
	int r, n = 0;

	dirp = opendir(path);
	if (dirp == NULL)
		return 0;

	while ((dp = readdir(dirp)) != NULL) {
		/* Skip self and parent */
		if (strcmp(dp->d_name, ".") == 0 ||
		    strcmp(dp->d_name, "..") == 0)
			continue;
		if (filter(re, dp->d_name) == 0)
			continue;
		*dents = xrealloc(*dents, (n + 1) * sizeof(**dents));
		strlcpy((*dents)[n].name, dp->d_name, sizeof((*dents)[n].name));
		/* Get mode flags */
		mkpath(path, dp->d_name, newpath, sizeof(newpath));
		r = lstat(newpath, &sb);
		if (r == -1)
			printerr(1, "lstat");
		(*dents)[n].mode = sb.st_mode;
		(*dents)[n].t = sb.st_mtime;
        (*dents)[n].dummy  = 0;
        (*dents)[n].notify  = 0;
		n++;
	}

	/* Should never be null */
	r = closedir(dirp);
	if (r == -1)
		printerr(1, "closedir");
	return n;
}

void
dentfree(entry_t* dents)
{
	free(dents);
}

/* Return the position of the matching entry or 0 otherwise */
int
dentfind(entry_t *dents, int n, char *cwd, char *path)
{
	char tmp[PATH_MAX];
	int i;

	if (path == NULL)
		return 0;
	for (i = 0; i < n; i++) {
		mkpath(cwd, dents[i].name, tmp, sizeof(tmp));
		DPRINTF_S(path);
		DPRINTF_S(tmp);
		if (strcmp(tmp, path) == 0)
			return i;
	}
	return 0;
}

int
populate(char *path, char *oldpath, char *fltr,char coli)
{
	regex_t re;
	int r;

    filemanager_t* fm = get_filemanager();
	/* Can fail when permissions change while browsing */
	if (canopendir(path) == 0)
		return -1;

	/* Search filter */
	r = setfilter(&re, fltr);
	if (r != 0)
		return -1;

	dentfree(fm->cols[coli].dents);

    fm->cols[coli].cur = 0;
	fm->cols[coli].ndents = 0;
	fm->cols[coli].dents = NULL;
    fm->cols[coli].total_notifications=0;

	fm->cols[coli].ndents = dentfill(path, &fm->cols[coli].dents, visible, &re);

	freefilter(&re);
	if (fm->cols[coli].ndents == 0)
		return 0; /* Empty result */

	qsort(fm->cols[coli].dents, fm->cols[coli].ndents, sizeof(*(fm->cols[coli].dents)), entrycmp);

	/* Find cur from history */
	fm->cols[coli].cur = dentfind(fm->cols[coli].dents, fm->cols[coli].ndents, path, oldpath);

	return 0;
}

void
redraw(filemanager_t* fm)
{
     if (LINES < 5) {erase(); return;}
     if ((COLS/(fm->totalcols)<15) && (fm->totalcols!=0)) {erase(); return;}

	char cwd[PATH_MAX], cwdresolved[PATH_MAX];
	size_t ntcols;
	int nlines, odd;
	int i,j;

    int maxndents = 0;
    for (i=0;i<PATH_MAX;++i) {cwd[i]=0; cwdresolved[i]=0;}
    for (i=0;i<fm->totalcols; ++i)
        if (fm->cols[i].ndents>maxndents)
            maxndents =fm->cols[i].ndents;

	nlines = MIN(LINES - 4, maxndents);

	/* Clean screen */
	erase();

	/* Strip trailing slashes */
	for (j=0;j<fm->totalcols;j++) {
	for (i = strlen(fm->cols[j].path) - 1; i > 0; i--)
		if (fm->cols[j].path[i] == '/')
			fm->cols[j].path[i] = '\0';
		else
			break;
    }
    int cur_curcol = fm->cols[fm->curcol].cur;
    char* cur_path = fm->cols[fm->curcol].path;
    int ndents = getcurrentcolumnndents(fm);

	DPRINTF_D(cur_curcol);
	DPRINTF_S(cur_path);

	/* No text wrapping in cwd line */
	ntcols = COLS;
    if (ntcols > PATH_MAX)
    	ntcols = PATH_MAX;
    strlcpy(cwd, cur_path, ntcols);
    cwd[ntcols - strlen(CWD) - 1] = '\0';
    realpath(cwd, cwdresolved);

   	printw(CWD "%s\n\n", cwdresolved);
	/* Print listing */
	odd = ISODD(nlines);
	j=0;
    entry_t line[MAX_COLS];
	if (cur_curcol < nlines / 2) {
		for (i = 0; i < nlines; i++) {
            getentline(line, i,i);
			printentline(line, i );
        }
	} else if (cur_curcol >= ndents - nlines / 2) {
		for (i = ndents - nlines; i < ndents; i++) {

            getentline(line, i, j);
			printentline(line,   j++);
        }
	} else {
	    j = 0;
		for (i = cur_curcol - nlines / 2;
		     i < cur_curcol + nlines / 2 + odd; i++) {
            getentline(line, i, j);
			printentline(line,  j++);
        }
	}

}

void
populate_current(filemanager_t* fm)
{
    column_t* curcol = getcurrentcolumn(fm);
    int r = populate(curcol->path, curcol->oldpath, fm->fltr, fm->curcol);
    if (r == -1) {
        printwarn();
        return;
    }
    return;
}

void
addcol(filemanager_t* fm)
{
    column_t* curcol = getcurrentcolumn(fm);
    column_t* cols = fm->cols;
    if (COLS/(fm->totalcols+1) < 20) {  return; }
    int i;
    if ((fm->totalcols  < MAX_COLS)) {
        if (curcol->ndents==0) {
            for (i=0;i<PATH_MAX;++i) {
                    cols[fm->totalcols].path[i] = curcol->path[i];
                    cols[fm->totalcols].oldpath[i]=curcol->oldpath[i];
            }
            int r = populate(cols[fm->totalcols].path, cols[fm->totalcols].oldpath, fm->fltr, fm->totalcols);
            if (r == -1) {
                printwarn();
                return;
            }
            fm->curcol = fm->totalcols;
            fm->totalcols++;
            return;
        }
        if (!S_ISDIR(getcurrentrymode(fm))) {
            for (i=0;i<PATH_MAX;++i) {
                 cols[fm->totalcols].path[i] = curcol->path[i];
                 cols[fm->totalcols].oldpath[i] = curcol->oldpath[i];
            }
        }
        else {
            mkpath(curcol->path, getcurrentryname(fm), cols[fm->totalcols].path , sizeof(cols[fm->totalcols].path));
        }

        int r = populate(cols[fm->totalcols].path, cols[fm->totalcols].oldpath, fm->fltr, fm->totalcols);
        if (r == -1) {
            printwarn();
            return;
        }
        fm->curcol = fm->totalcols;
        fm->totalcols++;

        }
        return;
}

void
removecol(filemanager_t* fm)
{
    int i,j;
    if (fm->totalcols > 1) {
        if (fm->curcol!=fm->totalcols-1) {
            for (i=fm->curcol;i<fm->totalcols;++i) {
                fm->cols[i].dents = fm->cols[i+1].dents;
                fm->cols[i].ndents =fm->cols[i+1].ndents;
                fm->cols[i].cur = fm->cols[i+1].cur;

                for (j=0;j<PATH_MAX;++j) {
                    fm->cols[i].path[j] = fm->cols[i+1].path[j];
                    fm->cols[i].oldpath[j]=fm->cols[i+1].oldpath[j];
                }
            }
        }
        else {
            fm->curcol = fm->totalcols - 2;
        }
        dentfree(fm->cols[fm->totalcols].dents);
        fm->cols[fm->totalcols].dents=NULL;
        fm->totalcols--;
    }
    return;
}

int
update_col(filemanager_t* fm, int index)
{
    populate(fm->cols[index].path, fm->cols[index].oldpath, fm->fltr, index);
    int i, flag =0;
    for (i=0;i<fm->cols[index].ndents;++i) {
        if (fm->cols[index].dents[i].t > fm->lastupdatetimestamp) {
            fm->cols[index].dents[i].notify = 1;
            fm->cols[index].total_notifications++;
            flag = 1;
        }
    }
    return flag;
}

int
cmp_cols(const void* c1, const void* c2)
{
    column_t* col1 = (column_t*) c1;
    column_t* col2 = (column_t*) c2;
    if (col1->total_notifications > col2->total_notifications) {
        return 1;
    }

    if (col1->total_notifications == col2->total_notifications) {
        return 0;
    }

    return -1;

}

void
remove_notification(filemanager_t* fm)
{
    entry_t* ent = getcurrentry(fm);
    column_t* col = getcurrentcolumn(fm);
    if (ent->notify == 1) col->total_notifications--;
    ent->notify = 0;
    return;
}

void
sortcols(filemanager_t* fm)
{
    int i, need_sort;
    int j;
    need_sort = 0;
    for (i = 0; i< fm->totalcols ; ++i)
        need_sort += update_col(fm, i);

    if (need_sort) {
        fm->lastupdatetimestamp = time(NULL);
        qsort(fm->cols, fm->totalcols, sizeof(column_t), cmp_cols);
        fm->curcol = 0;
        fm->cols[fm->curcol].cur=0;
        remove_notification(fm);
    }

    return;
}

static void
initfm (filemanager_t* fm, char *ipath[MAX_COLS], char *ifilter, int totalcols)
{
    fm->totalcols = totalcols;
    fm->lastupdatetimestamp = time(NULL);
    size_t i = 0;
    size_t j = 0;
    for (i=0;i<LINE_MAX;++i) {
        fm->fltr[i]=0;
    }
    for (i=0;i<fm->totalcols;++i) {
        for(j=0;j<PATH_MAX;++j) {
            fm->cols[i].path[j] = 0;
            fm->cols[i].oldpath[j]=0;
            fm->cols[i].newpath[j]=0;
        }
    }
    for (i=0;i<fm->totalcols; ++i) {
        strlcpy(fm->cols[i].path, ipath[i], sizeof(fm->cols[i].path));
        fm->cols[i].oldpath[0] = '\0';
    }
    strlcpy(fm->fltr, ifilter, sizeof(fm->fltr));
	fm->curcol = 0;
    for (i=0;i<fm->totalcols; ++i) {
        int r = populate(fm->cols[i].path, fm->cols[i].oldpath, fm->fltr, i);
        if (r == -1) {
            printwarn();
        }
    }

    for (i=0;i<fm->totalcols; ++i) {
        fm->cols[i].total_notifications=0;
    }

    return;
}

void
browse(char *ipath[MAX_COLS], char *ifilter, int totalcols)
{
    key_out_t nextkey;
    int numcode = 0;
	char *bin, *dir, *tmp, *run, *env;
	struct stat sb;
	regex_t re;
	int r, fd;
	size_t i = 0;
    filemanager_t* fm = get_filemanager();
    initfm(fm, ipath, ifilter, totalcols);
begin:
   	for (;;) {
    column_t* curcol = getcurrentcolumn(fm);
		redraw(fm);
nochange:
        nextkey = nextsel(&run, &env);
        switch (nextkey.enumc) {
		case SEL_QUIT:
		    for (i=0;i<totalcols;++i)
            { dentfree(fm->cols[i].dents); fm->cols[i].dents= NULL; }
			return;
		case SEL_BACK:
			/* There is no going back */
			if (strcmp(curcol->path, "/") == 0 ||
			    strcmp(curcol->path, ".") == 0 ||
			    strchr(curcol->path, '/') == NULL)
				goto nochange;
			dir = xdirname(curcol->path);
			if (canopendir(dir) == 0) {
				printwarn();
				goto nochange;
			}
			/* Save history */
			strlcpy(curcol->oldpath, curcol->path, sizeof(curcol->oldpath));
			strlcpy(curcol->path, dir, sizeof(curcol->path));
			/* Reset filter */
			strlcpy(fm->fltr, ifilter, sizeof(fm->fltr));
            populate_current(fm);
			goto begin;
		case SEL_GOIN:
			/* Cannot descend in empty directories */
			if (fm->cols[fm->curcol].ndents == 0)
				goto nochange;

			mkpath(curcol->path, getcurrentryname(fm), curcol->newpath, sizeof(curcol->newpath));
			DPRINTF_S(curcol->newpath);

			/* Get path info */
			fd = open(curcol->newpath, O_RDONLY | O_NONBLOCK);
			if (fd == -1) {
				printwarn();
				goto nochange;
			}
			r = fstat(fd, &sb);
			if (r == -1) {
				printwarn();
				close(fd);
				goto nochange;
			}
			close(fd);
			DPRINTF_U(sb.st_mode);

			switch (sb.st_mode & S_IFMT) {
			case S_IFDIR:
				if (canopendir(curcol->newpath) == 0) {
					printwarn();
					goto nochange;
				}
				strlcpy(curcol->path, curcol->path, sizeof(curcol->path));
				/* Reset filter */
				strlcpy(fm->fltr, ifilter, sizeof(fm->fltr));

            populate_current(fm);
				goto begin;
			case S_IFREG:
				bin = openwith(curcol->newpath);
				if (bin == NULL) {
					printmsg(user_strings[STR_ERROR_NO_ASSOC]);
					goto nochange;
				}
				exitcurses();
				spawn(bin, curcol->newpath, NULL);
				initcurses(fm->totalcols);
				continue;
			default:
				printmsg(user_strings[STR_ERR_UNSUPPORTED_FILE]);
				goto nochange;
			}
		case SEL_FLTR:
			/* Read filter */
			printprompt(user_strings[STR_PROMPT_REGEX]);
			tmp = readln();
			if (tmp == NULL)
				tmp = ifilter;
			/* Check and report regex errors */
			r = setfilter(&re, tmp);
			if (r != 0)
				goto nochange;
			freefilter(&re);
			strlcpy(fm->fltr, tmp, sizeof(fm->fltr));
			DPRINTF_S(fm->fltr);
			/* Save current */
			if (curcol->ndents > 0)
				mkpath(curcol->path, getcurrentryname(fm), curcol->oldpath, sizeof(curcol->oldpath));



            populate_current(fm);
			goto begin;
		case SEL_NEXT:
		    if (curcol->ndents!=0) {
                if (curcol->ndents - 1 == curcol->cur)
                {  curcol->cur= 0; remove_notification(fm); break; }

                if (curcol->cur < curcol->ndents - 1)
                    curcol->cur++;
                remove_notification(fm);
            }
			break;
		case SEL_PREV:
		    if (curcol->ndents!=0) {
                if (0==curcol->cur)
                     curcol->cur= curcol->ndents;

                if (curcol->cur > 0)
                    curcol->cur--;
                remove_notification(fm);
            }
			break;
		case SEL_PGDN:
			if (curcol->cur < curcol->ndents - 1)
				curcol->cur += MIN((LINES - 4) / 2, curcol->ndents - 1 - curcol->cur);
			break;
		case SEL_PGUP:
			if (curcol->cur > 0)
				curcol->cur -= MIN((LINES - 4) / 2, curcol->cur);
			break;
		case SEL_HOME:
			curcol->cur = 0;
			break;
		case SEL_END:
			curcol->cur = curcol->ndents - 1;
			break;
		case SEL_CD:
			/* Read target dir */
			printprompt(user_strings[STR_PROMPT_CD]);
			tmp = readln();
			if (tmp == NULL) {
				clearprompt();
				goto nochange;
			}
			mkpath(curcol->path, tmp, curcol->path, sizeof(curcol->newpath));
			if (canopendir(curcol->newpath) == 0) {
				printwarn();
				goto nochange;
			}
			strlcpy(curcol->path, curcol->newpath, sizeof(curcol->path));
			/* Reset filter */
			strlcpy(fm->fltr, ifilter, sizeof(fm->fltr))
			DPRINTF_S(curcol->path);

            populate_current(fm);
			goto begin;
        case SEL_FASTDIR:
            numcode = nextkey.keyp - '0';
			if ((numcode >=0) && (numcode <10)) {
				strlcpy(fm->fastdir,fast_dirs[numcode], sizeof(tmp));

				mkpath(curcol->path, fm->fastdir, curcol->newpath, sizeof(curcol->newpath));
				if (canopendir(curcol->newpath) == 0) {
					printwarn();
					goto nochange;
				}
				strlcpy(curcol->path, curcol->newpath, sizeof(curcol->path));
				/* Reset filter */
				strlcpy(fm->fltr, ifilter, sizeof(fm->fltr))
				DPRINTF_S(path);
				populate_current(fm);
			}
			goto begin;
            break;
		case SEL_CDHOME:
			tmp = getenv("HOME");
			if (tmp == NULL) {
				clearprompt();
				goto nochange;
			}
			if (canopendir(tmp) == 0) {
				printwarn();
				goto nochange;
			}
			strlcpy(curcol->path, tmp, sizeof(curcol->path));
			/* Reset filter */
			strlcpy(fm->fltr, ifilter, sizeof(fm->fltr));
			DPRINTF_S(curcol->path);

            populate_current(fm);
			goto begin;
		case SEL_TOGGLEDOT:
			showhidden ^= 1;
			initfilter(showhidden, &ifilter);
			strlcpy(fm->fltr, ifilter, sizeof(fm->fltr));

            populate_current(fm);

			goto begin;
		case SEL_MTIME:
			mtimeorder = !mtimeorder;
			/* Save current */
			if (curcol->ndents > 0)
				mkpath(curcol->path, getcurrentryname(fm), curcol->oldpath, sizeof(curcol->oldpath));

            populate_current(fm);
			goto begin;
		case SEL_REDRAW:
			/* Save current */
			if (curcol->ndents > 0)
				mkpath(curcol->path, getcurrentryname(fm), curcol->oldpath, sizeof(curcol->oldpath));

            populate_current(fm);
			goto begin;
		case SEL_RUN:
			run = xgetenv(env, run);
			exitcurses();
			spawn(run, NULL, curcol->path);
			initcurses(fm->totalcols);
			break;
		case SEL_RUNARG:
			run = xgetenv(env, run);
			exitcurses();
			spawn(run, getcurrentryname(fm), curcol->path);
			initcurses(fm->totalcols);
			break;
        case SEL_NEXTCOL:
            (fm->curcol < fm->totalcols - 1) ? (fm->curcol++) : (fm->curcol = 0);
            break;
        case SEL_PREVCOL:
            (fm->curcol == 0) ? (fm->curcol = fm->totalcols - 1): (fm->curcol--) ;
            break;
        case SEL_ADDCOL:
             addcol(fm);
            break;
        case SEL_REMOVECOL:
            removecol(fm);
            break;
        case SEL_SORTCOL:
            sortcols(fm);
            break;

		default:
		    break;
		}
		/* Screensaver */
		if (idletimeout != 0 && fm->idle == idletimeout) {
			fm->idle = 0;
			exitcurses();
			spawn(idlecmd, NULL, NULL);
			initcurses(fm->totalcols);
		}

		if ((autosorttimeout!=0) && (fm->idle % autosorttimeout == autosorttimeout/2)) {
            sortcols(fm);
        }
	}
}

static void
version()
{
    fprintf(stdout, "%s", user_strings[STR_VERSION]);
    exit(1);
}

static void
usage(char *argv0)
{
	fprintf(stderr, user_strings[STR_USAGE] , argv0, NCOLS-1);
	exit(1);
}

int
main(int argc, char *argv[])
{
	char cwd[PATH_MAX];
	char *ipath[MAX_COLS];
	char *ifilter;

  	/* Confirm we are in a terminal */
	if (!isatty(0) || !isatty(1)) {
		fprintf(stderr, "%s", user_strings[STR_ERR_NOTATTY]);
		exit(1);
	}

	if (getuid() == 0)
		showhidden = 1;
	initfilter(showhidden, &ifilter);

    int c = 0; int totalcols= NCOLS;
    while ((c = getopt (argc, argv, "c:e:uvh?")) != -1)
    switch (c) {
         case 'c':
             totalcols = atoi(optarg);
             if ((totalcols>MAX_COLS) || (totalcols<1)) {
                 fprintf(stderr, user_strings[STR_ERR_COLS_ARG_FAILED], MAX_COLS); exit(1);
             }
             break;
         case 'v':
            version();
            break;
         case 'e':
            break;
         case 'u':
         case 'h':
         case '?':
         default:
            usage(argv[0]);
            break;
    }
    argc -= optind;
    argv += optind;

	signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);

    size_t i = 0;
    for (i = 0; i < totalcols; ++i) {
        if (i<argc) {
            if (argv[i] != NULL)  {
                ipath[i] = argv[i];
            }
        }
        else {
            ipath[i] = getcwd(cwd, sizeof(cwd));
            if (ipath[i] == NULL)
                ipath[i] = "/";
        }
        if (canopendir(ipath[i]) == 0) {
            fprintf(stderr, "%s: %s\n", ipath[i], strerror(errno));
            exit(1);
        }
    }

	/* Set locale before curses setup */
	setlocale(LC_ALL, "");
	initcurses(totalcols);
	browse(ipath, ifilter, totalcols);
	exitcurses();
	exit(0);
}

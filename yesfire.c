/* See LICENSE file for copyright and license details. */
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <curses.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <locale.h>
#include <regex.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "util.h"

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

#ifndef PATH_MAX
#define PATH_MAX 256
#endif
#ifndef LINE_MAX
#define LINE_MAX 128
#endif

struct assoc {
	char *regex; /* Regex to match on filename */
	char *bin;   /* Program */
};

/* Supported actions */
enum action {
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
	SEL_SORTCOL
};

struct key {
	int sym;         /* Key pressed */
	enum action act; /* Action */
	char *run;       /* Program to run */
	char *env;       /* Environment variable to run */
};

struct key_out {
    int enumc;
    char keyp;
};

#include "config.h"

struct entry {
	char name[PATH_MAX];
	mode_t mode;
	time_t t;
	char dummy;
};

/* Global context */
struct entry *dents[MAX_COLS];
int ndents[MAX_COLS], cur[MAX_COLS];
short curcol, totalcols;
int idle;

/*
 * Layout:
 * .---------
 * | cwd: /mnt/path
 * |
 * |    file0
 * |    file1
 * |  > file2
 * |    file3
 * |    file4
 *      ...
 * |    filen
 * |
 * | Permission denied
 * '------
 */

void printmsg(char *);
void printwarn(void);
void printerr(int, char *);

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

	for (i = 0; i < LEN(assocs); i++) {
		if (regcomp(&regex, assocs[i].regex,
			    REG_NOSUB | REG_EXTENDED | REG_ICASE) != 0)
			continue;
		if (regexec(&regex, file, 0, NULL, 0) == 0) {
			bin = assocs[i].bin;
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
	const struct entry *a = va, *b = vb;

	if (mtimeorder)
		return b->t - a->t;
	return strcmp(a->name, b->name);
}

void
initcurses(void)
{
	char *term;

	if (initscr() == NULL) {
		term = getenv("TERM");
		if (term != NULL)
			fprintf(stderr, "error opening terminal: %s\n", term);
		else
			fprintf(stderr, "failed to initialize curses\n");
		exit(1);
	}
	if (COLS < 80) {fprintf(stderr, "minimum 80 cols terminal please, its 4 panel file viewer\n"); endwin(); exit(1);}
	if (COLS/totalcols < 20) {fprintf(stderr, "minimum 20 cols per panel please.\n"); endwin(); exit(1);}

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

	c = getch();
	if (c == -1)
		idle++;
	else
		idle = 0;
	return c;
}

/* Returns SEL_* if key is bound and 0 otherwise.
 * Also modifies the run and env pointers (used on SEL_{RUN,RUNARG}) */
struct key_out
nextsel(char **run, char **env)
{
	int c, i;
	struct key_out out;

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
    int nlines = MIN(LINES - 4, ndents[i]);

    if (cur[i] < nlines / 2) {
        delta = 0;

    } else if (cur[i] >= ndents[i] - nlines / 2) {
        delta = ndents[i] -nlines;

    }
    else {
        delta = cur[i] - nlines /2;
    }

    return delta;
}
void
getentline(struct entry ent[MAX_COLS], int index, int j)
{
    int i = 0;
    int delta = 0;
    for (i =0;i<totalcols;++i) {
        if (i==curcol) {
            if (j<ndents[i]){
                    ent[i] = dents[i][j+get_delta(i)];
            }
            else {
                ent[i].name[0]='\0';
                ent[i].mode=0;
                ent[i].t=0;
                ent[i].dummy=1;
            }
        }
        else {
            if (j<ndents[i]){
                delta = get_delta(i);
                ent[i] = dents[i][j+delta];
            }
            else {
                ent[i].name[0]='\0';
                ent[i].mode=0;
                ent[i].t=0;
                ent[i].dummy=1;
            }
        }
    }

    return;
}

void
printentline(struct entry ent[MAX_COLS], int ind)
{
	char name[PATH_MAX];
	unsigned int maxlen = COLS/totalcols - strlen(CURSR) - 1;
    int i = 0;
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
            if (cm == 0)
                sprintf(line , "%s%s%c ", (ind+get_delta(i)==cur[i] ) ? CURSR : EMPTY, name, ' ');
            else
                sprintf(line , "%s%s%c ", (ind+get_delta(i)==cur[i]) ? CURSR : EMPTY, name, cm);


            if (ind+get_delta(i)==cur[i] && i==curcol) attron(A_STANDOUT);

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
dentfill(char *path, struct entry **dents,
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
		n++;
	}

	/* Should never be null */
	r = closedir(dirp);
	if (r == -1)
		printerr(1, "closedir");
	return n;
}

void
dentfree(struct entry *dents)
{
	free(dents);
}

/* Return the position of the matching entry or 0 otherwise */
int
dentfind(struct entry *dents, int n, char *cwd, char *path)
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

	/* Can fail when permissions change while browsing */
	if (canopendir(path) == 0)
		return -1;

	/* Search filter */
	r = setfilter(&re, fltr);
	if (r != 0)
		return -1;

	dentfree(dents[coli]);

    cur[coli] = 0;
	ndents[coli] = 0;
	dents[coli] = NULL;

	ndents[coli] = dentfill(path, &dents[coli], visible, &re);

	freefilter(&re);
	if (ndents[coli] == 0)
		return 0; /* Empty result */

	qsort(dents[coli], ndents[coli], sizeof(*dents[coli]), entrycmp);

	/* Find cur from history */
	cur[coli] = dentfind(dents[coli], ndents[coli], path, oldpath);

	return 0;
}

void
redraw(char path[MAX_COLS][PATH_MAX])
{
	char cwd[PATH_MAX], cwdresolved[PATH_MAX];
	size_t ntcols;
	int nlines, odd;
	int i,j;

    int maxndents = 0;
    for (i=0;i<PATH_MAX;++i) {cwd[i]=0; cwdresolved[i]=0;}
    for (i=0;i<totalcols; ++i)
        if (ndents[i]>maxndents)
            maxndents =ndents[i];

	nlines = MIN(LINES - 4, maxndents);

	/* Clean screen */
	erase();

	/* Strip trailing slashes */
	for (j=0;j<totalcols;j++) {
	for (i = strlen(path[j]) - 1; i > 0; i--)
		if (path[j][i] == '/')
			path[j][i] = '\0';
		else
			break;
    }

	DPRINTF_D(cur[curcol]);
	DPRINTF_S(path[curcol]);

	/* No text wrapping in cwd line */
	ntcols = COLS;
    if (ntcols > PATH_MAX)
    	ntcols = PATH_MAX;
    strlcpy(cwd, path[curcol], ntcols);
    cwd[ntcols - strlen(CWD) - 1] = '\0';
    realpath(cwd, cwdresolved);

   	printw(CWD "%s\n\n", cwdresolved);
	/* Print listing */
	odd = ISODD(nlines);
	j=0;
    struct entry line[MAX_COLS];
	if (cur[curcol] < nlines / 2) {
		for (i = 0; i < nlines; i++) {
            getentline(line, i,i);
			printentline(line, i );
        }
	} else if (cur[curcol] >= ndents[curcol] - nlines / 2) {
		for (i = ndents[curcol] - nlines; i < ndents[curcol]; i++) {

            getentline(line, i, j);
			printentline(line,   j++);
        }
	} else {
	    j = 0;
		for (i = cur[curcol] - nlines / 2;
		     i < cur[curcol] + nlines / 2 + odd; i++) {
            getentline(line, i, j);
			printentline(line,  j++);
        }
	}

}

void
populate_current(char path[MAX_COLS][PATH_MAX], char oldpath[MAX_COLS][PATH_MAX], char fltr[LINE_MAX])
{
    int r = populate(path[curcol], oldpath[curcol], fltr, curcol);
    if (r == -1) {
        printwarn();
        return;
    }
}

void
addcol(char path[MAX_COLS][PATH_MAX], char oldpath[MAX_COLS][PATH_MAX], char fltr[LINE_MAX])
{
    int i;
    if (totalcols  < MAX_COLS)  {
        if (!S_ISDIR(dents[curcol][cur[curcol]].mode)) {
            for (i=0;i<PATH_MAX;++i) {
                path[totalcols][i] = path[curcol][i];
                oldpath[totalcols][i]=oldpath[curcol][i];
            }
        }
        else {
            mkpath(path[curcol], dents[curcol][cur[curcol]].name, path[totalcols] , sizeof(path[totalcols]));
        }

        int r = populate(path[totalcols], oldpath[totalcols], fltr, totalcols);
        if (r == -1) {
           printwarn();
           return;
         }
         curcol = totalcols;
         totalcols++;

    }
    return;
}

void
removecol(char path[MAX_COLS][PATH_MAX], char oldpath[MAX_COLS][PATH_MAX])
{
    int i,j;
    if (totalcols > 1) {
        if (curcol!=totalcols-1) {
            for (i=curcol;i<totalcols;++i) {
                dents[i] = dents[i+1];
                ndents[i] = ndents[i+1];
                cur[i] = cur[i+1];
                for (j=0;j<PATH_MAX;++j) {
                    path[i][j] = path[i+1][j];
                    oldpath[i][j]=oldpath[i+1][j];
                }
            }
        }
        else {
            curcol = totalcols - 2;
        }
        totalcols--;
    }
    return;
}

void sortcol()
{
    return;
}
void
browse(char *ipath[MAX_COLS], char *ifilter)
{
    struct key_out nextkey;
    int numcode = 0;
	char path[MAX_COLS][PATH_MAX], oldpath[MAX_COLS][PATH_MAX], newpath[MAX_COLS][PATH_MAX];
	char fltr[LINE_MAX];
	char fastdir[PATH_MAX];
	char *bin, *dir, *tmp, *run, *env;
	struct stat sb;
	regex_t re;
	int r, fd;

    size_t i = 0;
    size_t j = 0;

    for (i=0;i<LINE_MAX;++i) {
        fltr[i]=0;
    }
    for (i=0;i<totalcols;++i) {
        for(j=0;j<PATH_MAX;++j) {
            path[i][j] = 0;
            oldpath[i][j]=0;
            newpath[i][j]=0;
        }
    }
    for (i=0;i<totalcols; ++i) {
        strlcpy(path[i], ipath[i], sizeof(path[i]));
        oldpath[i][0] = '\0';
    }
    strlcpy(fltr, ifilter, sizeof(fltr));
	curcol = 0;
    for (i=0;i<totalcols; ++i) {
        r = populate(path[i], oldpath[i], fltr, i);
        if (r == -1) {
            printwarn();
            goto nochange;
        }
    }

begin:
   	for (;;) {
		redraw(path);
nochange:
        nextkey = nextsel(&run, &env);
        switch (nextkey.enumc) {
		case SEL_QUIT:
		    for (i=0;i<totalcols;++i)
                dentfree(dents[i]);
			return;
		case SEL_BACK:
			/* There is no going back */
			if (strcmp(path[curcol], "/") == 0 ||
			    strcmp(path[curcol], ".") == 0 ||
			    strchr(path[curcol], '/') == NULL)
				goto nochange;
			dir = xdirname(path[curcol]);
			if (canopendir(dir) == 0) {
				printwarn();
				goto nochange;
			}
			/* Save history */
			strlcpy(oldpath[curcol], path[curcol], sizeof(oldpath[curcol]));
			strlcpy(path[curcol], dir, sizeof(path[curcol]));
			/* Reset filter */
			strlcpy(fltr, ifilter, sizeof(fltr));
            populate_current(path,oldpath,fltr);
			goto begin;
		case SEL_GOIN:
			/* Cannot descend in empty directories */
			if (ndents[curcol] == 0)
				goto nochange;

			mkpath(path[curcol], dents[curcol][cur[curcol]].name, newpath[curcol], sizeof(newpath[curcol]));
			DPRINTF_S(newpath[curcol]);

			/* Get path info */
			fd = open(newpath[curcol], O_RDONLY | O_NONBLOCK);
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
				if (canopendir(newpath[curcol]) == 0) {
					printwarn();
					goto nochange;
				}
				strlcpy(path[curcol], newpath[curcol], sizeof(path[curcol]));
				/* Reset filter */
				strlcpy(fltr, ifilter, sizeof(fltr));

            populate_current(path,oldpath,fltr);
				goto begin;
			case S_IFREG:
				bin = openwith(newpath[curcol]);
				if (bin == NULL) {
					printmsg("No association");
					goto nochange;
				}
				exitcurses();
				spawn(bin, newpath[curcol], NULL);
				initcurses();
				continue;
			default:
				printmsg("Unsupported file");
				goto nochange;
			}
		case SEL_FLTR:
			/* Read filter */
			printprompt("filter(aka regex): ");
			tmp = readln();
			if (tmp == NULL)
				tmp = ifilter;
			/* Check and report regex errors */
			r = setfilter(&re, tmp);
			if (r != 0)
				goto nochange;
			freefilter(&re);
			strlcpy(fltr, tmp, sizeof(fltr));
			DPRINTF_S(fltr);
			/* Save current */
			if (ndents[curcol] > 0)
				mkpath(path[curcol], dents[curcol][cur[curcol]].name, oldpath[curcol], sizeof(oldpath[curcol]));



            populate_current(path,oldpath,fltr);
			goto begin;
		case SEL_NEXT:
	    	if (ndents[curcol] - 1 == cur[curcol])
            { cur[curcol] = 0; break;}

			if (cur[curcol] < ndents[curcol] - 1)
		    	cur[curcol]++;
			break;
		case SEL_PREV:
            if (0==cur[curcol])
			    cur[curcol] = ndents[curcol];

			if (cur[curcol] > 0)
				cur[curcol]--;
			break;
		case SEL_PGDN:
			if (cur[curcol] < ndents[curcol] - 1)
				cur[curcol] += MIN((LINES - 4) / 2, ndents[curcol] - 1 - cur[curcol]);
			break;
		case SEL_PGUP:
			if (cur[curcol] > 0)
				cur[curcol] -= MIN((LINES - 4) / 2, cur[curcol]);
			break;
		case SEL_HOME:
			cur[curcol] = 0;
			break;
		case SEL_END:
			cur[curcol] = ndents[curcol] - 1;
			break;
		case SEL_CD:
			/* Read target dir */
			printprompt("Change dir where?: ");
			tmp = readln();
			if (tmp == NULL) {
				clearprompt();
				goto nochange;
			}
			mkpath(path[curcol], tmp, newpath[curcol], sizeof(newpath[curcol]));
			if (canopendir(newpath[curcol]) == 0) {
				printwarn();
				goto nochange;
			}
			strlcpy(path[curcol], newpath[curcol], sizeof(path[curcol]));
			/* Reset filter */
			strlcpy(fltr, ifilter, sizeof(fltr))
			DPRINTF_S(path[curcol]);

            populate_current(path,oldpath,fltr);
			goto begin;
        case SEL_FASTDIR:
            numcode = nextkey.keyp - '0';
			if ((numcode >=0) && (numcode <10)) {
				strlcpy(fastdir,fast_dirs[numcode], sizeof(tmp));

				mkpath(path[curcol], fastdir, newpath[curcol], sizeof(newpath[curcol]));
				if (canopendir(newpath[curcol]) == 0) {
					printwarn();
					goto nochange;
				}
				strlcpy(path[curcol], newpath[curcol], sizeof(path[curcol]));
				/* Reset filter */
				strlcpy(fltr, ifilter, sizeof(fltr))
				DPRINTF_S(path[curcol]);
				populate_current(path,oldpath,fltr);
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
			strlcpy(path[curcol], tmp, sizeof(path[curcol]));
			/* Reset filter */
			strlcpy(fltr, ifilter, sizeof(fltr));
			DPRINTF_S(path[curcol]);

            populate_current(path,oldpath,fltr);
			goto begin;
		case SEL_TOGGLEDOT:
			showhidden ^= 1;
			initfilter(showhidden, &ifilter);
			strlcpy(fltr, ifilter, sizeof(fltr));

            populate_current(path,oldpath,fltr);

			goto begin;
		case SEL_MTIME:
			mtimeorder = !mtimeorder;
			/* Save current */
			if (ndents[curcol] > 0)
				mkpath(path[curcol], dents[curcol][cur[curcol]].name, oldpath[curcol], sizeof(oldpath[curcol]));

            populate_current(path,oldpath,fltr);
			goto begin;
		case SEL_REDRAW:
			/* Save current */
			if (ndents[curcol] > 0)
				mkpath(path[curcol], dents[curcol][cur[curcol]].name, oldpath[curcol], sizeof(oldpath[curcol]));

            populate_current(path,oldpath,fltr);
			goto begin;
		case SEL_RUN:
			run = xgetenv(env, run);
			exitcurses();
			spawn(run, NULL, path[curcol]);
			initcurses();
			break;
		case SEL_RUNARG:
			run = xgetenv(env, run);
			exitcurses();
			spawn(run, dents[curcol][cur[curcol]].name, path[curcol]);
			initcurses();
			break;
        case SEL_NEXTCOL:
            (curcol < totalcols - 1) ? (curcol++) : (curcol = 0);
            break;
        case SEL_PREVCOL:
            (curcol == 0) ? (curcol = totalcols - 1): (curcol--) ;
            break;
        case SEL_ADDCOL:
             addcol(path, oldpath, fltr);
            break;
        case SEL_REMOVECOL:
            removecol(path, oldpath);
            break;
        case SEL_SORTCOL:
            sortcol();
            break;

		default:
		    break;
		}
		/* Screensaver */
		if (idletimeout != 0 && idle == idletimeout) {
			idle = 0;
			exitcurses();
			spawn(idlecmd, NULL, NULL);
			initcurses();
		}
	}
}

void
version()
{
    fprintf(stdout, "Yesfire 42.0: Some men just want to watch the fil(r)es OPENED.\n");
    exit(1);
}

void
usage(char *argv0)
{
	fprintf(stderr, "usage: %s [-c num_cols] [dir 0] ... [dir %d]\n", argv0, NCOLS-1);
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
		fprintf(stderr, "stdin or stdout is not a tty\n");
		exit(1);
	}

	if (getuid() == 0)
		showhidden = 1;
	initfilter(showhidden, &ifilter);

    int c = 0; totalcols = NCOLS;
    while ((c = getopt (argc, argv, "c:uvh?")) != -1)
    switch (c) {
         case 'c':
             totalcols = atoi(optarg);
             if ((totalcols>MAX_COLS) || (totalcols<1)) {
                 fprintf(stderr, "wrong cols number, must be in [1..%d]\n", MAX_COLS); exit(1);
             }
             break;
         case 'v':
            version();
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
	initcurses();
	browse(ipath, ifilter);
	exitcurses();
	exit(0);
}

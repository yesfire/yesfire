/* See LICENSE file for copyright and license details. */
#include "yesfire.h"
#include "config.h"

static char* user_strings[] = {
    "error opening terminal: %s\n",
    "failed to initialize curses\n",
    "minimum 80 cols terminal please, its 4 panel file viewer\n",
    "minimum 20 cols per panel please.\n",
    "No association",
    "Unsupported file",
    "filter(aka regex): ",
    "Change dir where?: ",
    "Yesfire 42.0: Some men just want to watch the fil(r)es OPENED. Build Date: %s\n",
    "usage: %s [-e stack_mode_command] [-c num_cols] [dir 0] ... [dir %d]\n",
    "stdin or stdout is not a tty\n",
    "wrong cols number, must be in [1..%d]\n",
};

static char* used_env_vars[] = {
    "SHELL",
    "TERM",
    "HOME",
    "FIRE_DEPTH",
};

static filemanager_t*
get_filemanager(void)
{
    static filemanager_t fire;
    return &fire;
}

YF_WRAP_CALL yfw_pid_t_t
yfw_fork()
{
    return fork();
};
YF_WRAP_CALL char*
yfw_getenv(const char* name)
{
    return getenv(name);
};
YF_WRAP_CALL int
yfw_setenv(const char* name, const char* value, int overwrite)
{
    return setenv(name, value ,overwrite);
};
YF_WRAP_CALL yfw_uid_t_t
yfw_getuid(void)
{
    return getuid();
};
YF_WRAP_CALL int
yfw_open(const char *pathname, int flags)
{
    return open(pathname, flags);
};

YF_WRAP_CALL yfw_ssize_t_t
yfw_write(int fd, const void *buf, size_t count) {
    return yfw_write(fd, buf, count);
};

YF_WRAP_CALL int
yfw_close(int fd)
{
    return close(fd);
};

YF_WRAP_CALL int
yfw_lstat(const char *file_name, yfw_stat_t *buf)
{
    return lstat(file_name, buf);
};

YF_WRAP_CALL int
yfw_fstat(int fd, yfw_stat_t *buf)
{
    return fstat(fd, buf);
};

YF_WRAP_CALL int
yfw_chdir(const char* path)
{
    return chdir(path);
};
#define MAX_ARGS 10
YF_WRAP_CALL int
yfw_execlp(const char *file, const char *arg, ...)
{
    int i = 1;
    char* argv[MAX_ARGS];
    char* ap;

    va_list argl;
    va_start(argl, arg);
    for (;;) {
        ap = va_arg(argl, char*);
        argv[i++] = ap;
        if (ap == NULL) break;
    }
    va_end(argl);

    argv[0] = arg;
    return execvp(file, argv);
};
YF_WRAP_CALL yfw_pid_t_t
yfw_waitpid(yfw_pid_t_t pid, int *status, int options )
{
    return waitpid(pid, status, options);
};

YF_WRAP_CALL int
yfw_getopt(int argc, char * const argv[], const char *optstring)
{
    return getopt(argc, argv, optstring);
};
YF_WRAP_CALL char*
yfw_getcwd(char *buf, size_t size)
{
    return getcwd(buf, size);
};
YF_WRAP_CALL int
yfw_isatty(int fd)
{
    return isatty(fd);
};
YF_WRAP_CALL yfw_dir_t*
yfw_opendir(const char* name)
{
    return opendir(name);
};
YF_WRAP_CALL yfw_dirent_t *
yfw_readdir(yfw_dir_t* dirp)
{
    return readdir(dirp);
};
YF_WRAP_CALL int
yfw_closedir(yfw_dir_t *dirp)
{
    return closedir(dirp);
};

YF_WRAP_CALL int
yfw_regexec(const yfw_regex_t_t *preg, const char *string, size_t nmatch, yfw_regmatch_t_t pmatch[], int eflags)
{
    return regexec(preg, string, nmatch, pmatch, eflags);
};
YF_WRAP_CALL void
yfw_regfree(yfw_regex_t_t* preg)
{
    return regfree(preg);
};
YF_WRAP_CALL int
yfw_regcomp(yfw_regex_t_t *preg, const char *regex, int cflags)
{
    return regcomp(preg, regex, cflags);
};
YF_WRAP_CALL size_t
yfw_regerror(int errcode, const regex_t *preg, char *errbuf, size_t errbuf_size)
{

    return regerror(errcode, preg, errbuf, errbuf_size);
};

YF_WRAP_CALL yfw_window_t*
yfw_initscr(void)
{
    return initscr();
};
YF_WRAP_CALL int
yfw_cbreak(void)
{
    return cbreak();
};
YF_WRAP_CALL int
yfw_noecho(void)
{
    return noecho();
};
YF_WRAP_CALL int
yfw_echo(void)
{
    return echo();
};
YF_WRAP_CALL int
yfw_nonl(void)
{
    return nonl();
};
YF_WRAP_CALL int
yfw_wgetnstr(yfw_window_t *win, char *str, int n)
{
    return wgetnstr(win, str, n);
};
YF_WRAP_CALL int
yfw_intrflush(yfw_window_t* win, bool bf)
{
    return intrflush(win, bf);
};
YF_WRAP_CALL int
yfw_keypad(yfw_window_t* win, bool bf)
{
    return keypad(win, bf);
};
YF_WRAP_CALL int
yfw_curs_set(int visibility)
{
    return curs_set(visibility);
};
YF_WRAP_CALL void
yfw_timeout(int delay)
{
    return timeout(delay);
};
YF_WRAP_CALL int
yfw_move(int y, int x)
{
    return move(y, x);
};
YF_WRAP_CALL int
yfw_printw(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    filemanager_t* fm = get_filemanager();
    int out = vw_printw(fm->window, fmt, args);
    va_end(args);

    return out;
};
YF_WRAP_CALL int
yfw_endwin(void)
{
    return endwin();
};
YF_WRAP_CALL int
yfw_erase(void)
{
    return erase();
};

YF_WRAP_CALL int
yfw_getch(void)
{
    return getch();
};
YF_WRAP_CALL int
yfw_attron(int attrs)
{
    return attron(attrs);
};

YF_WRAP_CALL int
yfw_attroff(int attrs)
{
    return attroff(attrs);
};


YF_WRAP_CALL char*
yfw_dirname(char* path)
{
    return dirname(path);
};


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
gettotalcols(void)
{
    filemanager_t* fm = get_filemanager();
    return fm->totalcols;
}

void
printmsg(char *msg)
{
	yfw_move(YFW_LINES - 1, 0);
	yfw_printw("%s\n", msg);
}

void
printwarn(void)
{
	printmsg(strerror(errno));
}


void
exitcurses(void)
{
	yfw_endwin();
}

void
printerr(int ret, char *prefix)
{
	exitcurses();
	fprintf(stderr, "%s: %s\n", prefix, strerror(errno));
	exit(ret);
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
		yfw_write(fd, buf, r);
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
	p = yfw_dirname(tmp);
	if (p == NULL)
		printerr(1, "dirname");
	strlcpy(out, p, sizeof(out));
	return out;
}

static int
spawn(const char *file, const char *arg, const char *dir)
{
    if (file == NULL) return 0;
	yfw_pid_t_t pid;
	int status;
	pid = yfw_fork();
	if (pid == 0) {
		if (dir != NULL) {
			if (-1 == yfw_chdir(dir)) {
			    printwarn();
            }
        }
		yfw_execlp(file, file, arg, NULL);
		_exit(1);


	} else {
		while (yfw_waitpid(pid, &status, 0) == -1)
			DPRINTF_D(status);
		DPRINTF_D(pid);
	}
	return status;
}

char *
xgetenv(char *name, char *fallback)
{
	char *value;

	if (name == NULL)
		return fallback;
	value = yfw_getenv(name);
	return value && value[0] ? value : fallback;
}

char *
openwith(char *file)
{
	yfw_regex_t_t regex;
	char *bin = NULL;
	int i;

	for (i = 0; i < LEN(assocs_view); i++) {
		if (yfw_regcomp(&regex, assocs_view[i].regex,
			    YFW_REG_NOSUB | YFW_REG_EXTENDED | YFW_REG_ICASE) != 0)
			continue;
		if (yfw_regexec(&regex, file, 0, NULL, 0) == 0) {
			bin =  assocs_view[i].bin[0];
			yfw_regfree(&regex);
			break;
		}
		yfw_regfree(&regex);
	}
	DPRINTF_S(bin);
	return bin;
}

int
setfilter(yfw_regex_t_t *regex, char *filter)
{
	char errbuf[LINE_MAX];
	size_t len;
	int r;

	r = yfw_regcomp(regex, filter, YFW_REG_NOSUB | YFW_REG_EXTENDED | YFW_REG_ICASE);
	if (r != 0) {
		len = YFW_COLS;
		if (len > sizeof(errbuf))
			len = sizeof(errbuf);
		yfw_regerror(r, regex, errbuf, len);
		printmsg(errbuf);
	}
	return r;
}

void
freefilter(yfw_regex_t_t *regex)
{
	yfw_regfree(regex);
	return;
}

void
initfilter(int dot, char **ifilter)
{
	*ifilter = dot ? "." : "^[^.]";
	return;
}

int
visible(yfw_regex_t_t *regex, char *file)
{
	return yfw_regexec(regex, file, 0, NULL, 0) == 0;
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
    yfw_window_t* window = yfw_initscr();
	if (window == NULL) {
		term = yfw_getenv(used_env_vars[ENV_TERM]);
		if (term != NULL)
			fprintf(stderr,  user_strings[STR_ERR_TERMINAL_OPEN], term);
		else
			fprintf(stderr, "%s", user_strings[STR_ERR_CURSES_INIT_FAILED]);
		exit(1);
	}
	if (YFW_COLS < 80) {fprintf(stderr,"%s", user_strings[STR_ERR_RESIZE_FAILED]); endwin(); exit(1);}
	if (YFW_COLS/totalcols < 20) {fprintf(stderr, "%s", user_strings[STR_ERR_RESIZE_FAILED2]); endwin(); exit(1);}

    filemanager_t* fm = get_filemanager();
    fm->window = window;
	yfw_cbreak();
	yfw_noecho();
	yfw_nonl();
	yfw_intrflush(stdscr, FALSE);
	yfw_keypad(stdscr, TRUE);
	yfw_curs_set(FALSE);
	yfw_timeout(1000);
	return;
}

void
clearprompt(void)
{
	printmsg("");
	return;
}


void
printprompt(char *str)
{
	clearprompt();
	yfw_printw(str);
	return;
}

int xgetch(void)
{
	int c;
    filemanager_t* fm = get_filemanager();
	c = yfw_getch();
	if (c == -1)
		fm->idle++;
	else
		fm->idle = 0;
	return c;
}

yf_key_t
nextact(global_ui_state_t mode)
{
	int c, i;
	yf_key_t out;

	c = xgetch();
	if (c == 033)
		c = META(xgetch());
    out.nm = 0;
    out.sm = 0;
    out.bb = 0;
    if (mode == NORMAL_MODE) {
        for (i = 0; i < LEN(bindings); i++)
            if (c == bindings[i].sym) {
                out.nm = bindings[i].nm;
            }
        for (i = 0; i < LEN(binbindings); i++)
            if (c == binbindings[i].key) {
                out.bb = i;
                out.nm = SEL_BINBINDING;
            }
    }

    if (mode == STACK_MODE) {
        for (i = 0; i < LEN(smbindings); i++)
            if (c == smbindings[i].sym) {
                out.sm = smbindings[i].sm;
            }
    }

	out.sym = c;
	return out;
}

char *
readln(void)
{
	static char ln[LINE_MAX];

	yfw_timeout(-1);
	yfw_echo();
	yfw_curs_set(TRUE);
	memset(ln, 0, sizeof(ln));
	yfw_wgetnstr(stdscr, ln, sizeof(ln) - 1);
	yfw_noecho();
	yfw_curs_set(FALSE);
	yfw_timeout(1000);
	return ln[0] ? ln : NULL;
}

int
canopendir(char *path)
{
	yfw_dir_t *dirp;

	dirp = yfw_opendir(path);
	if (dirp == NULL)
		return 0;
	yfw_closedir(dirp);
	return 1;
}

char *
mkpath(char *dir, char *name, char *out, size_t n)
{
	if (name[0] == '/') {
		strlcpy(out, name, n);
	} else {
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
    int nlines = MIN(YFW_LINES - 4, ndents);

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
	unsigned int maxlen = YFW_COLS/totalcols - strlen(CURSR) - 1;
    int i = 0;
    filemanager_t* fm = get_filemanager();
    for (i=0;i<PATH_MAX;++i) name[i]=0;
    int j = 0;
    for (i =0;i<totalcols;++i) {
        char cm = 0;

        if (!ent[i].dummy) {
        strlcpy(name, ent[i].name, sizeof(name));
            if (YFW_S_ISDIR(ent[i].mode)) {
                cm = '/';
            } else if (YFW_S_ISLNK(ent[i].mode)) {
                cm = '@';
            } else if (YFW_S_ISSOCK(ent[i].mode)) {
                cm = '=';
            } else if (YFW_S_ISFIFO(ent[i].mode)) {
                cm = '|';
            } else if (ent[i].mode & YFW_S_IXUSR) {
                cm = '*';
            }
        }

        if (strlen(name) > maxlen)
            name[maxlen] = '\0';

        char line[2*maxlen]; for (j=0;j<2*maxlen;++j) line[j]=0;
        char lformat[10]; for (j=0;j<10;++j) lformat[j]=0;
        char align = 0; (i % 2 == 1) ? (align = '+') : (align = '-');

        sprintf(lformat, "%c%c%ds", '%', align, YFW_COLS/totalcols - 2);

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


            if (ind+get_delta(i)==fm->cols[i].cur && i==fm->curcol) yfw_attron(A_STANDOUT);

            if ((strlen(line) >=maxlen) && (cm!=0) && (line[maxlen-2]) !=cm)
                line[maxlen-1] = cm;
            line[maxlen]='\0';
            yfw_printw(lformat, line);
        }
        else if (ent[i].dummy==1){
            yfw_printw(lformat, " ");
        }
        yfw_attroff(A_STANDOUT);
        if (i==totalcols - 1) printw ("\n");
    }
}

int
dentfill(char *path, entry_t **dents,
	 int (*filter)(yfw_regex_t_t *, char *), yfw_regex_t_t *re)
{
	char newpath[PATH_MAX];
	size_t i = 0; for (i=0;i<PATH_MAX;++i) newpath[i]=0;
	yfw_dir_t *dirp;
	yfw_dirent_t *dp;
	yfw_stat_t sb;
	int r, n = 0;

	dirp = yfw_opendir(path);
	if (dirp == NULL)
		return 0;

	while ((dp = yfw_readdir(dirp)) != NULL) {
		if (strcmp(dp->d_name, ".") == 0 ||
		    strcmp(dp->d_name, "..") == 0)
			continue;
		if (filter(re, dp->d_name) == 0)
			continue;
		*dents = xrealloc(*dents, (n + 1) * sizeof(**dents));
		strlcpy((*dents)[n].name, dp->d_name, sizeof((*dents)[n].name));

		mkpath(path, dp->d_name, newpath, sizeof(newpath));
		r = yfw_lstat(newpath, &sb);
		if (r == -1)
			printerr(1, "lstat");
		(*dents)[n].mode = sb.st_mode;
		(*dents)[n].t = sb.st_mtime;
        (*dents)[n].dummy  = 0;
        (*dents)[n].notify  = 0;
		n++;
	}

	r = yfw_closedir(dirp);
	if (r == -1)
		printerr(1, "closedir");
	return n;
}

void
dentfree(entry_t* dents)
{
	free(dents);
}

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
	yfw_regex_t_t re;
	int r;

    filemanager_t* fm = get_filemanager();

	if (canopendir(path) == 0)
		return -1;

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
		return 0;

	qsort(fm->cols[coli].dents, fm->cols[coli].ndents, sizeof(*(fm->cols[coli].dents)), entrycmp);


	fm->cols[coli].cur = dentfind(fm->cols[coli].dents, fm->cols[coli].ndents, path, oldpath);

	return 0;
}

void
redraw(filemanager_t* fm)
{
     if (YFW_LINES < 5) {yfw_erase(); return;}
     if ((YFW_COLS/(fm->totalcols)<15) && (fm->totalcols!=0)) {yfw_erase(); return;}

	char cwd[PATH_MAX], cwdresolved[PATH_MAX];
	size_t ntcols;
	int nlines, odd;
	int i,j;

    int maxndents = 0;
    for (i=0;i<PATH_MAX;++i) {cwd[i]=0; cwdresolved[i]=0;}
    for (i=0;i<fm->totalcols; ++i)
        if (fm->cols[i].ndents>maxndents)
            maxndents =fm->cols[i].ndents;

	nlines = MIN(YFW_LINES - 4, maxndents);


	yfw_erase();


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


	ntcols = YFW_COLS;
    if (ntcols > PATH_MAX)
    	ntcols = PATH_MAX;
    strlcpy(cwd, cur_path, ntcols);
    cwd[ntcols - strlen(CWD) - 1] = '\0';
    realpath(cwd, cwdresolved);

   	yfw_printw(CWD "%s\n\n", cwdresolved);

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

int
fm_addcol(filemanager_t* fm)
{
    column_t* curcol = getcurrentcolumn(fm);
    column_t* cols = fm->cols;
    if (YFW_COLS/(fm->totalcols+1) < 20) {  return 0; }
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
                return 0;
            }
            fm->curcol = fm->totalcols;
            fm->totalcols++;
            return 0;
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
            return 0;
        }
        fm->curcol = fm->totalcols;
        fm->totalcols++;

        }
        return 1;
}

int
fm_removecol(filemanager_t* fm)
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
    return 1;
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

int
fm_sortcols(filemanager_t* fm)
{
    int i;
    int need_sort = 0;
    for (i = 0; i< fm->totalcols; ++i)
        need_sort += update_col(fm, i);

    if (need_sort) {
        fm->lastupdatetimestamp = time(NULL);
        qsort(fm->cols, fm->totalcols, sizeof(column_t), cmp_cols);
        fm->curcol = 0;
        fm->cols[fm->curcol].cur=0;
        remove_notification(fm);
        return 1;
    }

    return 1;
}

static void
initfm (filemanager_t* fm, char *ipath[MAX_COLS], char *ifilter, int totalcols)
{
    fm->mode = NORMAL_MODE;
    fm->totalcols = totalcols;
    fm->lastupdatetimestamp = time(NULL);
    fm->ifilter = ifilter;
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


int
diediediemydarling(filemanager_t* fm, int curses_too)
{
    int i;
	for (i=0;i<fm->totalcols;++i) {
	    dentfree(fm->cols[i].dents); fm->cols[i].dents= NULL;
	}

	if (curses_too) exitcurses();

	exit(0);

    return 0;
}

int
fm_prev_dir(filemanager_t*fm, column_t* curcol)
{
    char* ifilter = fm->ifilter;

    if (strcmp(curcol->path, "/") == 0 ||
        strcmp(curcol->path, ".") == 0 ||
        strchr(curcol->path, '/') == NULL)
        return 0;
    char* dir = xdirname(curcol->path);
    if (canopendir(dir) == 0) {
        printwarn();
        return 0;
    }

    strlcpy(curcol->oldpath, curcol->path, sizeof(curcol->oldpath));
    strlcpy(curcol->path, dir, sizeof(curcol->path));

    strlcpy(fm->fltr, ifilter, sizeof(fm->fltr));
    populate_current(fm);
    return 1;
}

int
fm_next_dir(filemanager_t* fm, column_t* curcol)
{

    if (fm->cols[fm->curcol].ndents == 0)
        return 0;

    mkpath(curcol->path, getcurrentryname(fm), curcol->newpath, sizeof(curcol->newpath));
    DPRINTF_S(curcol->newpath);

    int r, fd;
    yfw_stat_t sb;
    char* bin, *env;

    fd = yfw_open(curcol->newpath, YFW_O_RDONLY | YFW_O_NONBLOCK);
    if (fd == -1) {
        printwarn();
        return 0;
    }
    r = yfw_fstat(fd, &sb);
    if (r == -1) {
        printwarn();
        yfw_close(fd);
        return 0;
    }
    yfw_close(fd);
    DPRINTF_U(sb.st_mode);

    int fl = 0;
    switch (sb.st_mode & YFW_S_IFMT) {
    case YFW_S_IFDIR:

        if (canopendir(curcol->newpath) == 0) {
            printwarn();
            break;
        }
        strlcpy(curcol->path, curcol->newpath, sizeof(curcol->path));

        strlcpy(fm->fltr, fm->ifilter, sizeof(fm->fltr));

        populate_current(fm);
        fl = 1; break;
    case YFW_S_IFREG:
        bin = openwith(curcol->newpath);
        if (bin == NULL) {
            printmsg(user_strings[STR_ERROR_NO_ASSOC]);
            break;
        }
        exitcurses();
        spawn(bin, curcol->newpath, NULL);
        initcurses(fm->totalcols);
        fl = 1;
        break;
    default:
        printmsg(user_strings[STR_ERR_UNSUPPORTED_FILE]);
        break;
    }

    return fl;
}

int
fm_fltr_nonblock(filemanager_t* fm, column_t* curcol)
{

    printprompt(user_strings[STR_PROMPT_REGEX]);
    char* ifilter = fm->ifilter;
    char* tmp = readln();
    if (tmp == NULL)
        tmp = ifilter;

    yfw_regex_t_t re;
    int r = setfilter(&re, tmp);
    if (r != 0)
        return 0;
    freefilter(&re);
    strlcpy(fm->fltr, tmp, sizeof(fm->fltr));
    DPRINTF_S(fm->fltr);

    if (curcol->ndents > 0)
        mkpath(curcol->path, getcurrentryname(fm), curcol->oldpath, sizeof(curcol->oldpath));



    populate_current(fm);
    return 1;

}


int
fm_next_entry(filemanager_t* fm, column_t* curcol)
{
    if (curcol->ndents!=0) {
        if (curcol->ndents - 1 == curcol->cur)
        {  curcol->cur= 0; remove_notification(fm);  return 1; }

        if (curcol->cur < curcol->ndents - 1)
            curcol->cur++;
        remove_notification(fm);
    }
    return 1;
}

int
fm_prev_entry(filemanager_t* fm, column_t* curcol)
{
    if (curcol->ndents!=0) {
        if (0==curcol->cur)
                curcol->cur= curcol->ndents;

        if (curcol->cur > 0)
            curcol->cur--;
        remove_notification(fm);
    }
    return 1;
}

int
fm_pageup(column_t* curcol)
{
    if (curcol->cur > 0)
        curcol->cur -= MIN((YFW_LINES - 4) / 2, curcol->cur);
    return 1;
}

int
fm_pagedown(column_t* curcol)
{
    if (curcol->cur < curcol->ndents - 1)
        curcol->cur += MIN((YFW_LINES - 4) / 2, curcol->ndents - 1 - curcol->cur);
    return 1;
}

int
fm_home(column_t* curcol)
{
    curcol->cur = 0;
    return 1;
}

int
fm_end(column_t* curcol)
{
    curcol->cur = curcol->ndents - 1;
    return 1;
}

int
fm_cd(filemanager_t* fm, column_t* curcol)
{

    printprompt(user_strings[STR_PROMPT_CD]);
    char* tmp = readln();
    if (tmp == NULL) {
        clearprompt();
        return 0;
    }
    mkpath(curcol->path, tmp, curcol->path, sizeof(curcol->newpath));
    if (canopendir(curcol->newpath) == 0) {
        printwarn();
        return 0;
    }
    strlcpy(curcol->path, curcol->newpath, sizeof(curcol->path));

    strlcpy(fm->fltr, fm->ifilter, sizeof(fm->fltr))
    DPRINTF_S(curcol->path);

    populate_current(fm);
    return 1;
}

int
fm_fastdir(filemanager_t* fm, column_t* curcol, char key)
{
    char numcode = key - '0';
    if ((numcode >=0) && (numcode <10)) {
        strlcpy(fm->fastdir,fast_dirs[numcode], sizeof(fast_dirs[numcode]));

        mkpath(curcol->path, fm->fastdir, curcol->newpath, sizeof(curcol->newpath));
        if (canopendir(curcol->newpath) == 0) {
            printwarn();
            return 0;
        }
        strlcpy(curcol->path, curcol->newpath, sizeof(curcol->path));

        strlcpy(fm->fltr, fm->ifilter, sizeof(fm->fltr))
        DPRINTF_S(path);
        populate_current(fm);
    }
    return 1;
}

int
fm_cdhome(filemanager_t* fm, column_t* curcol)
{
    char* tmp = yfw_getenv(used_env_vars[ENV_HOME]);
    if (tmp == NULL) {
        clearprompt();
        return 0;
    }
    if (canopendir(tmp) == 0) {
        printwarn();
        return 0;
    }
    strlcpy(curcol->path, tmp, sizeof(curcol->path));

    strlcpy(fm->fltr, fm->ifilter, sizeof(fm->fltr));
    DPRINTF_S(curcol->path);

    populate_current(fm);
    return 1;
}


int
fm_togglehidden(filemanager_t* fm)
{
    showhidden ^= 1;
    initfilter(showhidden, &fm->ifilter);
    strlcpy(fm->fltr, fm->ifilter, sizeof(fm->fltr));

    populate_current(fm);
    return 1;
}

int
fm_togglemtime(filemanager_t* fm, column_t* curcol)
{
    mtimeorder = !mtimeorder;

    if (curcol->ndents > 0)
        mkpath(curcol->path, getcurrentryname(fm), curcol->oldpath, sizeof(curcol->oldpath));

    populate_current(fm);
    return 1;

}

int
fm_forceredraw(filemanager_t* fm, column_t* curcol)
{

    if (curcol->ndents > 0)
        mkpath(curcol->path, getcurrentryname(fm), curcol->oldpath, sizeof(curcol->oldpath));

    populate_current(fm);
    return 1;
}


int
fm_run(filemanager_t* fm, column_t* curcol)
{
    return 1;
}


int
fm_view(filemanager_t* fm)
{
    return 0;
}

int
fm_edit(filemanager_t* fm)
{
    return 0;
}

int
fm_shell(filemanager_t* fm, column_t* curcol)
{
    exitcurses();
    spawn(xgetenv(used_env_vars[ENV_SHELL], favourite_shell), NULL, curcol->path);
    initcurses(fm->totalcols);
    return 1;
}

int
fm_binbinding(filemanager_t* fm, column_t* curcol, int i)
{
   	exitcurses();
   	spawn(binbindings[i].bin, NULL, curcol->path);
   	initcurses(fm->totalcols);
    return 1;
}

int
fm_next_col(filemanager_t* fm)
{
    (fm->curcol < fm->totalcols - 1) ? (fm->curcol++) : (fm->curcol = 0);
    return 1;
}

int
fm_prev_col(filemanager_t* fm)
{
    (fm->curcol == 0) ? (fm->curcol = fm->totalcols - 1): (fm->curcol--) ;
    return 1;
}

int
fm_tostackmode(filemanager_t* fm)
{
    return 1;
}

update_state_t
normal_mode_handler(filemanager_t* fm, column_t* curcol, yf_key_t key)
{
    int s = 0;
    action_t what = key.nm;
    switch (what) {
        case SEL_QUIT:
            s = diediediemydarling(fm, 1);
            break;
        case SEL_BACK:
            s = fm_prev_dir(fm, curcol);
            break;
        case SEL_GOIN:
            s = fm_next_dir(fm, curcol);
            break;
        case SEL_FLTR:
            s = fm_fltr_nonblock(fm, curcol);
            break;
        case SEL_NEXT:
            s = fm_next_entry(fm, curcol);
            break;
        case SEL_PREV:
            s = fm_prev_entry(fm, curcol);
            break;
        case SEL_PGDN:
            s = fm_pagedown(curcol);
            break;
        case SEL_PGUP:
            s = fm_pageup(curcol);
            break;
        case SEL_HOME:
            s = fm_home(curcol);
            break;
        case SEL_END:
            s = fm_end(curcol);
            break;
        case SEL_CD:
            s = fm_cd(fm, curcol);
            break;
        case SEL_FASTDIR:
            s = fm_fastdir(fm,curcol, key.sym);
            break;
        case SEL_CDHOME:
            s = fm_cdhome(fm, curcol);
            break;
        case SEL_TOGGLEDOT:
            s = fm_togglehidden(fm);
            break;
        case SEL_MTIME:
            s = fm_togglemtime(fm, curcol);
            break;
        case SEL_REDRAW:
            s = fm_forceredraw(fm, curcol);
            break;
        case SEL_RUN:
            s = fm_run(fm,curcol);
            break;
        case SEL_VIEW:
            s = fm_view(fm);
            break;
        case SEL_EDIT:
            s = fm_edit(fm);
            break;
        case SEL_SHELL:
            s = fm_shell(fm, curcol);
            break;
        case SEL_NEXTCOL:
            s = fm_next_col(fm);
            break;
        case SEL_PREVCOL:
            s = fm_prev_col(fm);
            break;
        case SEL_ADDCOL:
            s = fm_addcol(fm);
            break;
        case SEL_REMOVECOL:
            s = fm_removecol(fm);
            break;
        case SEL_SORTCOL:
            s = fm_sortcols(fm);
            break;
        case SEL_STACKMODE:
            s = fm_tostackmode(fm);
            break;
        case SEL_BINBINDING:
            s = fm_binbinding(fm, curcol, key.bb);
            break;

        default:
            s = 0;
            break;
        }
        if ((autosorttimeout!=0) && (fm->idle % autosorttimeout == autosorttimeout/2)) {
            fm_sortcols(fm);
        }
        return (update_state_t) s;
}

update_state_t
stack_mode_handler(filemanager_t* fm, yf_key_t key)
{
    stack_mode_action_t what = key.sm;
    switch (what) {
        case STACK_DELFILE:
            break;
        case STACK_DELDIR:
            break;
        case STACK_RENAMEFILE:
            break;
        case STACK_RENAMEDIR:
            break;
        case STACK_MOVEFILE:
            break;
        case STACK_MOVEDIR:
            break;
        case STACK_MAKEDIR:
            break;
        case STACK_TOUCHFILE:
            break;
        case STACK_COPYFILE:
            break;
        case STACK_COPYDIR:
            break;
        case STACK_PUSHCOLCWD:
            break;
        case STACK_PUSHCOLCURDIR:
            break;
        case STACK_PUSHCOLCURFILE:
            break;
        case STACK_SWAPFILE:
            break;
        case STACK_SWAPDIR:
            break;
        case STACK_DROPDIR:
            break;
        case STACK_DROPFILE:
            break;
        case STACK_DUPDIR:
            break;
        case STACK_DUPFILE:
            break;
        case STACK_OVERDIR:
            break;
        case STACK_OVERFILE:
            break;
        case STACK_ROTDIR:
            break;
        case STACK_ROTFILE:
            break;
        case STACK_PICKDIR:
            break;
        case STACK_PICKFILE:
            break;
        case STACK_SHOWCWDSEL:
            break;
        case STACK_NORMALMODE:
            break;
        default:
            break;
    }
    return NOTHING_CHANGES;
}

int
detect_resize(void)
{
    static int x = 0;
    static int y = 0;
    if ((YFW_COLS != x) || (YFW_LINES !=y)) {
        x = YFW_COLS;
        y = YFW_LINES;
        return 1;
    }
    return 0;
}

#define ETERNITY for(;;)
void
browse(char *ipath[MAX_COLS], char *ifilter, int totalcols)
{
    filemanager_t* fm = get_filemanager();
    initfm(fm, ipath, ifilter, totalcols);
    int update_flag = NEEDS_REDRAW;
   	ETERNITY {
        column_t* curcol = getcurrentcolumn(fm);
        global_ui_state_t mode = fm->mode;
		if (update_flag == NEEDS_REDRAW) redraw(fm);

        yf_key_t key = nextact(mode);
        switch (mode) {
            case NORMAL_MODE:
                update_flag = normal_mode_handler(fm, curcol, key);
                break;
            case STACK_MODE:
                update_flag = stack_mode_handler(fm, key);
                break;
            default:
                update_flag = NOTHING_CHANGES;
                break;
        }
        if (update_flag == NOTHING_CHANGES)
            update_flag = (update_state_t) detect_resize();

		if ((idletimeout != 0) && (fm->idle == idletimeout)) {
			fm->idle = 0;
			exitcurses();
			spawn(idlecmd, NULL, NULL);
			initcurses(fm->totalcols);
		}

	}
}

static void
version(void)
{
    fprintf(stdout, user_strings[STR_VERSION], __DATE__);
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


	if (!yfw_isatty(0) || !yfw_isatty(1)) {
		fprintf(stderr, "%s", user_strings[STR_ERR_NOTATTY]);
		exit(1);
	}

	if (yfw_getuid() == 0)
		showhidden = 1;
	initfilter(showhidden, &ifilter);

    int c = 0; int totalcols= NCOLS;
    while ((c = yfw_getopt (argc, argv, "c:e:uvh?")) != -1)
    switch (c) {
         case 'c':
             totalcols = atoi(YFW_OPTARG);
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
    argc -= YFW_OPTIND;
    argv += YFW_OPTIND;

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
            ipath[i] = yfw_getcwd(cwd, sizeof(cwd));
            if (ipath[i] == NULL)
                ipath[i] = "/";
        }
        if (canopendir(ipath[i]) == 0) {
            fprintf(stderr, "%s: %s\n", ipath[i], strerror(errno));
            exit(1);
        }
    }


	setlocale(LC_ALL, "");
	initcurses(totalcols);
	browse(ipath, ifilter, totalcols);
	exitcurses();
	exit(0);
}

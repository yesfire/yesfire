/* See LICENSE file for copyright and license details. */
#ifndef YESFIRE_H
#define YESFIRE_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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
/* #define MIN(x, y) ((x) < (y) ? (x) : (y))
Fun fact. It's possible to avoid branching at all in this newfaggot macro.
Also A +- B ~ MAX(A,B), if A >> B i.e. 100000 +- 1 ~ 100000
It's even possible to build 1 instruction subleq RISC processor with this lol. */
#define BPL (sizeof(int)*8-1)
#define MIN(x,y) (((x) & (((int)((x)-(y)))>>BPL)) + ((y) & ~(((int)((x)-(y)))>>BPL)))
/* See: https://jjj.de/fxt/fxtbook.pdf */

#define ISODD(x) ((x) & 1)
#define SWAP(x,y) do \
   { unsigned char swap_temp[sizeof(x) == sizeof(y) ? (signed)sizeof(x) : -1]; \
     memcpy(swap_temp,&y,sizeof(x)); \
     memcpy(&y,&x,       sizeof(x)); \
     memcpy(&x,swap_temp,sizeof(x)); \
    } while(0)

#define ANSWER 42

#define CONTROL(c) ((c) ^ 0x40)
#define META(c) ((c) ^ 0x80)

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif
#ifndef LINE_MAX
#define LINE_MAX 256
#endif

#define MAX_RECURSION_DEPTH 128

#define YF_WRAP_CALL
#define YF_WRAP_ID(x) yfw_##x##_t

typedef regex_t YF_WRAP_ID(regex_t);
typedef regmatch_t YF_WRAP_ID(regmatch_t);
typedef pid_t YF_WRAP_ID(pid_t);
typedef uid_t YF_WRAP_ID(uid_t);
typedef mode_t YF_WRAP_ID(mode_t);
typedef ssize_t YF_WRAP_ID(ssize_t);

#define yfw_stat_t struct stat
#define yfw_dirent_t struct dirent
#define yfw_dir_t struct DIR
#define yfw_window_t struct WINDOW

#define YFW_REG_NOSUB REG_NOSUB
#define YFW_REG_EXTENDED REG_EXTENDED
#define YFW_REG_ICASE REG_ICASE

#define YFW_S_ISDIR S_ISDIR
#define YFW_S_ISSOCK S_ISSOCK
#define YFW_S_ISFIFO S_ISFIFO
#define YFW_S_ISLNK S_ISLNK

#define YFW_S_IXUSR S_IXUSR
#define YFW_S_IFDIR S_IFDIR
#define YFW_S_IFREG S_IFREG
#define YFW_S_IFMT S_IFMT
#define YFW_O_RDONLY O_RDONLY
#define YFW_O_NONBLOCK O_NONBLOCK

#define YFW_LINES LINES
#define YFW_COLS COLS

#define YFW_OPTIND optind
#define YFW_OPTARG optarg

#define MAX_ASSOCS_RESERVE 10
typedef struct {
	char *regex;
	char *bin[MAX_ASSOCS_RESERVE];
}assoc_t;

typedef enum  {
    SEL_NOACTION = 1,
	SEL_QUIT,
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
	SEL_VIEW,
	SEL_EDIT,
	SEL_SHELL,
	SEL_RUNARG,
	SEL_NEXTCOL,
	SEL_PREVCOL,
	SEL_FASTDIR,
	SEL_ADDCOL,
	SEL_REMOVECOL,
	SEL_SORTCOL,
	SEL_STACKMODE,
	SEL_BINBINDING,
}action_t;

typedef enum {
    STACK_NOACTION = 1,
    STACK_DELFILE,
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
	int sym;
	action_t nm;
	stack_mode_action_t sm;
	int bb;
}yf_key_t;

typedef enum {
    NO_FLAG,
    DISOWN_SILENTLY,
    PIPE_TO_SHELL,
    PIPE_TO_VIEWER,
}spawn_flags_t;

typedef struct {
    int key;
    char* bin;
    spawn_flags_t pipe;
}binbinding_t;

typedef struct {
    const char* bin;
    const char* argv;
    const char* new_dir;
    spawn_flags_t flags;

}fire_spawn_t; /* You are in a computer game, Max! */

typedef enum {
    DEFAULT_FILE_OPS,
    USE_SYSTEM_BINARIES,
    BRUTEFORCE666
}file_ops_styles_t;

typedef enum {
    YF_VIEW,
    YF_EDIT,
    YF_RUN,
}default_behaviour_types_t;

typedef enum {
    YF_HEXEDIT,
    YF_METADATAEXTRACT,
    YF_ZIP,
    YF_UNZIP,
}rare_behaviour_types_t;

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

typedef enum {
    NORMAL_MODE,
    STACK_MODE,
}global_ui_state_t;

typedef enum {
    NOTHING_CHANGES,
    NEEDS_REDRAW,
    ERROR
}update_state_t;

typedef enum {
    NOTMAL_STACK_MODE,
    VISUAL_MODE,
}stack_mode_state;

#define MAX_COLS 32
#define MAX_STATUS_LINE 128
typedef struct {
    column_t cols[MAX_COLS];
    short curcol, totalcols;
    int idle;
    time_t lastupdatetimestamp;

	char fltr[LINE_MAX];
	char fastdir[PATH_MAX];

    entry_t* directory_stack;
    uint32_t dsp, dsc;

    entry_t* files_stack;
    uint32_t fsp, fsc;

    char status_line[MAX_STATUS_LINE];
    global_ui_state_t mode;

    char* ifilter;

    yfw_window_t *window;

    int recursion_depth;
}filemanager_t;


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

typedef enum {
    ENV_SHELL,
    ENV_TERM,
    ENV_HOME
}env_vars_list_t;

#ifdef __cplusplus
}
#endif /* __cpluspus */
#endif /* YESFIRE_H */




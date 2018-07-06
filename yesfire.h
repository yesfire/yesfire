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
	int sym;         /* Key pressed */
	action_t nm; /* Action */
	stack_mode_action_t sm;
	int bb;
}yf_key_t;

typedef struct {
    int key;
    char* bin;
}binbinding_t;

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

#define MAX_COLS 32 /* Max number of columns */
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
    global_ui_state_t mode;

    char* ifilter;
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

#ifdef __cplusplus
}
#endif /* __cpluspus */
#endif /* YESFIRE_H */




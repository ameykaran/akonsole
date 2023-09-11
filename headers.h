#define _XOPEN_SOURCE 500
#define _DEFAULT_SOURCE
#ifndef HEADERS_H_
#define HEADERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#include <fcntl.h>
#include <errno.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <ftw.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "prompt.h"
#include "pastevents.h"
#include "warp.h"
#include "utility.h"
#include "peek.h"
#include "seek.h"
#include "proclore.h"
#include "command_handler.h"
#include "fgbg.h"
#include "iman.h"
#include "activities.h"
#include "signal.h"

#define ANSI_FG_COLOR_BLACK "\x1b[30m"
#define ANSI_FG_COLOR_RED "\x1b[31m"
#define ANSI_FG_COLOR_GREEN "\x1b[32m"
#define ANSI_FG_COLOR_YELLOW "\x1b[33m"
#define ANSI_FG_COLOR_BLUE "\x1b[34m"
#define ANSI_FG_COLOR_MAGENTA "\x1b[35m"
#define ANSI_FG_COLOR_CYAN "\x1b[36m"
#define ANSI_FG_COLOR_WHITE "\x1b[37m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define min(a, b) a < b ? a : b
#define max(a, b) a > b ? a : b

extern char HOME_DIR[PATH_MAX];
extern char CURR_DIR[PATH_MAX];
extern char PREV_DIR[PATH_MAX];
extern long TERMINAL_PID;
extern char PREV_COMMAND[1024];
extern char PREV_COMMAND_OUTPUT[1024];
extern processList *Processes;
#define MAX_ARG_NUM 15 


void printLogo();
#endif
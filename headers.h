#ifndef HEADERS_H_
#define HEADERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#include <fcntl.h>

#include "prompt.h"
#include "pastevents.h"

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

void rstrip(char *str, char letter);

#endif
#include "headers.h"

void rstrip(char *str, char letter)
{
    int len = strlen(str);
    while (str[len - 1] == letter)
        str[--len] = '\0';
}

void print_error(char *msg)
{
    printf(ANSI_FG_COLOR_RED "%s" ANSI_COLOR_RESET "\n", msg);
}
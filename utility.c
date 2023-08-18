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

char *get_abs_path(char *path)
{
    char *absPath = (char *)calloc(PATH_MAX, sizeof(char));
    int i = 0;
    int startInd = 1;

    if (!strcmp(path, "-"))
    {
        !strcmp(PREV_DIR, "") && strcpy(absPath, PREV_DIR);
        return absPath;
    }

    if (path[0] == '~')
        for (; HOME_DIR[i]; i++)
            absPath[i] = HOME_DIR[i];

    if (!strcmp(path, "."))
        strcpy(absPath, CURR_DIR);

    else if (!strcmp(path, ".."))
    {
        strcpy(absPath, CURR_DIR);
        for (int j = strlen(absPath) - 1; j > -1; j--)
            if (absPath[j] == '/')
            {
                absPath[j] = '\0';
                break;
            }
        startInd = 2;
    }

    for (int j = startInd; j < strlen(path); i++, j++)
        absPath[i] = path[j];

    return absPath;
}

char *get_rel_path(char *path)
{
    if (!strncmp(CURR_DIR, path, strlen(CURR_DIR)))
    {
        path = path + strlen(HOME_DIR) + 1;
    }
    else if (!strncmp(HOME_DIR, path, strlen(HOME_DIR)))
    {
        path = path + strlen(HOME_DIR) - 1;
        path[0] = '~';
        path[1] = '/';
    }

    return path;
}

void print_aligned(long num, int digits)
{
    int copy = num;
    int dg = 1;
    for (int copy = num; copy /= 10; dg++)
        ;

    for (int i = 0; i < digits - dg; i++)
        printf(" ");
    printf("%ld ", num);
}

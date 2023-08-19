#include "headers.h"

char *rstrip(char *str, char letter)
{
    int len = strlen(str);
    while (str[len - 1] == letter)
        str[--len] = '\0';
    return str;
}

void print_error(char *msg)
{
    printf(ANSI_FG_COLOR_RED "%s" ANSI_COLOR_RESET "\n", msg);
}

char *get_abs_path(char *path, int expandDots)
{
    char *absPath = (char *)calloc(PATH_MAX, sizeof(char));
    int changed = 0;

    if ((path[0] == '/') || (!expandDots && path[0] == '.'))
    {
        strcpy(absPath, path);
        return absPath;
    }

    if (!strcmp(path, "-"))
    {
        !strcmp(PREV_DIR, "") && strcpy(absPath, PREV_DIR);
        return absPath;
    }

    if (path[0] == '~')
    {
        changed = 1;
        strcpy(absPath, HOME_DIR);
        strcat(absPath, "/");
        path += 1;
    }
    if (path[1] == '.' && path[0] == '.')
    {
        changed = 1;
        strcpy(absPath, CURR_DIR);
        if (!strcmp(absPath, "~"))
            strcpy(absPath, HOME_DIR);
        for (int j = strlen(absPath) - 1; j > -1; j--)
            if (absPath[j] == '/')
            {
                absPath[j] = '\0';
                break;
            }
        strcat(absPath, "/");
        path += 2;
    }
    else if (path[0] == '.')
    {
        changed = 1;
        strcpy(absPath, CURR_DIR);
        strcat(absPath, "/");
        path += 1;
    }

    if (!changed)
    {
        strcpy(absPath, "./");
        strcat(absPath, path);
        return get_abs_path(absPath, 1);
    }

    if (path[0] && path[1])
        strcat(absPath, path + (changed ? 1 : 0));

    return absPath;
}

char *get_rel_path(char *path)
{
    if (!strcmp(path, HOME_DIR))
    {
        path[0] = '~';
        path[1] = '\0';
        return path;
    }
    if (!strncmp(HOME_DIR, path, strlen(HOME_DIR)))
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

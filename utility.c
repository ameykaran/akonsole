#include "headers.h"

char *rstrip(char *string, char letter)
{
    char *str = strdup(string);
    int len = strlen(str);
    while (str[len - 1] == letter)
        str[--len] = '\0';
    return str;
}

char *lstrip(char *string, char letter)
{
    char *str = strdup(string);
    int i = 0;
    while (str[i++] == letter)
        ;
    return str + i - 1;
}

char *strip(char *str, char letter)
{
    return lstrip(rstrip(str, letter), letter);
}

void print_error(char *msg)
{
    printf(ANSI_FG_COLOR_RED "%s" ANSI_COLOR_RESET "\n", msg);
}

/*Get the absolute path of the given argument. Can return null path. Free the argument when possible*/
char *get_abs_path(char *path)
{
    char *absPath = (char *)calloc(PATH_MAX, sizeof(char));
    // int changed = 0;

    if ((path[0] == '/'))
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
        // changed = 1;
        strcpy(absPath, HOME_DIR);
        strcat(absPath, "/");
        path += 1;
    }

    strcat(absPath, path);
    // if (path[0] == '.' && path[1] == '.')
    // {
    //     changed = 1;
    //     strcpy(absPath, CURR_DIR);
    //     if (!strcmp(absPath, "~"))
    //         strcpy(absPath, HOME_DIR);
    //     for (int j = strlen(absPath) - 1; j > -1; j--)
    //         if (absPath[j] == '/')
    //         {
    //             absPath[j] = '\0';
    //             break;
    //         }
    //     strcat(absPath, "/");
    //     path += 2;
    // }
    // else if (path[0] == '.')
    // {
    //     changed = 1;
    //     strcpy(absPath, CURR_DIR);
    //     if (!strcmp(absPath, "~"))
    //         strcpy(absPath, HOME_DIR);
    //     strcat(absPath, "/");
    //     path += 1;
    // }

    // if (!changed)
    // {
    //     strcpy(absPath, CURR_DIR);
    //     strcat(absPath, "/");
    //     strcat(absPath, path);
    //     return get_abs_path(absPath, 1);
    // }

    // if (path[0] && path[1])
    //     strcat(absPath, path + (changed ? 1 : 0));

    return absPath;
}

char *get_rel_path(char *path)
{
    char *dup = strdup(path);
    if (!strcmp(path, HOME_DIR))
    {
        dup[0] = '~';
        dup[1] = '\0';
        return dup;
    }
    if (!strncmp(HOME_DIR, path, strlen(HOME_DIR)))
    {
        dup = dup + strlen(HOME_DIR) - 1;
        dup[0] = '~';
        dup[1] = '/';
    }
    return dup;
}

char *remove_whitespaces(char *text)
{
    char *str = strdup(text);
    int i = 0, j = 0;
    int n = strlen(str);
    while (j < n)
    {
        while (str[j] == ' ' && str[j + 1] == ' ')
            j++;
        if (i != j)
            str[i] = str[j];
        i++;
        j++;
    }
    str[i] = '\0';
    free(text);
    return strip(str, ' ');
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

char *trim(char *string, char *prefix)
{
    char *new = strdup(string);
    int i = 0;
    while (new[i] == prefix[i])
        i++;
    new = new + i + (i == 0 ? 0 : 1);
    return new;
}
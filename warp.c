#include "headers.h"

extern char HOME_DIR[PATH_MAX];
extern char CURR_DIR[PATH_MAX];
extern char PREV_DIR[PATH_MAX];

void cd_prev()
{
    cd(PREV_DIR);
}

void cd(char *path)
{

    // if (path[0] == '-')
    //     cd(PREV_DIR);
    // else

    if (!strcmp(path, "~") || !strcmp(path, ""))
    {
        cd(HOME_DIR);
        return;
    }
    if (!strcmp(path, "-"))
    {
        cd(PREV_DIR);
        return;
    }

    char absPath[PATH_MAX];
    int i = 0;

    if (path[0] == '~')
        for (; HOME_DIR[i]; i++)
            absPath[i] = HOME_DIR[i];

    else if (!strcmp(path, "."))
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
    }

    for (int j = 1; j < strlen(path); i++, j++)
        absPath[i] = path[j];

    printf("Path - %s\n", absPath);

    if (chdir(absPath) == -1)
    {
        if (errno == EACCES)
            print_error("Permission denied!\n");
        else if (errno == ENOTDIR)
            print_error("The given path is not a directory~\n");
        else if (errno == ENOENT)
            print_error("The given path doesn't exist!\n");
        else if (errno == ENAMETOOLONG)
            print_error("Path length too long!\n");
        else
            print_error("An error occurred!\n");
    }
    else
    {
        strcpy(PREV_DIR, CURR_DIR);
        strcpy(CURR_DIR, absPath);
    }

    printf("%s\n", CURR_DIR);
}

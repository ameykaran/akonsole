#include "headers.h"

void cd(char *path)
{
    if (!strcmp(path, "-"))
    {
        if (!strcmp(PREV_DIR, ""))
        {
            print_error("Previous path not available.");
            return;
        }
        cd(PREV_DIR);
        return;
    }
    if (!strcmp(path, "~") || !strcmp(path, ""))
    {
        cd(HOME_DIR);
        return;
    }

    char *absPath = get_abs_path(path, 1);

    if (chdir(get_abs_path(absPath, 0)) == -1)
    {
        if (errno == EACCES)
            print_error("Permission denied!");
        else if (errno == ENOTDIR)
            print_error("The given path is not a directory");
        else if (errno == ENOENT)
            print_error("The given path doesn't exist!");
        else if (errno == ENAMETOOLONG)
            print_error("Path length too long");
        else
            print_error("An error occurred");
    }
    else
    {
        strcpy(PREV_DIR, CURR_DIR);
        strcpy(CURR_DIR, rstrip(absPath, '/'));
    }

    if (absPath[0])
        free(absPath);
    printf("%s\n", CURR_DIR);
}

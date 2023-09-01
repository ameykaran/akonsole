#include "headers.h"

void cd(char *path, int printOutput)
{
    if (!strcmp(path, "-"))
    {
        if (!strcmp(PREV_DIR, ""))
        {
            if (!printOutput)
                print_error("Previous path not available.");
            return;
        }
        cd(PREV_DIR, printOutput);
        return;
    }
    if (!strcmp(path, "~"))
    {
        cd(HOME_DIR, printOutput);
        return;
    }

    char *absPath = get_abs_path(path);

    if (chdir(absPath) == -1)
    {
        if (!printOutput)
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
    }
    else
    {
        strcpy(PREV_DIR, CURR_DIR);
        absPath = getcwd(absPath, PATH_MAX);
        strcpy(CURR_DIR, rstrip(absPath, '/'));
    }

    if (absPath[0])
        free(absPath);
    if (!printOutput)
        printf("%s\n", CURR_DIR);
}

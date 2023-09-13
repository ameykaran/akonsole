#include "headers.h"

int warpHandler(int argc, char *argv[])
{
    if (argc == 1)
        cd(HOME_DIR, 0);
    for (int i = 1; i < argc; i++)
        cd(argv[i], 0);
}

int peekHandler(int argc, char *argv[])
{
    int hidden = 0, detailed = 0;
    char path[PATH_MAX];
    strcpy(path, argv[argc - 1]);

    for (int i = 1; i < argc; i++)
    {
        char *arg = argv[i];
        if (arg[0] == '-')
        {
            for (int j = 1; j < strlen(arg); j++)
            {
                if (arg[j] == 'a')
                    hidden = 1;
                else if (arg[j] == 'l')
                    detailed = 1;
                else
                {
                    print_error("Unkown flags");
                    // TODO add peek usage
                    return 1;
                }
            }
        }
    }

    if (argv[argc - 1][0] == '-' || !strcmp(argv[argc - 1], "peek"))
        strcpy(path, ".");
    ls(path, hidden, detailed);
    return 0;
}

int pasteventsHandler(int argc, char *argv[])
{
    if (argc == 1)
    {
        print_history();
    }
    else if (!strcmp(argv[1], "purge"))
    {
        delete_history();
    }
    else
    {
        print_error("Invalid synatx of pastevents");
        printf(ANSI_FG_COLOR_BLUE "---Usage---\npastevents               - prints the last 15 commands\npastevents purge         - deletes all saved commands\npastevents execute <num> - executes the <num>th recent command\n" ANSI_COLOR_RESET);
    }
}

int procloreHandler(int argc, char *argv[])
{
    if (argc == 2)
    {
        char *temp;
        long pid = strtoll(argv[1], &temp, 10);
        proclore(pid);
    }
    else if (argc == 1)
    {
        char termPID[1024];
        sprintf(termPID, "%ld", TERMINAL_PID);
        proclore(TERMINAL_PID);
    }
    else
        // TODO add usage
        print_error("Wrong usage");
}

int seekHandler(int argc, char *argv[])
{
    char *name = (char *)calloc(PATH_MAX, sizeof(char));
    char *path = (char *)calloc(PATH_MAX, sizeof(char));
    char flags = 0;

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            for (int j = 1; j < strlen(argv[i]); j++)
            {
                if (argv[i][j] == 'd')
                    flags |= SEEK_DIR;
                else if (argv[i][j] == 'f')
                    flags |= SEEK_FILE;
                else if (argv[i][j] == 'l')
                    flags |= SEEK_LINK;
                else if (argv[i][j] == 'e')
                    flags |= SEEK_FLAG_EXEC;
                else if (argv[i][j] == 'h')
                    flags |= SEEK_FLAG_HIDDEN;
                else
                {
                    print_error("Unknown flag");
                    // Todo usage
                    return 1;
                }
            }
        }
        else
        {
            if (flags & SEEK_FLAG_NAME)
            {
                strcpy(path, argv[i]);
                flags |= SEEK_FLAG_PATH;
            }
            else
            {
                strcpy(name, argv[i]);
                flags |= SEEK_FLAG_NAME;
            }
        }
    }

    if ((SEEK_IS_DIR(flags) && SEEK_IS_FILE(flags)) ||
        (SEEK_IS_DIR(flags) && SEEK_IS_LINK(flags)) ||
        (SEEK_IS_LINK(flags) && SEEK_IS_FILE(flags)))
    {
        print_error("Invalid combination of flags");
        // Todo usage
        return 1;
    }

    if (name[0] == '\0')
    {
        print_error("Enter the entry to search for");
        // Todo usage
        return 1;
    }

    if (!(flags & SEEK_FLAG_PATH))
        strcpy(path, ".");
    char *newpath = get_abs_path(path);
    free(path);
    if (strcmp(newpath, "/"))
        newpath = rstrip(newpath, '/');

    if (!(SEEK_IS_DIR(flags) | SEEK_IS_FILE(flags) | SEEK_IS_LINK(flags)))
        flags |= SEEK_DIR | SEEK_FILE | SEEK_LINK;

    seek(newpath, flags, name);
}

int exitHandler()
{
    printf("Exiting the terminal...\n");
    exit(EXIT_SUCCESS);
}

int sysCmdHandler(int argc, char *argv[])
{
    argv[argc] = NULL;
    execvp(argv[0], argv);
}

int imanHandler(int argc, char *argv[])
{
    if (argc == 1)
    {
        print_error("Enter the command to get the manual for");
        return 1;
    }

    if (argc > 2)
    {
        print_error("Enter only one command");
        return 1;
    }

    get_man_page(argv[1]);
}

int activitiesHandler(int argc, char *argv[])
{
    activities();
}

int pingHandler(int argc, char *argv[])
{
    if (argc != 3)
    {
        print_error("Invalid syntax");
        return 1;
    }
    ping(atoi(argv[1]), atoi(argv[2]));
    return 0;
}

int fgHandler(int argc, char *argv[])
{
    if (argc != 2)
    {
        print_error("Invalid syntax");
        return 1;
    }
    fg(atoi(argv[1]));
    return 0;
}

int bgHandler(int argc, char *argv[])
{
    if (argc != 2)
    {
        print_error("Invalid syntax");
        return 1;
    }
    bg(atoi(argv[1]));
    return 0;
}

int neonateHandler(int argc, char *argv[])
{
    int time = 0;
    if (argc != 1 && argc != 2)
    {
        print_error("Invalid syntax");
        return 1;
    }
    if (argc == 2)
        time = atoi(argv[1]);
    neonate(time);
    return 0;
}

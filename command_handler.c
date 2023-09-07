#include "headers.h"

void execute_single_line_command(char *cmd);

int warpHandler(int argc, char *argv[])
{
    if (argc == 1)
        cd(HOME_DIR, 1);
    for (int i = 1; i < argc; i++)
        cd(argv[i], 1);
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
    else if (!strncmp(argv[1], "execute ", 9))
    {
        int i = 1;
        char
            cmd[BUFFER_LARGE] = {0};
        while (i <= argc)
        {
            strcat(cmd, argv[i]);
            strcat(cmd, " ");
        }

        char *execCmd = replace_pastevents_execute(cmd);

        if (strcmp(execCmd, "") && strcmp(execCmd, " "))
        {
            print_error("The given command is erroneous.");
            return 1;
        }

        execute_single_line_command(execCmd);
        return 0;
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
       strcpy( path, ".");
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
    char cmd[BUFFER_SMALL] = {0};
    char *temp;
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

    get_man_page(cmd);
}

int activitiesHandler(int argc, char *argv[])
{
    activities();
}

cmdMap cmdTable[] = {
    {"", UNKNOWN, sysCmdHandler},
    {"exit", EXIT, exitHandler},
    {"warp", WARP, warpHandler},
    {"peek", PEEK, peekHandler},
    {"pastevents", PASTEVENTS, pasteventsHandler},
    {"proclore", PROCLORE, procloreHandler},
    {"seek", SEEK, seekHandler},
    {"iman", IMAN, imanHandler},
    {"activities", ACTIVITIES, activitiesHandler}};

void execute_multi_line_command(char *cmd)
{
    // printf("Command: '%s'\n", cmd);
    char *arg = strdup(cmd);
    arg = remove_whitespaces(arg);
    arg = replace_pastevents_execute(arg);
    if (arg == "")
    {
        print_error("The given command is erroneous.");
        return;
    }

    save_command(arg);

    char *temp;
    char *command = strtok_r(arg, ";", &temp);
    while (command)
    {
        execute_single_line_command(strip(strdup(command), ' '));
        command = strtok_r(NULL, ";", &temp);
    }
}

int execute_command(execHandler handler, int argc, char *argv[], char isBg)
{
    int pid = fork();
    if (pid < 0)
    {
        print_error("Could not fork the terminal");
        return 1;
    }
    else if (pid == 0)
    {
        if (isBg)
        {
            FILE *outputFile = fopen(PREV_COMMAND_OUTPUT, "w");
            if (outputFile == NULL)
            {
                perror("fopen");
                return 1;
            }
            int fileDescriptor = fileno(outputFile);
            if (dup2(fileDescriptor, STDOUT_FILENO) == -1)
            {
                perror("dup2");
                return 1;
            }

            fclose(outputFile);

            handler(argc, argv);

            printf(ANSI_FG_COLOR_YELLOW "%s", argv[0]);
            print_error(" No such command found");
            exit(0);
        }
        else
        {
            handler(argc, argv);

            printf(ANSI_FG_COLOR_YELLOW "%s", argv[0]);
            print_error(" No such command found");
            exit(0);
        }
    }
    else
    {
        insert_process(pid, argc, argv, isBg);
        if (isBg)
            printf("[%d] %d\n", Processes->size, pid);
        else
        {
            int status;
            waitpid(pid, &status, 0);
            return 0;
        }
    }
}

void checkIORedirect() {}

void run_single_command(char *cmd, int isBg)
{
    char **argv = (char **)malloc(MAX_ARG_NUM * sizeof(char *));
    int argc = 0;

    char *temp, *curr;
    curr = strtok_r(strdup(cmd), " ", &temp);
    do
    {
        argv[argc++] = strdup(curr);
        curr = strtok_r(NULL, " ", &temp);
    } while (curr);
    // strcpy(argv[argc],);

    cmdMap *evaluatedCmd = NULL;
    for (int i = 0; i < sizeof(cmdTable) / sizeof(cmdMap); i++)
        if (!strcmp(argv[0], cmdTable[i].cmdName))
        {
            evaluatedCmd = cmdTable + i;
            break;
        }

    evaluatedCmd = evaluatedCmd ? evaluatedCmd : cmdTable;
    if (evaluatedCmd->cmdID == EXIT)
        evaluatedCmd->handler(0, NULL);

    time_t srtTime = 0, endTime = 0;
    srtTime = time(NULL);

    if (evaluatedCmd->cmdID != UNKNOWN)
    {
        evaluatedCmd->handler(argc, argv);
        printf("\n");
    }
    else
    {
        execute_command(evaluatedCmd->handler, argc, argv, isBg);
        endTime = time(NULL);
        if (!isBg)
        {
            printf(ANSI_FG_COLOR_YELLOW "\n%s " ANSI_COLOR_RESET "ran in " ANSI_FG_COLOR_MAGENTA "%lds" ANSI_COLOR_RESET "\n\n", argv[0], endTime - srtTime);

            if (endTime - srtTime > 2)
            {
                strcpy(PREV_COMMAND, " " ANSI_FG_COLOR_YELLOW);
                strcat(PREV_COMMAND, argv[0]);
                strcat(PREV_COMMAND, ANSI_COLOR_RESET ":" ANSI_FG_COLOR_MAGENTA);

                char time[100];
                sprintf(time, "%lds", endTime - srtTime);
                strcat(PREV_COMMAND, time);
                strcat(PREV_COMMAND, "" ANSI_COLOR_RESET);
            }
        }
    }
    free(argv);
    return;
}

void execute_single_line_command(char *cmd)
{
    char *arg = strdup(cmd);
    char *command = trim(rstrip(arg, '\n'), " ");

    if (!strcmp(command, ""))
        return;

    int ampCount = 0;
    for (int i = 0; command[i]; i++)
        if (command[i] == '&')
            ampCount++;

    char *bgCmd;
    char *temp;
    bgCmd = strtok_r(command, "&", &temp);

    while (ampCount)
    {
        run_single_command(strip(strdup(bgCmd), ' '), 1);
        ampCount--;
        bgCmd = strtok_r(NULL, "&", &temp);
    }

    if (bgCmd)
        run_single_command(strip(strdup(bgCmd), ' '), 0);
}

void print_last_exec_output()
{
    long bytes = 0;
    // FILE *output = fopen("/home/amey/output", "a+");
    FILE *output = fopen(PREV_COMMAND_OUTPUT, "a+");
    if (output < 0)
    {
        print_error("Not able to open previous output buffer");
        return;
    }

    if (output)
    {
        char buffer[1024];
        while (fgets(buffer, 1024, output))
        {
            printf("%s", buffer);
            bytes += strlen(buffer);
        }

        if (bytes)
            printf("\n");
        fclose(output);
    }
    output = fopen(PREV_COMMAND_OUTPUT, "w");
    if (output < 0)
    {
        print_error("Not able to open previous output buffer");
        return;
    }
    fputs("", output);
    fclose(output);

    return;
}

void kill_children(int id)
{
    dup2(STDOUT_FILENO, STDOUT_FILENO);

    int status, pid;
    if ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        printf("%d\n", pid);
        processNode *process = get_process_with_id(pid);
        if (!process)
        {
            print_error("Could not find process ");
            return;
        }

        if (status == 0)
            printf("[%d] %d exited " ANSI_FG_COLOR_GREEN "successfully " ANSI_FG_COLOR_YELLOW "- %s" ANSI_COLOR_RESET "\n", Processes->size - 1, pid, process->pName);
        else
            printf("[%d] %d exited " ANSI_FG_COLOR_RED "abnormally " ANSI_COLOR_RESET "with error: %d" ANSI_FG_COLOR_YELLOW " - %s" ANSI_COLOR_RESET "\n", Processes->size - 1, pid, status, process->pName);
        // remove_process_with_id(pid);
    }
}

void kill_terminal(int id)

{
    // signal(id, SIG_IGN);
    printf("Do you really want to quit? [y/n] ");
    char c = getchar();
    printf("*%d*%s*\n", c, &c);

    // if (c == 10)
    //     signal(SIGINT, kill_terminal);
    if (c == 'y' || c == 'Y')
        (cmdTable + 1)->handler(0, NULL);
    else
        signal(SIGINT, kill_terminal);
    getchar();
}

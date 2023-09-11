#include "headers.h"

int inpBackup = -1, inpTextBackup = -1, outBackup = -1;

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
                perror("Background buffer");
                return 1;
            }
            int fileDescriptor = fileno(outputFile);
            if (dup2(fileDescriptor, STDOUT_FILENO) == -1)
            {
                perror("Background buffer");
                return 1;
            }
            close(fileDescriptor);
            fclose(outputFile);

            handler(argc, argv);

            RESET_IO_REDIRECTION;
            printf(ANSI_FG_COLOR_YELLOW "%s", argv[0]);
            print_error(" No such command found");
            exit(0);
        }
        else
        {
            handler(argc, argv);
            RESET_IO_REDIRECTION;
            printf(ANSI_FG_COLOR_YELLOW "%s", argv[0]);
            print_error(" No such command found");
            exit(0);
        }
    }
    else
    {
        insert_process(pid, argc, argv[0], isBg);
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

void readTextInput(char **string, char *text)
{
    int i = readString(*string, text);
    *string = *string + i;
}

int checkIORedirect(char *cmd, IOQuadrio *quadrio)
{
    char newCmd[1000]; // TODO CHANGE to argmax
    int newInd = 0;
    for (int i = 0; i < strlen(cmd); i++)
    {
        if (!strncmp(cmd + i, ">>", 2))
        {
            newCmd[newInd++] = ' ';
            newCmd[newInd++] = '>';
            newCmd[newInd++] = '>';
            newCmd[newInd++] = ' ';
            i++;
        }
        else if (!strncmp(cmd + i, ">", 1))
        {
            newCmd[newInd++] = ' ';
            newCmd[newInd++] = '>';
            newCmd[newInd++] = ' ';
        }
        else if (!strncmp(cmd + i, "<<<", 3))
        {
            newCmd[newInd++] = ' ';
            newCmd[newInd++] = '<';
            newCmd[newInd++] = '<';
            newCmd[newInd++] = '<';
            newCmd[newInd++] = ' ';
            i += 2;
        }
        else if (!strncmp(cmd + i, "<", 1))
        {
            newCmd[newInd++] = ' ';
            newCmd[newInd++] = '<';
            newCmd[newInd++] = ' ';
        }
        else if (!strncmp(cmd + i, "<>", 2))
            i++;
        else if (!strncmp(cmd + i, "<>>", 3))
            i += 2;
        else
            newCmd[newInd++] = cmd[i];
    }
    newCmd[newInd] = 0;

    char *temp, *inp = 0, *out = 0, *app = 0, *inpText = 0, new[1000] = {0}; // todo change argmax
    char *str = strtok_r(newCmd, " ", &temp);
    char flag = 0, inpFlag = 0, outFlag = 0;
    while (str)
    {
        if (flag == IO_INP)
        {
            if (inpFlag)
            {
                print_error("Multiple inputs found");
                return 1;
            }
            if (strcmp(str, ">") && strcmp(str, "<") && strcmp(str, ">>"))
                inp = strdup(str);
            flag = 0;
            inpFlag += 1;
            str = strtok_r(NULL, " ", &temp);
            continue;
        }
        else if (flag == IO_INP_TEXT)
        {
            if (inpFlag)
            {
                print_error("Multiple inputs found");
                return 1;
            }
            if (strcmp(str, ">") && strcmp(str, "<") && strcmp(str, ">>"))
            {
                char res[NAME_MAX];
                char *tempCat = strdup(str);
                strcat(tempCat, " ");
                strcat(tempCat, temp);
                readTextInput(&tempCat, res);
                inpText = strdup(res);
                temp = tempCat;
            }
            flag = 0;
            inpFlag += 1;
            str = strtok_r(NULL, " ", &temp);
            continue;
        }
        else if (flag == IO_OUT)
        {
            if (outFlag)
            {
                print_error("Multiple outputs found");
                return 1;
            }
            if (!strcmp(str, ">") || !strcmp(str, "<") || !strcmp(str, ">>"))
            {
                print_error("IO Redirection: Empty output file");
                printf("Use '>' or '>>' to redirect the output to a file\n");
                return 1;
            }
            out = strdup(str);
            flag = 0;
            outFlag += 1;
            str = strtok_r(NULL, " ", &temp);
            continue;
        }
        else if (flag == IO_APP)
        {
            if (outFlag)
            {
                print_error("Multiple outputs found");
                return 1;
            }
            if (!strcmp(str, ">") || !strcmp(str, "<") || !strcmp(str, ">>"))
            {
                print_error("IO Redirection: Empty output file");
                printf("Use '>' or '>>' to redirect the output to a file\n");
                return 1;
            }
            app = strdup(str);
            flag = 0;
            outFlag += 1;
            str = strtok_r(NULL, " ", &temp);
            continue;
        }

        if (!strcmp(str, "<"))
            flag = IO_INP;
        else if (!strcmp(str, ">"))
            flag = IO_OUT;
        else if (!strcmp(str, "<<<"))
            flag = IO_INP_TEXT;
        else if (!strcmp(str, ">>"))
            flag = IO_APP;
        else
        {
            strcat(new, str);
            strcat(new, " ");
        }

        str = strtok_r(NULL, " ", &temp);
    }
    if (inp)
        quadrio->inp = strdup(inp);
    if (inpText)
        quadrio->inpText = strdup(inpText);
    if (out)
        quadrio->out = strdup(out);
    if (app)
        quadrio->app = strdup(app);
    printf("%s\t%s\t%s\t%s\n", inp, inpText, out, app);
    strcpy(cmd, new);
    return 0;
}

void run_single_command(char *cmd, int isBg, IOQuadrio quadrio)
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

    if (quadrio.inp)
    {
        int inpFd = open(quadrio.inp, O_RDONLY);
        if (inpFd == -1)
        {
            perror("IORedirection");
            return;
        }
        inpBackup = dup(STDIN_FILENO);
        dup2(inpFd, STDIN_FILENO);
        close(inpFd);
    }

    if (quadrio.inpText)
    {
        FILE *tmpFile = tmpfile();
        // fprintf(tmpFile, "%s", quadrio.inpText);
        fwrite(quadrio.inpText, sizeof(char), strlen(quadrio.inpText), tmpFile);
        fflush(NULL);
        fseek(tmpFile, 0, SEEK_SET);

        int inpFd = fileno(tmpFile);
        if (inpFd == -1)
        {
            perror("IORedirection");
            return;
        }
        inpBackup = dup(STDIN_FILENO);
        dup2(inpFd, STDIN_FILENO);
        inpTextBackup = dup(inpFd);
        close(inpFd);
        fclose(tmpFile);
    }

    if (quadrio.out)
    {
        int outFd = open(quadrio.out, O_CREAT | O_WRONLY | O_TRUNC, 0644);
        if (outFd == -1)
        {
            perror("IORedirection");
            return;
        }
        outBackup = dup(STDOUT_FILENO);
        dup2(outFd, STDOUT_FILENO);
        close(outFd);
    }
    if (quadrio.app)
    {
        int outFd = open(quadrio.out, O_CREAT | O_APPEND | O_TRUNC, 0644);
        if (outFd == -1)
        {
            perror("IORedirection");
            return;
        }
        outBackup = dup(STDOUT_FILENO);
        dup2(outFd, STDOUT_FILENO);
        close(outFd);
    }

    if (evaluatedCmd->cmdID != UNKNOWN)
    {
        evaluatedCmd->handler(argc, argv);
        RESET_IO_REDIRECTION;
        printf("\n");
    }

    else
    {
        execute_command(evaluatedCmd->handler, argc, argv, isBg);
        RESET_IO_REDIRECTION;
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
        IOQuadrio quadrio = {0};
        int ct = 0, arg[4] = {0};
        char *processed = strip(strdup(bgCmd), ' ');
        if (checkIORedirect(processed, &quadrio))
            return;
        run_single_command(processed, 1, quadrio);
        ampCount--;
        bgCmd = strtok_r(NULL, "&", &temp);
    }

    if (bgCmd)
    {
        IOQuadrio quadrio = {0};
        char *processed = strip(strdup(bgCmd), ' ');
        if (checkIORedirect(processed, &quadrio))
            return;
        if (bgCmd)
            run_single_command(processed, 0, quadrio);
    }
}
#include "headers.h"

int inpBackup = -1, inpTextBackup = -1, outBackup = -1;

void execute_single_line_command(char *cmd);

cmdMap cmdTable[] = {
    {"", UNKNOWN, sysCmdHandler},
    {"exit", EXIT, exitHandler},
    {"warp", WARP, warpHandler},
    {"peek", PEEK, peekHandler},
    {"pastevents", PASTEVENTS, pasteventsHandler},
    {"proclore", PROCLORE, procloreHandler},
    {"seek", SEEK, seekHandler},
    {"iman", IMAN, imanHandler},
    {"ping", PING, pingHandler},
    {"fg", FG, fgHandler},
    {"bg", BG, bgHandler},
    {"neonate", NEONATE, neonateHandler},
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
            // Background child process
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
            remove_process_with_id(pid);
            exit(0);
        }
        else
        {
            // Foreground child process
            handler(argc, argv);
            RESET_IO_REDIRECTION;
            printf(ANSI_FG_COLOR_YELLOW "%s", argv[0]);
            print_error(" No such command found");
            remove_process_with_id(pid);
            exit(0);
        }
    }
    else
    {
        // Parent process
        insert_process(pid, argc, argv[0], isBg);
        if (isBg)
            printf("[%d] %d\n", Processes->size, pid);
        else
        {
            int status;
            waitpid(pid, &status, WUNTRACED);
            // printf("Here\n");
            processNode *currProc = get_process_with_id(pid);
            if (!currProc)
                return 1;
            RESET_IO_REDIRECTION;

            // remove_process_with_id(pid);
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

void run_single_command(char *cmd, int isBg, IOQuadrio quadrio, int isPrintOutput)
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

    for (int i = 0; i < argc; i++)
        printf("%s+", argv[i]);

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

    srtTime = time(NULL);
    if (evaluatedCmd->cmdID != UNKNOWN)
    {
        evaluatedCmd->handler(argc, argv);
        RESET_IO_REDIRECTION;
        if (isPrintOutput)
            printf("\n");
    }

    else
    {
        execute_command(evaluatedCmd->handler, argc, argv, isBg);
        RESET_IO_REDIRECTION;
        endTime = time(NULL);
        if (!isBg && isPrintOutput)
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

int error(char *text)
{
    perror(text);
    return 1;
}

#define error_dup(fd1, fd2, msg) \
    if (dup2(fd1, fd2) == -1)    \
        return error(msg);

int execute_pipelets(char **pipelets, int pipeCt)
{
#define PIPE_READ 0
#define PIPE_WRITE 1

    int inpBackup = dup(STDIN_FILENO), outBackup = dup(STDOUT_FILENO);

    for (int i = 0; i <= pipeCt; i++)
    {
        int pipeFd[2];
        if (pipe(pipeFd) == -1)
            return error("Pipe creation");

        int pid = fork();
        if (pid < 0)
            return error("Pipe fork");

        else if (pid == 0)
        {
            close(pipeFd[PIPE_READ]);

            if (i == pipeCt)
            {
                error_dup(outBackup, STDOUT_FILENO, "Pipe out file dup");
                close(outBackup);
            }
            else
                error_dup(pipeFd[PIPE_WRITE], STDOUT_FILENO, "Pipe out file dup");

            IOQuadrio quadrio = {0};
            int ct = 0, arg[4] = {0};
            char *processed = strip(strdup(pipelets[i]), ' ');
            if (checkIORedirect(processed, &quadrio))
                return 2;
            error_dup(inpBackup, STDIN_FILENO, "Pipe inp file dup");

            if (dup2(inpBackup, STDIN_FILENO) == -1)
                return error("Pipe inp file dup");
            run_single_command(processed, 0, quadrio, 0);

            close(pipeFd[PIPE_WRITE]);
            error_dup(inpBackup, STDIN_FILENO, "Pipe inp file dup");
            close(inpBackup);
            exit(EXIT_SUCCESS);
        }
        // else
        // {
        int status;
        waitpid(pid, &status, WUNTRACED);

        close(pipeFd[PIPE_WRITE]);

        if (i == pipeCt)
        {
            dup2(inpBackup, STDIN_FILENO);
            close(inpBackup);
        }
        else
            dup2(pipeFd[PIPE_READ], STDIN_FILENO);
        // }

        close(pipeFd[PIPE_READ]);
    }

    // dup2(inpBackup, STDIN_FILENO);
    // dup2(outBackup, STDOUT_FILENO);

    // close(inpBackup);
    // close(outBackup);
}

void execute_single_line_command(char *cmd)
{
    char *arg = strdup(cmd);
    char *command = trim(rstrip(arg, '\n'), " ");
    char *leftCmd = strdup(command);

    if (!strcmp(command, ""))
        return;

    int ampCount = 0, pipeCount = 0;
    for (int i = 0; command[i]; i++)
        if (command[i] == '&')
            ampCount++;
        else if (command[i] == '|')
            pipeCount++;

    if (ampCount)
    {
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
            run_single_command(processed, 1, quadrio, 1);
            ampCount--;
            bgCmd = strtok_r(NULL, "&", &temp);
        }
        if (bgCmd)
            leftCmd = strdup(bgCmd);
        else
            leftCmd = 0;
    }
    else if (pipeCount)
    {
        char *temp, *pipelet;
        char *pipelets[pipeCount];
        pipelet = strtok_r(command, "|", &temp);

        for (int i = 0; i <= pipeCount; i++)
        {
            pipelets[i] = strdup(pipelet);
            pipelet = strtok_r(NULL, "|", &temp);
        }

        execute_pipelets(pipelets, pipeCount);
        return;
    }
    if (leftCmd)
    {
        IOQuadrio quadrio = {0};
        char *processed = strip(strdup(leftCmd), ' ');
        if (checkIORedirect(processed, &quadrio))
            return;
        if (leftCmd)
            run_single_command(processed, 0, quadrio, 1);
    }
}
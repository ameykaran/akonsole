#include "headers.h"

void execute_single_line_command(char *cmd);

int warpHandler(const char *args)
{
    char *arguments = strdup(args), *temp;
    char *arg = strtok_r(arguments, " ", &temp);
    if (!arg)
        cd("~", 0);
    while (arg)
    {
        cd(arg, 0);
        arg = strtok_r(NULL, " ", &temp);
    }
}

int peekHandler(const char *args)
{
    char *arguments = strdup(args), *temp;
    char *arg = strtok_r(arguments, " ", &temp);
    int hidden = 0, detailed = 0;
    char prev[PATH_MAX] = {0};
    while (arg)
    {
        if (arg[0] == '-')
        {
            if ((strlen(arg) == 2) || (strlen(arg) == 3))
            {
                if (arg[1] == 'a')
                    hidden = 1;
                else if (arg[1] == 'l')
                    detailed = 1;
                else
                {
                    print_error("Unkown flags");
                    // TODO add peek usage
                    return 1;
                }

                if (strlen(arg) == 3)
                {
                    if (arg[2] == 'a')
                        hidden = 1;
                    else if (arg[2] == 'l')
                        detailed = 1;
                    else
                    {
                        print_error("Unkown flags");
                        // TODO add peek usage
                        return 1;
                    }
                }
            }
            else
            {
                print_error("Unkown flags");
                // TODO add peek usage
                return 1;
            }
        }
        strcpy(prev, arg);
        arg = strtok_r(NULL, " ", &temp);
    }

    if (prev[0] == '-')
        strcpy(prev, ".");
    // printf("*%d*%d -%s\n", hidden, detailed, prev);
    ls(prev, hidden, detailed);
    return 0;
}

int pasteventsHandler(const char *args)
{
    if (!strncmp(args, "execute ", 9))
    {
        char *cmd = strdup(args);
        char *execCmd = replace_pastevents_execute(cmd);

        if (execCmd == "")
        {
            print_error("The given command is erroneous.");
            return 1;
        }

        execute_single_line_command(execCmd);
        free(cmd);
        return 0;
    }
    else if (!strcmp(args, "purge"))
    {
        delete_history();
    }
    else if (!strcmp(args, ""))
    {
        print_history();
    }
    else
    {
        print_error("Invalid synatx of pastevents");
        printf(ANSI_FG_COLOR_BLUE "---Usage---\npastevents               - prints the last 15 commands\npastevents purge         - deletes all saved commands\npastevents execute <num> - executes the <num>th recent command\n" ANSI_COLOR_RESET);
    }
}

int procloreHandler(const char *args)
{
    if (strcmp(args, ""))
    {
        char *str = strdup(args);
        char *temp;
        long pid = strtoll(str, &temp, 10);
        proclore(pid);
        free(str);
    }
    else
    {
        char termPID[1024];
        sprintf(termPID, "%ld", TERMINAL_PID);
        proclore(TERMINAL_PID);
    }
}

int seekHandler(const char *args)
{
    char *arguments = strdup(args), *temp;
    char *arg = strtok_r(arguments, " ", &temp);

    char *name = (char *)calloc(PATH_MAX, sizeof(char));
    char *path = (char *)calloc(PATH_MAX, sizeof(char));
    char flags = 0;

    while (arg)
    {
        if (arg[0] == '-')
        {
            for (int i = 1; i < strlen(arg); i++)
            {
                if (arg[i] == 'd')
                    flags |= SEEK_DIR;
                else if (arg[i] == 'f')
                    flags |= SEEK_FILE;
                else if (arg[i] == 'l')
                    flags |= SEEK_LINK;
                else if (arg[i] == 'e')
                    flags |= SEEK_FLAG_EXEC;
                else if (arg[i] == 'h')
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
                strcpy(path, arg);
                flags |= SEEK_FLAG_PATH;
            }
            else
            {
                strcpy(name, arg);
                flags |= SEEK_FLAG_NAME;
            }
        }
        arg = strtok_r(NULL, " ", &temp);
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

    path = get_abs_path(path ? path : ".", 1);
    if (strcmp(path, "/"))
        path = rstrip(path, '/');

    if (!(SEEK_IS_DIR(flags) | SEEK_IS_FILE(flags) | SEEK_IS_LINK(flags)))
        flags |= SEEK_DIR | SEEK_FILE | SEEK_LINK;

    seek(path, flags, name);
}

int exitHandler()
{
    printf("Exiting the terminal...\n");
    exit(EXIT_SUCCESS);
}

int sysCmdHandler(const char *args)
{

    char *arguments = strdup(args), *temp;
    char *arg = strtok_r(arguments, " ", &temp);

    char *argList[1024] = {NULL};
    int argCount = 0;

    while (arg)
    {
        argList[argCount++] = strdup(arg);
        arg = strtok_r(NULL, " ", &temp);
    }
    argList[argCount] = NULL;

    execvp(argList[0], argList);
}

cmdMap cmdTable[] = {
    {"", UNKNOWN, sysCmdHandler},
    {"warp", WARP, warpHandler},
    {"peek", PEEK, peekHandler},
    {"pastevents", PASTEVENTS, pasteventsHandler},
    {"proclore", PROCLORE, procloreHandler},
    {"seek", SEEK, seekHandler},
    {"exit", EXIT, exitHandler}};

typedef struct CommandNode
{
    char data[ARG_MAX];
    struct CommandNode *next;
} CommandNode;
typedef struct CommandsList
{
    CommandNode *head;
    CommandNode *tail;
} CommandsList;
CommandNode *createCommandNode(char *cmd)
{
    CommandNode *temp = (CommandNode *)malloc(sizeof(CommandNode));
    strcpy(temp->data, cmd);
    temp->next = NULL;
    return temp;
}
void enqueueList(CommandsList *list, char *command)
{
    CommandNode *temp = createCommandNode(command);

    if (!list->head)
    {
        list->head = temp;
        list->tail = temp;
    }
    else
    {
        list->tail->next = temp;
        list->tail = temp;
    }
}
CommandNode *dequeueList(CommandsList *list, char *command)
{
    CommandNode *start = list->head;
    if (start)
        list->head = start->next;
    return start;
}

void execute_multi_line_command(char *cmd)
{
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

    // CommandsList commandList = {NULL, NULL};
    while (command)
    {
        // enqueueList(&commandList, strip(strdup(command), ' '));
        execute_single_line_command(strip(strdup(command), ' '));
        command = strtok_r(NULL, ";", &temp);
    }

    // CommandNode *head = commandList.head;
    // CommandNode *next;
    // while (head)
    // {
    //     execute_single_line_command(head->data);
    //     next = head->next;
    //     free(head);
    //     head = next;
    // }
}

void split_raw_command(char *cmd, char **exec, char **args)
{
    char *rawCmd = strdup(cmd);

    for (int i = 0; i < strlen(rawCmd); i++)
    {
        if (rawCmd[i] != ' ')
            continue;
        *exec = rawCmd;
        *args = rawCmd + i + 1;
        rawCmd[i] = '\0';
    }

    if (!exec[0])
        *exec = rawCmd;
    if (!args[0])
        *args = strdup("");
}

int execute_foreground(cmdMap *evaluatedCmd, char *rawCmd)
{
    // printf("Foreground Process %s\n", rawCmd);

    int pid = fork();
    if (pid < 0)
    {
        print_error("Could not fork the terminal");
        return 1;
    }
    else if (pid == 0)
    {
        (evaluatedCmd->handler)(rawCmd);

        printf(ANSI_FG_COLOR_YELLOW);
        for (int i = 0; i < strlen(rawCmd); i++)
        {
            if (rawCmd[i] == ' ')
                break;
            printf("%c", rawCmd[i]);
        }
        print_error(" No such command found");
        exit(0);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
        return 0;
    }
}

int execute_background(cmdMap *evaluatedCmd, char *rawCmd)
{
    // printf("Background Process %s\n", rawCmd);

    int pid = fork();
    if (pid < 0)
    {
        print_error("Could not fork the terminal");
        return 1;
    }
    else if (pid == 0)
    {
        // wait(NULL);
        // FILE *outputFile = fopen("/home/amey/output", "w");
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

        (evaluatedCmd->handler)(rawCmd);

        printf(ANSI_FG_COLOR_YELLOW);
        for (int i = 0; i < strlen(rawCmd); i++)
        {
            if (rawCmd[i] == ' ')
                break;
            printf("%c", rawCmd[i]);
        }
        print_error(" No such command found");
        exit(0);
    }
    else
    {
        insert_process(pid, rawCmd);
        printf("[%d] %d\n", bgProcesses->size, pid);
        return 0;
    }
}

void run_single_command(char *command, int isBg)
{
    char *rawCmd = strdup(command);
    char *exec = NULL;
    for (int i = 0; i < strlen(rawCmd); i++)
    {
        if (rawCmd[i] != ' ')
            continue;
        exec = rawCmd;
        rawCmd[i] = '\0';
    }
    exec = exec ? exec : rawCmd;

    cmdMap *evaluatedCmd = NULL;
    for (int i = 0; i < sizeof(cmdTable) / sizeof(cmdMap); i++)
        if (!strcmp(exec, cmdTable[i].cmdName))
        {
            evaluatedCmd = cmdTable + i;
            break;
        }

    evaluatedCmd = evaluatedCmd ? evaluatedCmd : cmdTable;
    if (evaluatedCmd->cmdID == EXIT)
        evaluatedCmd->handler("");

    time_t srtTime = 0, endTime = 0;
    srtTime = time(NULL);

    if (evaluatedCmd->cmdID != UNKNOWN)
    {
        char *exec = NULL, *args = NULL;
        split_raw_command(strdup(command), &exec, &args);
        evaluatedCmd->handler(args);
    }
    else
    {
        if (isBg)
            execute_background(evaluatedCmd, command);
        else
            execute_foreground(evaluatedCmd, command);
        endTime = time(NULL);
        if (!isBg)
        {
            printf(ANSI_FG_COLOR_YELLOW "\n%s " ANSI_COLOR_RESET "ran in " ANSI_FG_COLOR_MAGENTA "%lds" ANSI_COLOR_RESET "\n\n", exec, endTime - srtTime);

            if (endTime - srtTime > 2)
            {
                int i = 0;
                char *dup = strdup(command);

                while (dup && dup[i] != ' ')
                    i++;

                dup[i] = '\0';

                strcpy(PREV_COMMAND, " " ANSI_FG_COLOR_YELLOW);
                strcat(PREV_COMMAND, dup);
                strcat(PREV_COMMAND, ANSI_COLOR_RESET ":" ANSI_FG_COLOR_MAGENTA);

                char time[100];
                sprintf(time, "%lds", endTime - srtTime);
                strcat(PREV_COMMAND, time);
                strcat(PREV_COMMAND, "" ANSI_COLOR_RESET);
            }
        }
    }
    return;
}

void execute_single_line_command(char *cmd)
{
    char *arg = strdup(cmd);
    arg = rstrip(arg, '\n');
    char *command = remove_whitespaces(arg);

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
        run_single_command(bgCmd, 0);
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

        // printf("%s", PREV_COMMAND_OUTPUT);
        // PREV_COMMAND_OUTPUT[0] = '\0';

        if (bytes)
            printf("\n");
        fclose(output);
    }
    output = fopen(PREV_COMMAND_OUTPUT, "w");
    // output = fopen("/home/amey/output", "w");
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
            printf("[%d] %d exited " ANSI_FG_COLOR_GREEN "successfully" ANSI_FG_COLOR_YELLOW "\t%s\n" ANSI_COLOR_RESET, bgProcesses->size - 1, pid, process->pName);
        else
            printf("[%d] %d exited " ANSI_FG_COLOR_RED "abnormally " ANSI_COLOR_RESET "with error: %d\t" ANSI_FG_COLOR_YELLOW "%s\n" ANSI_COLOR_RESET, bgProcesses->size - 1, pid, status, process->pName);
        remove_process_with_id(pid);
    }
}

void kill_terminal(int id)

{
    cmdMap *evaluatedCmd = NULL;
    for (int i = 0; i < sizeof(cmdTable) / sizeof(cmdMap); i++)
        if (!strcmp("exit", cmdTable[i].cmdName))
        {
            evaluatedCmd = cmdTable + i;
            break;
        }

    execute_foreground(evaluatedCmd, "exit");
    prompt();
}

#include "headers.h"

int warpHandler(const char *args)
{
    char *arguments = strdup(args);
    char *arg = strtok(arguments, " ");
    if (!arg)
        cd("~", 0);
    while (arg)
    {
        // printf("%s\n", arg);
        cd(arg, 0);
        arg = strtok(NULL, " ");
    }
}

int peekHandler(const char *args)
{
    char *arguments = strdup(args);
    char *arg = strtok(arguments, " ");
    int hidden = 0, detailed = 0;
    char prev[PATH_MAX];
    while (arg)
    {
        // printf("%s\n", arg);
        if (arg[0] == '-')
        {
            if (arg[1] == 'a')
                hidden = 1;
            else if (arg[1] == 'l')
                detailed = 1;

            if (strlen(arg) == 3)
                if (arg[2] == 'a')
                    hidden = 1;
                else if (arg[2] == 'l')
                    detailed = 1;
        }
        strcpy(prev, arg);
        arg = strtok(NULL, " ");
    }
    // printf("*%d*%d -%s\n", hidden, detailed, prev);
    ls(prev, hidden, detailed);
}

int exitHandler()
{
    printf("Exiting the terminal...\n");
    exit(EXIT_SUCCESS);
}

cmdMap cmdTable[] = {
    {"warp", WARP, warpHandler},
    {"peek", PEEK, peekHandler},
    {"pastevents", PASTEVENTS},
    {"proclore", PROCLORE},
    {"seek", SEEK},
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
// typedef struct CommandWord
// {
//     char data[1024];
//     struct CommandWord *next;
// } CommandWord;
// typedef struct CommandWords
// {
//     CommandWord *head;
//     CommandWord *tail;
// } CommandWords;
// char *genCommand(CommandWords words)
// {
//     char *command = (char *)calloc(ARG_MAX, sizeof(char));
//     CommandWord *start = words.head;
//     CommandWord *next;
//     while (start != NULL)
//     {
//         strcat(command, start->data);
//         strcat(command, " ");

//         next = start->next;
//         free(start);
//         start = next;
//     }
//     return rstrip(command, ' ');
// }
// void pushWord(CommandWords *words, char *word)
// {
//     CommandWord *temp = (CommandWord *)malloc(1 * sizeof(CommandWord));
//     strcpy(temp->data, word);
//     temp->next = NULL;

//     if (!words->head)
//     {
//         words->head = temp;
//         words->tail = temp;
//     }
//     else
//     {
//         words->tail->next = temp;
//         words->tail = temp;
//     }
// }

void execute_single_command(char *cmd);
void execute_multi_command(char *cmd)
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

    char delim[2] = ";";
    char *command = strtok(arg, delim);

    CommandsList commandList = {NULL, NULL};
    while (command)
    {
        enqueueList(&commandList, strip(strdup(command), ' '));
        command = strtok(NULL, delim);
    }

    CommandNode *head = commandList.head;
    CommandNode *next;
    while (head)
    {
        execute_single_command(head->data);
        next = head->next;
        free(head);
        head = next;
    }
}

void execute_single_command(char *cmd)
{
    char *arg = strdup(cmd);
    arg = rstrip(arg, '\n');
    char *command = remove_whitespaces(arg);
    // char delim[2] = " ";

    if (!strcmp(command, ""))
        return;

    // CommandWords words = {NULL, NULL};
    // char *word = strtok(arg, delim);
    // while (word)
    // {
    //     pushWord(&words, strip(strdup(word), ' '));
    //     word = strtok(NULL, delim);
    // }
    // char *command = genCommand(words);

    // printf("Command*%s*\n", command);

    char *exec = 0;
    char *args = 0;

    for (int i = 0; i < strlen(command); i++)
    {
        if (command[i] != ' ')
            continue;
        exec = command;
        args = command + i + 1;
        command[i] = '\0';
    }

    if (!exec && !args)
    {
        exec = command;
        args = strdup("");
    }

    cmdMap *evaluatedCmd = NULL;
    for (int i = 0; i < sizeof(cmdTable) / sizeof(cmdMap); i++)
    {
        if (!strcmp(exec, cmdTable[i].cmdName))
        {
            evaluatedCmd = cmdTable + i;
            break;
        }
    }

    // Remove
    // int evaluatedCmdID = evaluatedCmd ? evaluatedCmd->cmdID : UNKNOWN;
    // printf("cmd - %d\n", evaluatedCmdID);

    if (evaluatedCmd && evaluatedCmd->cmdID)
        (evaluatedCmd->handler)(args);
}
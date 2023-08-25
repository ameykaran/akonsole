#include "headers.h"

// Linkedlist stroing pid of processes

// typedef struct processNode
// {
//     pid_t pid;
//     char pName[256];
//     struct processNode *next;
// } processNode;

// typedef struct processList
// {
//     processNode *head;
//     processNode *tail;
//     int size;
// } processList;

void insert_process(int pid, char *cmd)
{
    processNode *newNode = (processNode *)malloc(sizeof(processNode));
    newNode->pid = pid;

    // int i = 0;
    // for (; i < strlen(cmd); i++)
    // {
    //     if (cmd[i] == ' ')
    //         break;
    // }
    // cmd[i] = '\0';

    strcpy(newNode->pName, cmd);
    newNode->next = NULL;

    if (bgProcesses->head == NULL)
    {
        bgProcesses->head = newNode;
        bgProcesses->tail = newNode;
        bgProcesses->size = bgProcesses->size == 0 ? 1 : bgProcesses->size + 1;
        return;
    }

    bgProcesses->tail->next = newNode;
    bgProcesses->tail = newNode;

    bgProcesses->size = bgProcesses->size == 0 ? 1 : bgProcesses->size + 1;
    return;
}

// processNode *getFront()
// {
//     if (bgProcesses->head == NULL)
//         return NULL;
//     return bgProcesses->head;
// }

processNode *get_process_with_id(int pid)
{
    processNode *temp = bgProcesses->head;
    while (temp)
    {
        if (temp->pid == pid)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

void remove_process_with_id(int pid)
{
    processNode *temp = bgProcesses->head;
    processNode *prev = NULL;
    while (temp)
    {
        if (temp->pid == pid)
        {
            if (prev == NULL)
            {
                bgProcesses->head = temp->next;
                free(temp);
                return;
            }
            prev->next = temp->next;
            free(temp);
            return;
        }
        prev = temp;
        temp = temp->next;
    }
    bgProcesses->size -= 1;
}

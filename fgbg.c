#include "headers.h"

void insert_process(int pid, char *cmd)
{
    processNode *newNode = (processNode *)malloc(sizeof(processNode));
    newNode->pid = pid;

    strcpy(newNode->pName, cmd);
    newNode->next = NULL;

    if (bgProcesses->head == NULL)
    {
        bgProcesses->head = newNode;
        bgProcesses->tail = newNode;
        bgProcesses->size += 1;
        return;
    }

    bgProcesses->tail->next = newNode;
    bgProcesses->tail = newNode;

    bgProcesses->size += 1;
    return;
}

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
                bgProcesses->size -= 1;
                return;
            }
            prev->next = temp->next;
            free(temp);
            bgProcesses->size -= 1;
            return;
        }
        prev = temp;
        temp = temp->next;
    }
}

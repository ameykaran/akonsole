#include "headers.h"

void insert_process(int pid, char *cmd, char isBg)
{
    processNode *newNode = (processNode *)calloc(1, sizeof(processNode));
    newNode->pid = pid;
    newNode->isBg = isBg;
    newNode->spawnTime = time(NULL);
    strcpy(newNode->pName, cmd);

    if (Processes->head == NULL)
    {
        Processes->head = newNode;
        Processes->tail = newNode;
        Processes->size += 1;
        return;
    }

    Processes->tail->next = newNode;
    Processes->tail = newNode;

    Processes->size += 1;
    return;
}

processNode *get_process_with_id(int pid)
{
    processNode *temp = Processes->head;
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
    processNode *temp = Processes->head;
    processNode *prev = NULL;
    while (temp)
    {
        if (temp->pid == pid)
        {
            if (prev == NULL)
            {
                Processes->head = temp->next;
                free(temp);
                Processes->size -= 1;
                return;
            }
            prev->next = temp->next;
            free(temp);
            Processes->size -= 1;
            return;
        }
        prev = temp;
        temp = temp->next;
    }
}

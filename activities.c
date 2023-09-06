#include "headers.h"

void activities()
{
    processNode *head = Processes->head;
    while (head)
    {
        printf("%ld : %s - ", head->pid, head->pName);
        char statusFilePath[PATH_MAX];
        FILE *statusFile;

        snprintf(statusFilePath, PATH_MAX, "/proc/%ld/status", head->pid);

        statusFile = fopen(statusFilePath, "r");
        if (!statusFile)
        {
            printf("Stopped\n");
            head = head->next;
            continue;
        }
        char buffer[1024];

        while (fgets(buffer, 1024, statusFile))
        {
            if (!strncmp(buffer, "State:", 6))
            {
                for (int i = 10; i < strlen(buffer); i++)
                {
                    if (i == 10)
                        printf("%c", buffer[i] - 'a' + 'A');
                    else if (buffer[i] == ')')
                        break;
                    else
                        printf("%c", buffer[i]);
                }
                printf("\n");
            }
        }
        fclose(statusFile);
        head = head->next;
    }
}
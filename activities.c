#include "headers.h"

void activities()
{
    processNode *head = Processes->head;
    while (head)
    {
        char statusFilePath[PATH_MAX];
        FILE *statusFile;

        snprintf(statusFilePath, PATH_MAX, "/proc/%ld/status", head->pid);

        statusFile = fopen(statusFilePath, "r");
        if (!statusFile)
        {
            remove_process_with_id(head->pid);
            head = head->next;
            continue;
        }

        char buffer[1024];
        printf("%ld : %s - ", head->pid, head->pName);

        while (fgets(buffer, 1024, statusFile))
        {
            if (!strncmp(buffer, "State:", 6))
            {
                int i = 7;
                while (buffer[i] != '(')
                    i++;
                i--;
                i--;

                if (buffer[i] == 'R' || buffer[i] == 'S')
                    printf("Running");

                else if (buffer[i] == 'T')
                    printf("Stopped");

                printf("\n");
            }
        }
        fclose(statusFile);
        head = head->next;
    }
}
#include "headers.h"

void proclore(long pid)
{
    char statusFilePath[PATH_MAX];
    FILE *statusFile;

    snprintf(statusFilePath, PATH_MAX, "/proc/%ld/status", pid);

    statusFile = fopen(statusFilePath, "r");
    if (!statusFile)
    {
        print_error("Error opening status file");
        return;
    }

    long tcpgid = tcgetpgrp(STDOUT_FILENO);

    procInfo info = {0};
    char buffer[1024];

    while (fgets(buffer, 1024, statusFile))
    {
        if (!strncmp(buffer, "Pid:", 4))
            sscanf(buffer + 5, "%ld", &info.pid);
        else if (!strncmp(buffer, "State:", 6))
            sscanf(buffer + 7, "%s", info.status);
        else if (!strncmp(buffer, "PPid:", 5))
            sscanf(buffer + 6, "%ld", &info.ppid);
        else if (!strncmp(buffer, "VmSize:", 7))
            sscanf(buffer + 8, "%ld", &info.vmsize);
    }

    char executablePath[1024];
    snprintf(executablePath, 1024, "/proc/%ld/exe", pid);
    realpath(executablePath, info.exec);

    info.fgbg = (TERMINAL_PID == tcpgid ? '+' : ' ');

    printf("PID: %ld\n", info.pid);
    printf("Process Status: %c", info.status[0]);
    printf("%c\n", info.fgbg);
    printf("Process group: %ld\n", info.ppid);
    printf("Virtual Memory: %ld\n", info.vmsize);
    printf("Executable: %s\n", get_rel_path(info.exec));

    fclose(statusFile);
}
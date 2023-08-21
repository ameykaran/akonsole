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
        exit(EXIT_FAILURE);
    }

    procInfo info;
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

    printf("Process id: %ld\n", info.pid);
    long pgid = getpgid(info.pid);
    printf("Process group id - %ld\n", pgid);
    printf("Terminal id - %ld\n", TERMINAL_PID);
    printf("Terminal group id - %ld\n", getpgid(TERMINAL_PID));

    printf("PID: %ld\n", info.pid);
    printf("Process Status: %c", info.status[0]);
    printf("%c\n", info.fgbg);
    printf("Process group: %ld\n", info.ppid);
    printf("Virtual Memory: %ld\n", info.vmsize);
    printf("Executable: %s\n", info.exec);

    fclose(statusFile);
}
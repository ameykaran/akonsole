#include "headers.h"

extern cmdMap cmdTable[];

void kill_children(int id)
{
    int status, pid;
    if ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        processNode *process = get_process_with_id(pid);
        if (!process)
        {
            print_error("Could not find process ");
            return;
        }

        if (process->isBg)
            if (status == 0)
                printf("[%d] %d exited " ANSI_FG_COLOR_GREEN "successfully " ANSI_FG_COLOR_YELLOW "- %s" ANSI_COLOR_RESET "\n", Processes->size - 1, pid, process->pName);
            else
                printf("[%d] %d exited " ANSI_FG_COLOR_RED "abnormally " ANSI_COLOR_RESET "with error: %d" ANSI_FG_COLOR_YELLOW " - %s" ANSI_COLOR_RESET "\n", Processes->size - 1, pid, status, process->pName);
        // remove_process_with_id(pid);
    }
}

void kill_terminal(int id)

{
    printf("%d\t%d\n", id, TERMINAL_PID);
    printf("\nDo you really want to quit? [y/n] ");
    char c = getchar();
    printf("*%d*%s*\n", c, &c);

    if (c == 'y' || c == 'Y')
        (cmdTable + 1)->handler(0, NULL);
    else
        signal(SIGINT, kill_terminal);
    getchar();
}

void kill_fg_process(int id)
{
    if (!Processes->head)
        return;
    processNode *lastFg = Processes->tail;
    while (lastFg && lastFg->isBg)
        lastFg = lastFg->prev;
    if (lastFg)
    {
        printf("Recent fg- %d\n", lastFg->pid);
        kill(lastFg->pid, SIGINT);
    }
}

void set_signal_handlers()
{
    struct sigaction bgProcessFinishSig = {0};

    bgProcessFinishSig.sa_handler = kill_children;
    sigemptyset(&bgProcessFinishSig.sa_mask);
    bgProcessFinishSig.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &bgProcessFinishSig, NULL);

    struct sigaction exitSig = {0};
    // exitSig.sa_handler = kill_terminal;
    sigemptyset(&exitSig.sa_mask);
    exitSig.sa_flags = SA_RESTART;
    exitSig.sa_handler = kill_fg_process;

    sigaction(SIGINT, &exitSig, NULL);
    // pause();

    // exitSig.sa_sigaction = get_pid;

    // printf("PID - %d\n", signalPid);
}
#include "headers.h"

extern cmdMap cmdTable[];

void ping(int pid, int sig)
{
    if (sig < 0)
    {
        print_error("Invalid signal");
        return;
    }
    sig = sig % 32;
    if (!kill(pid, sig))
        printf(ANSI_FG_COLOR_GREEN "Sent signal %d to pid %d\n" ANSI_COLOR_RESET, sig, pid);
    else
        printf(ANSI_FG_COLOR_RED "Could not send signal %d to pid %d\n" ANSI_COLOR_RESET, sig, pid);
}

void kill_children(int id)
{
    int status, pid;
    if ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        processNode *process = get_process_with_id(pid);
        if (!process)
            return;

        if (process->isBg)
        {
            if (status == 0)
                printf("[%d] %d exited " ANSI_FG_COLOR_GREEN "successfully " ANSI_FG_COLOR_YELLOW "- %s" ANSI_COLOR_RESET "\n", Processes->size - 1, pid, process->pName);
            else
                printf("[%d] %d exited " ANSI_FG_COLOR_RED "abnormally " ANSI_COLOR_RESET "with error: %d" ANSI_FG_COLOR_YELLOW " - %s" ANSI_COLOR_RESET "\n", Processes->size - 1, pid, status, process->pName);
        }
        remove_process_with_id(pid);
    }
}

processNode *get_last_fg_process()
{
    if (!Processes->head)
        return NULL;
    processNode *lastFg = Processes->tail;
    while (lastFg && lastFg->isBg)
        lastFg = lastFg->prev;

    return lastFg;
}

void kill_fg_process(int id)
{
    processNode *lastFg = get_last_fg_process();
    if (lastFg)
        kill(lastFg->pid, SIGINT);
}

void set_signal_handlers()
{
    struct sigaction bgProcessFinishSig = {0};

    bgProcessFinishSig.sa_handler = kill_children;
    sigemptyset(&bgProcessFinishSig.sa_mask);
    bgProcessFinishSig.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &bgProcessFinishSig, NULL);

    struct sigaction ctrlC_Sig = {0};
    sigemptyset(&ctrlC_Sig.sa_mask);
    ctrlC_Sig.sa_flags = SA_RESTART;
    ctrlC_Sig.sa_handler = kill_fg_process;
    sigaction(SIGINT, &ctrlC_Sig, NULL);

    struct sigaction ctrlZ_Sig = {0};
    sigemptyset(&ctrlZ_Sig.sa_mask);
    ctrlZ_Sig.sa_flags = SA_RESTART;
    ctrlZ_Sig.sa_handler = ctrl_z_handler;
    sigaction(SIGTSTP, &ctrlZ_Sig, NULL);
}

void ctrl_d_handler(char *buffer)
{
    processNode *head = Processes->head;
    while (head)
    {
        kill(head->pid, SIGTERM);
        perror("Kill");
        head = head->next;
    }

    kill_children(0);
    (cmdTable + 1)->handler(0, NULL);
}

void ctrl_z_handler(int id)
{
    if (id == SIGTSTP)
    {
        processNode *lastFg = get_last_fg_process();
        if (!lastFg)
        {
            print_error("No foreground process running");
            return;
        }
        lastFg->isRunning = 0;
        kill(lastFg->pid, SIGTSTP);
    }
}

void fg(int pid)
{
    int status;
    kill(pid, SIGCONT);
    processNode *proc = get_process_with_id(pid);
    if (!proc)
        return;
    proc->isRunning = 1;
    waitpid(pid, &status, WUNTRACED);
    return;
}

void bg(int pid)
{
    kill(pid, SIGCONT);
    processNode *proc = get_process_with_id(pid);
    if (!proc)
        return;
    proc->isRunning = 1;
    // waitpid(pid, &status, WUNTRACED);
    return;
}
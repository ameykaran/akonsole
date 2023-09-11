#ifndef SIGNAL_H_
#define SIGNAL_H_

// void check_bg_status();
void set_signal_handlers();
void kill_children(int id);
void kill_fg_process(int id);

void ctrl_d_handler(char *buffer);
void ping(int pid, int sig);

#endif
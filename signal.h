#ifndef SIGNAL_H_
#define SIGNAL_H_

// void check_bg_status();
void set_signal_handlers();
void kill_children(int id);
void kill_terminal(int id);
void kill_fg_process(int id);

#endif
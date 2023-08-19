
#ifndef UTILITY_H_
#define UTILITY_H_

void rstrip(char *text, char letter);
void print_error(char *error);
char *get_abs_path(char *path, int expandDots);
char *get_rel_path(char *path);

/*
Prints the given num as a digits-digit number
*/
void print_aligned(long num, int digits);

#endif
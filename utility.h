
#ifndef UTILITY_H_
#define UTILITY_H_

char *lstrip(char *str, char letter);
char *rstrip(char *str, char letter);
char *strip(char *str, char letter);
void print_error(char *error);
char *get_abs_path(char *path, int expandDots);
char *get_rel_path(char *path);
char *remove_whitespaces(char *text);

/*
Prints the given num as a digits-digit number
*/
void print_aligned(long num, int digits);

#endif
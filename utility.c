#include "headers.h"

void rstrip(char *str, char letter)
{
    int len = strlen(str);
    while (str[len - 1] == letter)
        str[--len] = '\0';
}
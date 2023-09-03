#ifndef IMAN_H_
#define IMAN_H_

#include <netdb.h>
#include <arpa/inet.h>

int get_man_page(const char *command);

#define REQUEST_MAX_SIZE 1024
#define RESPONSE_MAX_SIZE 1024
#define BUFFER_SMALL 512
#define BUFFER_MEDIUM 1024
#define BUFFER_LARGE 2048

#endif
#include "headers.h"

#define NETWORK_ERROR                                                               \
    if (h_errno == HOST_NOT_FOUND)                                                  \
        print_error("The specified host is unknown");                               \
    else if (h_errno == NO_ADDRESS || h_errno == NO_DATA)                           \
        print_error("The requested name is valid but does not have an IP address"); \
    else if (h_errno == NO_RECOVERY)                                                \
        print_error("A nonrecoverable name server error occurred");                 \
    else                                                                            \
        print_error("A temporary error occurred on an authoritative name server. Try again later");

#define BASE_LINK "man.he.net"

typedef struct htmlEscapeCodes
{
    char code[10], symbol[10];
} htmlEscapeCodes;

// Source https://mateam.net/html-escape-characters/
htmlEscapeCodes escapeTable[] = {
    {"&Tab;", "\t"},
    {"&#9;", "\t"},
    {"&NewLine;", "\n"},
    {"&#10;", "\n"},
    {"&nbsp;", " "},
    {"&quot;", "\""},
    {"&amp;", "&"},
    {"&lt;", "<"},
    {"&gt;", ">"},
    {"&gt;", ">"},
};

int resolve_host(const char *host, char *ipAddr)
{
    struct hostent *hostEntity;
    struct in_addr **addrList;

    printf("Resolving DNS...\n");

    if (!(hostEntity = gethostbyname(host)))
    {
        NETWORK_ERROR
        return 1;
    }

    addrList = (struct in_addr **)hostEntity->h_addr_list;

    if (!addrList[0])
    {
        NETWORK_ERROR
        return 1;
    }

    strcpy(ipAddr, inet_ntoa(*addrList[0]));
    printf("IP Address: %s\n", ipAddr);
    return 0;
}

void process_raw(char *text, char *processed)
{
#define STRCAT_TO_PROCESSED(str)    \
    processed[processedInd] = '\0'; \
    strcat(processed, str);         \
    processedInd = strlen(processed);

    int textInd = 0, processedInd = 0;

    while (text[textInd] != '\n')
    {
        for (int i = 0; i < sizeof(escapeTable) / sizeof(htmlEscapeCodes); i++)
        {
            if (!strncmp(text + textInd, escapeTable[i].code, strlen(escapeTable[i].code)))
            {
                textInd += strlen(escapeTable[i].code);
                STRCAT_TO_PROCESSED(escapeTable[i].symbol);
                continue;
            }
        }

        if (!strncmp(text + textInd, "<STRONG>", 8))
        {
            textInd += 8;
            STRCAT_TO_PROCESSED("\x1b[1m");
            continue;
        }
        if (!strncmp(text + textInd, "</STRONG>", 9))
        {
            textInd += 9;
            STRCAT_TO_PROCESSED("\x1b[0m");
            continue;
        }

        if (!strncmp(text + textInd, "<A HREF=\"", 9))
        {
            textInd += 9;
            char prefixURL[] = "\x1b]8;;" BASE_LINK;
            STRCAT_TO_PROCESSED("\033[4m");
            STRCAT_TO_PROCESSED(prefixURL);
            while (text[textInd] != '"')
            {
                processed[processedInd] = text[textInd];
                processedInd++;
                textInd++;
            }
            while (text[textInd] != '>')
                textInd++;
            textInd++;
            STRCAT_TO_PROCESSED("\x1b\\");
            continue;
        }
        if (!strncmp(text + textInd, "</A>", 4))
        {
            textInd += 4;
            STRCAT_TO_PROCESSED("\x1b]8;;\x1b\\");
            STRCAT_TO_PROCESSED("\033[0m");
            continue;
        }

        else
        {
            processed[processedInd] = text[textInd];
            processedInd++;
            textInd++;
        }
    }
}

int get_man_page(const char *command)
{
    char ipAddress[20];
    if (resolve_host(BASE_LINK, ipAddress))
    {
        print_error("Error resolving host");
        return 1;
    }

    struct sockaddr_in server = {0};
    char request[REQUEST_MAX_SIZE];
    char response[RESPONSE_MAX_SIZE];

    int socketFileDesc = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFileDesc < 0)
    {
        print_error("Error opening socket");
        return 1;
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(80);
    server.sin_addr.s_addr = inet_addr(ipAddress);

    if (connect(socketFileDesc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Connection to server: ");
        return 1;
    }

    sprintf(request, "GET /?topic=%s&section=all HTTP/1.1\r\nHost: " BASE_LINK "\r\n\r\n", command);
    if (write(socketFileDesc, request, strlen(request)) < 0)
    {
        print_error("Error sending request");
        return 1;
    }

    printf("Getting man page for \033[1m%s\033[0m\n", command);

    FILE *fd = tmpfile();
    while (1)
    {
        int bytesRead = read(socketFileDesc, response, 1000);
        if (bytesRead < 0)
        {
            print_error("Error receiving response");
            return 1;
        }
        else if (bytesRead == 0)
            break;
        else
            fwrite(response, 1, bytesRead, fd);
    }
    close(socketFileDesc);

    char readStr[BUFFER_MEDIUM];
    int totalLines = 0;
    fseek(fd, 0, SEEK_SET);
    while (fgets(readStr, BUFFER_MEDIUM, fd))
        totalLines++;

    fseek(fd, 0, SEEK_SET);
    while (fgets(readStr, BUFFER_MEDIUM, fd))
    {
        for (int i = 0; readStr[i] != '\n'; i++)
            if (!strncmp(readStr + i, "No matches for", 14))
            {
                print_error("No manual entry for the specified command");
                return 1;
            }
    }

    int pos = 0, lines = 0;
    fseek(fd, 0, SEEK_SET);
    // printf("\033c");

    while (fgets(readStr, BUFFER_MEDIUM, fd))
    {
        lines++;
        if (lines < 17)
            continue;

        else if (lines == totalLines - 7)
            break;

        else
        {
            char processed[BUFFER_LARGE] = {0};
            process_raw(readStr, processed);
            if (lines == 17)
            {
                int i;
                for (i = 0; processed[i] != ' '; i++)
                    ;
                processed[i] = '\0';
            }
            printf("%s\n", processed);
        }
    }

    fclose(fd);

    return 0;
}
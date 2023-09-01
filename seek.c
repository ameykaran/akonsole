#include "headers.h"

void seek_recursive(char *basePath, char *name, int flags, int *count, char *rootPath, char *execPath, char *execType)
{
    struct dirent *direntry;

    DIR *dir = opendir(basePath);

    if (!dir)
    {
        printf("%s:", trim(basePath, rootPath));
        if (errno == ENOENT)
            print_error("Directory doesn't exist");
        else if (errno == ENOTDIR)
            print_error("The given path is not a directory");
        else
            print_error("Error listing the directory!");
        return;
    }

    struct stat info;
    while (direntry = readdir(dir))
    {
        if (strcmp(direntry->d_name, ".") != 0 && strcmp(direntry->d_name, "..") != 0)
        {
            if (direntry->d_name[0] == '.' && !(flags & SEEK_FLAG_HIDDEN))
                continue;

            char currPath[PATH_MAX];
            strcpy(currPath, basePath);
            strcat(currPath, "/");
            strcat(currPath, direntry->d_name);
            stat(direntry->d_name, &info);

            if (!strncmp(direntry->d_name, name, strlen(name)))
            {
                char *relPath = trim(currPath, rootPath);
                relPath = relPath - 2;
                relPath[0] = '.';
                relPath[1] = '/';

                if (flags & SEEK_FILE && direntry->d_type == DT_REG)
                {
                    printf(ANSI_FG_COLOR_GREEN "%s\n" ANSI_COLOR_RESET, relPath);
                    if (*count == 0)
                    {
                        strcpy(execPath, currPath);
                        execType[0] = SEEK_FILE;
                    }
                    *count += 1;
                }
                else if (flags & SEEK_DIR && direntry->d_type == DT_DIR)
                {
                    printf(ANSI_FG_COLOR_BLUE "%s\n" ANSI_COLOR_RESET, relPath);
                    if (*count == 0)
                    {
                        strcpy(execPath, currPath);
                        execType[0] = SEEK_DIR;
                    }
                    *count += 1;
                }
                if (flags & SEEK_LINK && direntry->d_type == DT_LNK)
                {
                    printf(ANSI_FG_COLOR_CYAN "%s\n" ANSI_COLOR_RESET, relPath);
                    if (*count == 0)
                    {
                        strcpy(execPath, currPath);
                        execType[0] = SEEK_LINK;
                    }
                    *count += 1;
                }
            }

            if (direntry->d_type == DT_DIR)
                seek_recursive(currPath, name, flags, count, rootPath, execPath, execType);
        }
    }

    closedir(dir);
}

void print_file(char *path)
{
    if (access(path, R_OK))
    {
        print_error("Permission denied to read the file");
        return;
    }
    FILE *file = fopen(path, "r");
    if (!file)
    {
        if (errno == ENOENT)
            print_error("File doesn't exist");
        else if (errno == ENOTDIR)
            print_error("The given path is not a file");
        else
            print_error("Error opening the file!");
        return;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, file)) != -1)
        printf("%s", line);
}

void change_directory(char *path, int isLink)
{
    if (access(path, X_OK))
    {
        print_error("Permission denied to open the directory");
        return;
    }
    cd(path, 0);
}

void follow_link(char *path)
{
    if (access(path, X_OK))
    {
        print_error("Permission denied to follow the link");
        return;
    }

    struct stat realisedStat;
    stat(path, &realisedStat);

    if (S_ISDIR(realisedStat.st_mode))
        cd(path, 0);
    else
        print_file(path);
}

void seek(char *path, int flags, char *name)
{
    int count = 0;
    char execPath[PATH_MAX], execType = 0;

    seek_recursive(path, name, flags, &count, path, execPath, &execType);

    if (count == 1 && flags & SEEK_FLAG_EXEC)
    {
        if (execType == SEEK_DIR)
            change_directory(execPath, 0);
        else if (execType == SEEK_FILE)
            print_file(execPath);
        else if (execType == SEEK_LINK)
            change_directory(execPath, 1);
    }
    else if (count == 0)
        printf("No match found!\n");

    free(path);
    free(name);
}

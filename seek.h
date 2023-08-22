#ifndef SEEK_H_
#define SEEK_H_

typedef enum seekFlag seekFlag;

#define SEEK_DIR 1
#define SEEK_FILE 2
#define SEEK_LINK 4
#define SEEK_FLAG_HIDDEN 8
#define SEEK_FLAG_EXEC 16
#define SEEK_FLAG_NAME 32
#define SEEK_FLAG_PATH 64

#define SEEK_IS_DIR(flag) (flag & SEEK_DIR)
#define SEEK_IS_FILE(flag) (flag & SEEK_FILE)
#define SEEK_IS_LINK(flag) (flag & SEEK_LINK)

void seek_recursive(char *basePath, char *name, int flags, int *count, char *rootPath, char *execPath, char *execType);
void seek(char *path, int flags, char *name);

// void find(char *name, char *path);

#endif
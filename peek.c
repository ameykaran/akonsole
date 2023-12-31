#include "headers.h"

void get_info(struct dirent *direntry, struct stat stat, directEntry *entries, int ind, int *maxNLinks, int *maxSize)
{
    if (S_ISLNK(stat.st_mode))
        entries[ind].perms[0] = 'l';
    else if (S_ISDIR(stat.st_mode))
        entries[ind].perms[0] = 'd';
    else
        entries[ind].perms[0] = '-';

    entries[ind].perms[1] = stat.st_mode & S_IRUSR ? 'r' : '-';
    entries[ind].perms[2] = stat.st_mode & S_IWUSR ? 'w' : '-';
    entries[ind].perms[3] = stat.st_mode & S_IXUSR ? 'x' : '-';
    entries[ind].perms[4] = stat.st_mode & S_IRGRP ? 'r' : '-';
    entries[ind].perms[5] = stat.st_mode & S_IWGRP ? 'w' : '-';
    entries[ind].perms[6] = stat.st_mode & S_IXGRP ? 'x' : '-';
    entries[ind].perms[7] = stat.st_mode & S_IROTH ? 'r' : '-';
    entries[ind].perms[8] = stat.st_mode & S_IWOTH ? 'w' : '-';
    entries[ind].perms[9] = stat.st_mode & S_IXOTH ? 'x' : '-';

    entries[ind].numLinks = stat.st_nlink;

    struct passwd *user = getpwuid(stat.st_uid);
    strcpy(entries[ind].user, user ? user->pw_name : "null");

    struct group *group = getgrgid(stat.st_gid);
    strcpy(entries[ind].group, group ? group->gr_name : "null");

    entries[ind].size = stat.st_size;
    entries[ind].timeCreated = stat.st_mtime;

    strcpy(entries[ind].name, direntry->d_name);

    *maxNLinks = max(*maxNLinks, stat.st_nlink);
    *maxSize = max(*maxSize, stat.st_size);
}

int comparator(const void *ent1, const void *ent2)
{
    directEntry *a = (directEntry *)ent1;
    directEntry *b = (directEntry *)ent2;

    int res = strcmp(a->name, b->name);
    if (!res)
        return a->timeCreated - b->timeCreated;
    return res;
}

void print_entries(directEntry *entries, int count)
{
    for (int i = 0; i < count; i++)
    {
        directEntry entry = entries[i];

        if (entry.perms[0] == 'l')
            printf(ANSI_FG_COLOR_CYAN);
        else if (entry.perms[0] == 'd')
            printf(ANSI_FG_COLOR_BLUE);
        else if (entry.perms[3] == 'x' | entry.perms[6] == 'x' | entry.perms[9] == 'x')
            printf(ANSI_FG_COLOR_GREEN);

        printf(entry.name);
        printf(ANSI_COLOR_RESET "\n");
    }
}

void print_detailed(directEntry *entries, int count, int maxNLinks, int maxSize)
{
    int digNlinks = 0;
    while (maxNLinks)
    {
        digNlinks += 1;
        maxNLinks /= 10;
    }
    int digSize = 0;
    while (maxSize)
    {
        digSize += 1;
        maxSize /= 10;
    }

    printf("total %d\n", count);
    for (int i = 0; i < count; i++)
    {
        directEntry entry = entries[i];
        printf("%s ", entry.perms);
        print_aligned(entry.numLinks, digNlinks);
        printf("%s ", entry.user);
        printf("%s ", entry.group);

        print_aligned(entry.size, digSize);

        char *time = ctime(&entry.timeCreated);
        time = time + 4;
        time[12] = '\0';
        printf("%s ", time);

        if (entry.perms[0] == 'l')
        {
            printf(ANSI_FG_COLOR_CYAN);
            char absPath[PATH_MAX];

            realpath(entry.name, absPath);
            printf("%s" ANSI_COLOR_RESET " -> ", entry.name);

            struct stat symStat;
            lstat(absPath, &symStat);

            if (S_ISLNK(symStat.st_mode))
                printf(ANSI_FG_COLOR_CYAN);
            else if (S_ISDIR(symStat.st_mode))
                printf(ANSI_FG_COLOR_BLUE);
            else if (symStat.st_mode & S_IXUSR | symStat.st_mode & S_IXGRP | symStat.st_mode & S_IXOTH)
                printf(ANSI_FG_COLOR_GREEN);
            printf("%s", get_rel_path(absPath));
            printf(ANSI_COLOR_RESET);
        }
        else if (entry.perms[0] == 'd')
            printf(ANSI_FG_COLOR_BLUE);
        else if (entry.perms[3] == 'x' | entry.perms[6] == 'x' | entry.perms[9] == 'x')
            printf(ANSI_FG_COLOR_GREEN);

        !(entry.perms[0] == 'l') && printf("%s", entry.name);
        printf(ANSI_COLOR_RESET "\n");
    }
}

void ls(char *arg, int isHidden, int isFullInfo)
{
    char *path = strdup(arg);
    path = strip(path, ' ');
    path = get_abs_path(path);

    cd(path, 1);

    DIR *dir = opendir(path);
    if (!dir)
    {
        if (errno == ENOENT)
            print_error("Directory doesn't exist");
        else if (errno == ENOTDIR)
            print_error("The given path is not a directory");
        else
            print_error("Error listing the directory!");
        return;
    }

    int maxEntries = DIR_MAX_ENTRIES;

    directEntry *entries = (directEntry *)calloc(maxEntries, sizeof(directEntry));
    if (!entries)
    {
        print_error("Cannot allocate memory to entries");
        return;
    }

    int count = 0;
    struct dirent *direntry;
    errno = 0;
    int maxNLinks = 0, maxSize = 0;

    while (direntry = readdir(dir))
    {
        if (errno)
        {
            print_error("An error occurred");
            return;
        }

        char *entryName = direntry->d_name;
        if (!isHidden && entryName[0] == '.')
            continue;

        struct stat stat;
        lstat(entryName, &stat);

        get_info(direntry, stat, entries, count, &maxNLinks, &maxSize);
        count++;

        if (count >= maxEntries)
        {
            maxEntries *= 2;
            entries = realloc(entries, maxEntries * sizeof(directEntry));
        }
    }
    qsort(entries, count, sizeof(directEntry), comparator);

    if (isFullInfo)
        print_detailed(entries, count, maxNLinks, maxSize);
    else
        print_entries(entries, count);

    cd(PREV_DIR, 1);

    free(path);
    free(entries);
}
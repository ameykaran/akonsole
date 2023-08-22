#include "headers.h"

char HOME_DIR[PATH_MAX] = {}, CURR_DIR[PATH_MAX] = {}, PREV_DIR[PATH_MAX] = {};
long TERMINAL_PID;

// char *name = "naem";
// static int
// display_info(const char *fpath, const struct stat *sb,
//              int tflag, struct FTW *ftwbuf)
// {
//     if (!strcmp(name, fpath + ftwbuf->base))
//         printf("%-3s %2d %7jd   %-40s %d %s\n",
//                (tflag == FTW_D) ? "d" : (tflag == FTW_DNR) ? "dnr"
//                                     : (tflag == FTW_DP)    ? "dp"
//                                     : (tflag == FTW_F)     ? "f"
//                                     : (tflag == FTW_NS)    ? "ns"
//                                     : (tflag == FTW_SL)    ? "sl"
//                                     : (tflag == FTW_SLN)   ? "sln"
//                                                            : "???",
//                ftwbuf->level, sb->st_size,
//                fpath, ftwbuf->base, fpath + ftwbuf->base);
//     else
//         printf(" ");
//     return 0; /* To tell nftw() to continue */
// }

int main()
{
    printf("\033c");

    printLogo();

    if (!getcwd(HOME_DIR, PATH_MAX))
    {
        printf("Unable to determine the home directory!");
        exit(1);
    }

    strcpy(CURR_DIR, HOME_DIR);
    TERMINAL_PID = getpid();

    char buffer[ARG_MAX];
    while (1)
    {
        prompt();
        fgets(buffer, ARG_MAX, stdin);
        execute_multi_command(buffer);
    }
}
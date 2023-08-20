#include "headers.h"

char HOME_DIR[PATH_MAX] = {}, CURR_DIR[PATH_MAX] = {}, PREV_DIR[PATH_MAX] = {};
long TERMINAL_PID;

int main()
{
    system("clear");

    if (!getcwd(HOME_DIR, PATH_MAX))
    {
        printf("Unable to determine the home directory!");
        exit(1);
    }

    strcpy(CURR_DIR, HOME_DIR);
    TERMINAL_PID = getpid();
    // printf("%d\n", TERMINAL_PID);

    char buffer[ARG_MAX];
    // sprintf(buffer, "%ld", TERMINAL_PID);

    // prompt();
    // cd("academs/test");
    // prompt();
    // cd("..");
    // prompt();
    // cd("..");
    // prompt();
    // cd("..");
    // prompt();
    // cd("..");
    // prompt();
    // cd("..");
    // prompt();

    // cd("-");
    // cd("-");
    // prompt();
    // ls(".", 0, 1);
    // // find("hello", "..");
    // // find("hello", ".");
    // // proclore(buffer);
    // // proclore("396429");
    // // while (1)
    // ;

    // char *str = strdup("ge gls;pastevents execute 1; hello");
    // printf("*%s*\n", str);
    // str = removeWhitespaces(str);
    // printf("*%s*\n", str);
    // replace_pastevents_execute(str);

    // str = strdup("ge gls;pastevents execute 10; hello");
    // replace_pastevents_execute(str);

    // str = strdup("ge gls;pastevents purge; hello");
    // replace_pastevents_execute(str);

    // str = strdup("ge gls;pastevents execute ad; hello");
    // replace_pastevents_execute(str);

    // char *str = strdup("ge gls;pastevents execute ; hello 10");
    // replace_pastevents_execute(str);

    // str = strdup("avvv        567                  ");
    // printf("*%s*\n", str);
    // str = removeWhitespaces(str);
    // printf("*%s*\n", str);

    while (1)
    {
        prompt();
        fgets(buffer, ARG_MAX, stdin);
        execute_multi_command(buffer);
        // save_command(buffer);
        printf("\n");
    }

    // execute_multi_command("peek -l /home/amey/academics/SCI1 ");
    // execute_multi_command("warp  ");
    // execute_multi_command("warp /home/amey/academics/SCI1 ");
    // execute_multi_command("peek -l /home/amey/Downloads");
    // execute_multi_command("peek -al   /home/amey/academics/SCI1          ");
    // execute_multi_command("peek -a ..");
    // execute_multi_command("peek -l ..");
    // execute_multi_command("peek -a -l ..");
    // execute_multi_command("peek -l -a ..");
    // execute_multi_command("peek -la ..");
    // execute_multi_command("peek -al ..");
}
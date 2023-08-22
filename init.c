#include "headers.h"

#include <sys/utsname.h>
#include <sys/sysinfo.h>

char *print_uptime(long timeElapsed)
{
    long hours = timeElapsed / 3600;
    long minutes = (timeElapsed % 3600) / 60;
    long seconds = ((timeElapsed % 3600) % 60) / 60;

    char *timeText = (char *)calloc(1024, sizeof(char));
    char buffer[100];
    if (hours)
    {
        sprintf(buffer, "%ld hour", hours);
        strcpy(timeText, buffer);
    }
    if (hours != 1)
        strcat(timeText, "s");
    if (minutes)
    {
        sprintf(buffer, " %ld min", minutes);
        strcat(timeText, buffer);
    }
    if (minutes != 1)
        strcat(timeText, "s");
    if (seconds)
    {
        sprintf(buffer, " %ld sec", seconds);
        strcat(timeText, buffer);
    }
    if (seconds != 0 && seconds != 1)
        strcat(timeText, "s");

    return timeText;
}
void printLogo()
{
    struct utsname utsInfo;
    struct sysinfo sysInfo;
    uname(&utsInfo);
    sysinfo(&sysInfo);

    char *timeText = print_uptime(sysInfo.uptime);
    char *logoText1 = ANSI_FG_COLOR_BLUE
        "\n"
        "          .??.    ~J?J?.      \n"
        "         :G@@^  :5@@@B~       \n"
        "        :B@@&E\\.7#@@&?        Welcome to AKonsole\n" ANSI_FG_COLOR_BLUE
        "       ^#@@; ^P@@@5:            " ANSI_COLOR_RESET " new age terminal!\n" ANSI_FG_COLOR_BLUE
        "      ^#@@:   @@@#^           OS:     " ANSI_COLOR_RESET;

    printf("%s", logoText1);
    printf("%s %s\n", utsInfo.nodename, utsInfo.sysname);

    char *logoText2 =
        ANSI_FG_COLOR_BLUE
        "     ~&@@@5??^5@@@@Y:         Kernel: " ANSI_COLOR_RESET;
    printf("%s", logoText2);
    printf("%s\n", utsInfo.release);

    char *logoText3 = ANSI_FG_COLOR_BLUE
        "    !&@@@@^~*^*~G@@@&?.       Uptime: " ANSI_COLOR_RESET;

    printf("%s", logoText3);
    printf("%s\n", timeText);

    char *logoText4 = ANSI_FG_COLOR_BLUE
        "   7@@@G!        7#@@@#!.     \n"
        "  :Y55Y3          .J5555~     \n"
        "\n" ANSI_COLOR_RESET;

    printf(logoText4);

    // char *logoText1 = ANSI_FG_COLOR_BLUE
    //     "+------------------------------+\n"
    //     "|                              |\n"
    //     "|          .??.    ~J?J?.      |\n"
    //     "|         :G@@^  :5@@@B~       |\n"
    //     "|        :B@@&: 7#@@&?         |   Welcome to AKonsole!\n" ANSI_FG_COLOR_BLUE
    //     "|       ^#@@; ^P@@@5:          |     " ANSI_COLOR_RESET " new age terminal\n" ANSI_FG_COLOR_BLUE
    //     "|      ^#@@:   @@@#^           |   OS: " ANSI_COLOR_RESET;

    // printf("%s", logoText1);
    // printf("%s %s\n", utsInfo.nodename, utsInfo.sysname);
    // // "%s %s\n"
    // char *logoText2 =
    //     ANSI_FG_COLOR_BLUE
    //     "|     ~&@@@5??^5@@@@Y:         |   Kernel: " ANSI_COLOR_RESET;
    // printf("%s", logoText2);
    // printf("%s\n", utsInfo.release);
    // // "%s\n"

    // char *logoText3 = ANSI_FG_COLOR_BLUE
    //     "|    !&@@@@^~   ~G@@@&?.       |   Uptime: " ANSI_COLOR_RESET;

    // printf("%s", logoText3);
    // printf("%s\n", timeText);
    // // "%s\n"
    // char *logoText4 = ANSI_FG_COLOR_BLUE
    //     "|   7@@@G!        7#@@@#!.     |\n"
    //     "|  :Y55Y3          .J5555~     |\n"
    //     "|                              |\n"
    //     "+------------------------------+\n" ANSI_COLOR_RESET;
    // printf(logoText4);

    // printf(ANSI_FG_COLOR_BLUE "%s\n" ANSI_COLOR_RESET, logoArt, timeText);

    // char *text = ".:.:.:.:.:.:.:.:.:.t88X;.:.:.::.888888825.:.:..:.:"
    //              ".:.:.:.:..:.:.:.:: 8 ..Q ;..:: XX     R:..:.:.:.:."
    //              ".:.:..:.:.:..:.:: :    C::.:..8      Xt.:.:.:.:.:"
    //              ".:.:.:.:.:.:.:.: .:    D ;:.t ..    8S.::.:.:.:.:."
    //              ".:..:.:.:.:.:.: .;     G ;.t8..    8;.:..:.:.:.:.:"
    //              ".:.:.:.:.:.:.:; ;      8:,:l      3 ::.:.:.:..:.:."
    //              ".:.:.:..:..:.: ;      8..3       S.::.:.:.:.:.:..:"
    //              ".:.:..:.:.:.: t       8X..      S.::.:.:.:.:.:.:.:"
    //              ".:.:.:.:.:.; X        .5 ..    8 :.:..:.:.:.:.:.:."
    //              ".:.:.:.:.:: :D;:...            t:.:.:.:..:..:..:.:"
    //              ".:.:.:.:.: 88:....:;..:6S8       XA.::.:.:.:.:.:.."
    //              ".:..:.:.: 8;:.     .081:..2       .S ::.:.:.:.:.:."
    //              ".:.:.:.:.A .      7 ;:..;:8:        S.:.:.:..:.:.:"
    //              ".:.:..:.A        3..8: .;: 8t       .8X.:.:.:.:.:."
    //              ".:.:.:.A        4.: .:; .:;;:S.       t.::.:.:.:.:"
    //              ".:.:.;S        1;.:::.:..:..:: 8S      .8t.:.:..:."
    //              ".:.:: ;.........:.:.:.::.:.:..:.8; ......8:.:.:.:."
    //              ".:..::XSSSS%@..:.:.:.:..:.:.:.:.:;@QSSSSSRB;.:.:.:";
    // printf("%s\n", text);
}

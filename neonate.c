#include "headers.h"

void die(const char *s)
{
    perror(s);
    exit(1);
}

struct termios orig_termios;

void disable_raw_mode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enable_raw_mode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

int sleep_millis(unsigned int n)
{
    return usleep(n * 1000);
}

int print_latest_pid()
{
    FILE *fd = fopen("/proc/loadavg", "r");
    int pid;
    fscanf(fd, "%d.%d %d.%d %d.%d %d/%d %d", &pid, &pid, &pid, &pid, &pid, &pid, &pid, &pid, &pid);
    fclose(fd);
    return pid;
}

int neonate(int sec)
{
    enable_raw_mode();
    int slice = min(sec * 1000, 100);

    unsigned int millis = 0;
    int st = 0, end = 0;

    while (1)
    {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
            die("read");
        if (c == 'x')
            break;
        sleep_millis(slice);
        millis += slice;

        if (millis == sec * 500)
        {
            printf("%d\r\n", print_latest_pid());
            millis = 0;
        }
    }
    disable_raw_mode();

    return 0;
}
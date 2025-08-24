#include <unistd.h>
#include <stdlib.h>

int main()
{
    // create an array that containe a commands
    char *cmd[] = {"ls", "-l", NULL};

    // run the command
    if (execvp(cmd[0], cmd) == -1)
        exit(-1);
}
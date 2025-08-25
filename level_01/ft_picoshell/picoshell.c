#include <unistd.h>
#include <stdlib.h>

int picoshell(char **cmds[])
{
    if (!cmds || !cmds[0])
        return (1);
    int pipefds[2];
    // int prev_fd = -1;
    int i = 0;
    while (cmds[i])
    {
        if (cmds[i + 1])
        {
            if (pipe(pipefds) == -1)
                return (1);
        }
        else
        {
            pipefds[0] = -1;
            pipefds[1] = -1;
        }
        if (fork() == 0)
        {
            execvp(cmds[i][0], cmds[i]);
            exit(1);
        }
        i++;
    }
    return (0);
}





int main(void)
{
    char *ls[] = {"ls", NULL};
    // char *wc[] = {"wc", "-l", NULL};
    char **cmds[] = {ls, NULL};

    return picoshell(cmds);
}

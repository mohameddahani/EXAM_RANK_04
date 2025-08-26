#include <unistd.h>
#include <stdlib.h>

int picoshell(char **cmds[])
{
    if (!cmds || !cmds[0])
        return (1);
    int pipefds[2];
    int prev_fd = -1;
    int i = 0;
    while (cmds[i])
    {
            int has_next = (cmds[i + 1]);
            if (has_next && pipe(pipefds) == -1)
                return (1);
        int pid = fork();
        if (pid == -1)
        {
            if (has_next)
            {
                close(pipefds[0]);
                close(pipefds[1]);
            }
            return (1);            
        }
        if (pid == 0)
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

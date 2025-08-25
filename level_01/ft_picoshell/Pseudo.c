#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int picoshell(char **cmds[])
{
    int i = 0;
    int prev_fd = -1;
    int pipefd[2];
    pid_t pid;
    int exit_code = 0; // باش نراقبو أي error فـ child

    while (cmds[i])
    {
        int has_next = (cmds[i + 1] != NULL);
        if (has_next && pipe(pipefd) == -1)
            return 1;

        pid = fork();
        if (pid == -1)
        {
            if (has_next)
            {
                close(pipefd[0]);
                close(pipefd[1]);
            }
            return 1;
        }

        if (pid == 0) // child
        {
            if (prev_fd != -1)
            {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }
            if (has_next)
            {
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[0]);
                close(pipefd[1]);
            }
            execvp(cmds[i][0], cmds[i]);
            // execvp failed
            perror("execvp");
            exit(1);
        }
        else // parent
        {
            if (prev_fd != -1)
                close(prev_fd);
            if (has_next)
            {
                close(pipefd[1]);
                prev_fd = pipefd[0];
            }
        }
        i++;
    }

    // انتظر جميع children وفحص exit status
    int status;
    while (wait(&status) > 0)
    {
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            exit_code = 1; // واحد من children فشل
        else if (WIFSIGNALED(status))
            exit_code = 1; // قتل من signal
    }

    if (prev_fd != -1)
        close(prev_fd);

    return exit_code; // 0 = success, 1 = أي خطأ
}

// -------------------- Main --------------------
int main(void)
{
    char *ls[] = {"ls", NULL};
    char *wc[] = {"wc", "-l", NULL};
    char **cmds[] = {ls, wc, NULL};

    return picoshell(cmds);
}

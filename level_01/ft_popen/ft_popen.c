#include <unistd.h>
#include <stdlib.h>

int ft_popen(const char *file, char *const argv[], char type)
{
    int pipefds[2];
    
    if (!file || !argv || (type != 'r' && type != 'r'))
        return (-1);
    if (pipe(pipefds) == -1)
        return (-1);
    if (fork() == 0)
    {
        if (type == 'r')
        {
            close(pipefds[0]);
            if (dup2(pipefds[1], 1) == -1)
            {
                close(pipefds[1]);
                exit(-1);   
            }
            close(pipefds[1]);
        }
        else
        {
            close(pipefds[1]);
            if (dup2(pipefds[0], 0) == -1)
            {
                close(pipefds[0]);
                exit(-1);
            }
            close(pipefds[0]);
        }
        execvp(file, argv);
        exit(-1);
    }
    if (type == 'r')
    {
        close(pipefds[1]);
        return (pipefds[0]);
    }
    else
    {
        close(pipefds[0]);
        return (pipefds[1]);
    }
    
}

#include <stdio.h>

int main(void)
{
    int fd;
    char buffer[1024];
    int bytes_read;

    fd = ft_popen("ls", (char *const []){"ls", NULL}, 'r');

    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0)
    {
        write(1, buffer, bytes_read);
    }
    close(fd);
    return 0;
}



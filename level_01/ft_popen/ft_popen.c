#include <unistd.h>
#include <stdlib.h>

int ft_popen(const char *file, char *const argv[], char type)
{
    int fds[2];
    
    if (!file || !argv || (type != 'r' && type != 'w'))
        return (-1);    
    if (pipe(fds) == -1)
        return (-1);
    int pid = fork();
    if (pid == -1)
        return (-1);
    if (pid == 0)
    {
        if (type == 'r')
        {
            close(fds[0]);
            if (dup2(fds[1], STDOUT_FILENO) == -1)
                exit(-1);
            close(fds[1]);
        }
        else
        {
            close(fds[1]);
            if (dup2(fds[0], STDIN_FILENO) == -1)
                exit(-1);
            close(fds[0]);
        }
        execvp(file, argv);
    }
    else
    {
        if (type == 'r')
        {
            close(fds[1]);
            return(fds[0]);
        }
        else
        {
            close(fds[0]);
            return(fds[1]);
        }
    }
    return(0);
}

#include <stdio.h>

// read
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

// write
// #include <string.h>
// int main(void)
// {
//     int fd = ft_popen("cat", (char *const []){"cat", NULL}, 'w');
//     char *msg = "Hello from ft_popen with type 'w'!\n";
//     write(fd, msg, strlen(msg));
//     close(fd);
//     return 0;
// }



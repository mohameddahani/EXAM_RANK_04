/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_popen.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdahani <mdahani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 09:47:43 by mdahani           #+#    #+#             */
/*   Updated: 2025/09/01 09:47:44 by mdahani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
    {
        close(fds[0]);
        close(fds[1]);
        return (-1);
    }
    if (pid == 0)
    {
        if (type == 'r')
        {
            close(fds[0]);
            dup2(fds[1], STDOUT_FILENO);
            close(fds[1]);
        }
        else
        {
            close(fds[1]);
            dup2(fds[0], STDIN_FILENO);
            close(fds[0]);
        }
        execvp(file, argv);
        exit(-1);
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
    return(-1);
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



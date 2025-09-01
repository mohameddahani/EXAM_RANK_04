/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdahani <mdahani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 09:48:09 by mdahani           #+#    #+#             */
/*   Updated: 2025/09/01 09:48:10 by mdahani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    // init the two pipes to communicate between child process and parent process
    int pipefds[2];

    // init buffer
    char buffer[256] = {0};

    // print the parent process id
    printf("parent process id: %d\n", getpid());

    // int the pipe
    if (pipe(pipefds) < 0)
        exit(-1);

    // write a msg in fd[1]
    write(pipefds[1], "msg", 3);

    if (fork() == 0)
    {
        // read from fd[0] in child process
        read(pipefds[0], buffer, 3);
        
        // print all things you read from fd[0]
        printf("msg: %s\n", buffer);
    
        // print the parent child id
        printf("child process id: %d\n", getpid());
    }
}

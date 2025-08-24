/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_popen.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 15:53:54 by lsilva-x          #+#    #+#             */
/*   Updated: 2025/08/04 16:47:35 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h> //pipe, fork, dup2, execvp, close
#include <stdlib.h> //exit

int ft_popen(const char *file, char *const argv[], char type);

//! auxiliary function to close fds
static void	close_fd(int fd[2])
{
	close(fd[0]);
	close(fd[1]);
}

int ft_popen(const char *file, char *const argv[], char type)
{
	int		fd[2];
	int		pid;
	
	//* Invalid input error validation
	if (!file || !argv || (type != 'r' && type != 'w'))
		return (-1);
	//* create a pipe and handle error
	if (pipe(fd) == -1)
		return (-1);
		
	pid = fork();
	if (pid == -1) //*subprocess creation error validation
		return (close_fd(fd), -1);
	//!Child process routine
	if (pid == 0)
	{
		if (type == 'r') //! Read case
		{
			close(fd[0]);
			//*Error handler in dup2
			if (dup2(fd[1], STDOUT_FILENO) == -1)
			{
				close(fd[1]);
				exit (-1);
			}
			close(fd[1]);
		}
		else  //! Write case
		{
			close(fd[1]);
			//*Error handler in dup2
			if (dup2(fd[0], STDIN_FILENO) == -1)
			{
				close(fd[0]);
				exit (-1);
			}
			close(fd[0]);
		}
		//! Run the bin in child process
		execvp(file, argv);
		//* Error handler in execvp failed case
		exit (-1);
	}
	//!Parent process
	if (type == 'r')
	{
		close(fd[1]); //*Close the unused fd
		return (fd[0]);
	}
	else
	{
		close(fd[0]); //*Close the unused fd
		return (fd[1]);
	}
	//* Error handler
	return (-1);
}

//!Main function given in subject to test the code
#include <stdio.h>

int main()
{
    int  fd;
    char line[9999];
	int		b_read;

    fd = ft_popen("ls", (char *const []){"ls", NULL}, 'r');
	dup2(fd, STDIN_FILENO);
	close(fd);
	fd = ft_popen("wc", (char *const []){"wc", "-l", NULL}, 'w');
    while (b_read = read(0, line, 9999))
	{
		line[b_read] = '\0';
		write(fd, line, b_read);
	}
	close(fd);
    return (0);
}
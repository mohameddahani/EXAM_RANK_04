/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   picoshell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdahani <mdahani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 09:47:53 by mdahani           #+#    #+#             */
/*   Updated: 2025/09/01 09:47:54 by mdahani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int picoshell(char **cmds[])
{
	int fds[2], prev_fd = -1;

	if (!cmds || !cmds[0])
		return (1);
	int i = 0;
	while (cmds[i])
	{
		int has_next = (cmds[i + 1] != NULL);
		if (has_next && pipe(fds) == -1)
			return (1);
		int pid = fork();
		if (pid == -1)
			return (1);
		if (pid == 0)
		{
			if (prev_fd != -1)
			{
				dup2(prev_fd, STDIN_FILENO);
				close(prev_fd);
			}
			if (has_next)
			{
				close(fds[0]);
				dup2(fds[1], STDOUT_FILENO);
				close(fds[1]);
			}
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		else
		{
			if (prev_fd != -1)
				close(prev_fd);	
			if (has_next)
			{
				close(fds[1]);
				prev_fd = fds[0];
			}
		}
		i++;
	}
	while (wait(NULL) > 0)
		;
	return (0);
}

int main()
{
	char *ls[] = {"ls", NULL};
	char *wc[] = {"wc", "-l", NULL};
	char **cmds[] = {ls, wc, NULL};
	return (picoshell(cmds));
}

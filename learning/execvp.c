/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execvp.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdahani <mdahani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 09:48:13 by mdahani           #+#    #+#             */
/*   Updated: 2025/09/01 09:48:14 by mdahani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
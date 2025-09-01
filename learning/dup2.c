/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dup2.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdahani <mdahani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 09:48:19 by mdahani           #+#    #+#             */
/*   Updated: 2025/09/01 09:48:20 by mdahani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    // open the file that you want to print the result to him
    int fd = open("file.txt", O_WRONLY);

    // redirect the output in file
    if (dup2(fd, 1) == -1)
        exit(1);
    printf("hello world\n");
    close(fd);
}

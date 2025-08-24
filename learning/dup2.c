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

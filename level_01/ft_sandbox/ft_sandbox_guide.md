# ft_sandbox Implementation Guide

## Required Libraries

The following header files are essential for implementing the ft_sandbox functionality:

```c
#include <stdbool.h>    // bool, true, false
#include <unistd.h>     // fork, alarm
#include <sys/wait.h>   // wait, waitpid
#include <stdlib.h>     // exit
#include <signal.h>     // signal, sigaction, kill
#include <sys/types.h>  // pid_t
#include <string.h>     // strsignal
#include <errno.h>      // EINTR
#include <stdio.h>      // printf
```

## Signal Handler Function

The alarm handler function is designed to handle timeout signals without performing any specific action:

```c
void alarm_handler(int sig)
{
    (void)sig;  // Suppress unused parameter warning
    return;
}
```

## Core Concepts and System Calls

### Understanding EINTR

`EINTR` is an error macro from the `errno.h` library that indicates a system call was interrupted by a signal before it could complete entirely.

The `waitpid` function waits for state changes in child processes. It returns when:
- The child process returns normally
- The child process exits
- A signal is received (SIGINT, SIGSEGV, etc.)
- The process is stopped (WUNTRACED)
- The process continues (WCONTINUED)

When `waitpid` returns `-1`, it's not necessarily due to how the process terminated, but because `waitpid` itself was interrupted by a signal from the child process.

The standard `waitpid` implementation blocks the parent process until the child process changes state. If the child process receives a signal that causes the parent to exit `waitpid` prematurely, `waitpid` automatically returns `-1` and sets `errno` to `EINTR`.

**Note:** The process doesn't necessarily need to terminate with a signal - a user inputting Ctrl+C (SIGINT) during a sleep operation can cause premature `waitpid` termination.

A common pattern to prevent user interruption is:

```c
do {
    res = waitpid(pid, &status, 0);
} while (res == -1 && errno == EINTR);
```

This loop continues until the child process exits successfully or returns an error other than interruption.

### SA_RESTART Flag

The `SA_RESTART` flag provides similar behavior to the loop above but operates at a lower level, making the program unaware of interruption requests:

```c
int main(void)
{
    struct sigaction sa;
    int status;
    pid_t pid;

    sa.sa_handler = handler_function;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);

    pid = fork();
    if (pid == 0) {
        sleep(10);
        exit(0);
    }

    waitpid(pid, &status, 0);
    // If SA_RESTART is set, this waitpid will be automatically restarted
}
```

The default behavior without any flags is to set the value to `0`:

```c
struct sigaction sa;
sa.sa_flags = 0;
```

### sigemptyset Function

Some signals can be executed during the execution of a sigaction handler. To specify which signals should be ignored or if none should be ignored, we use `sigemptyset`, which clears or sets the signal mask belonging to that sigaction.

This function determines **which signals should be temporarily blocked while the handler is executing**.

### SIGUSR1 and SIGUSR2

These are two reserved signals in POSIX systems that have no specific utility by default. They can be defined with various behaviors depending on our code requirements.

## Implementation Steps

### 1. Define the sigaction Structure

```c
void sighandler(int sig)
{
    (void)sig;  // Suppress unused parameter warning
}

int main(void)
{
    struct sigaction sa;

    sa.sa_handler = sighandler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGALRM, &sa, NULL);
}
```

### 2. Create Subprocess and Execute Function

```c
int main(void)
{
    pid_t pid;
    // ... previous code ...
    
    pid = fork();
    if (pid == -1)
        return (-1);
    if (pid == 0) {
        f();  // Function pointer from ft_sandbox scope
        exit(0);
    }
}
```

### 3. Set Alarm and Wait Loop

This is where we use the timeout value set in the function scope to configure the alarm and enter the waitpid loop:

```c
int main(void)
{
    // ... previous code ...
    
    alarm(timeout);
    if (waitpid(pid, &status, 0) == -1) {
        // Case where child process sent signal to exit waitpid
        if (errno == EINTR) {
            kill(pid, SIGKILL);     // Terminate subprocess
            waitpid(pid, NULL, 0);  // Wait for termination
            if (verbose)
                printf("Bad function: timed out after %d seconds\n", timeout);
            return (0);  // Return success as function was intentionally stopped
        }
        return (-1);  // Case where stop was not intentional
    }
}
```

### 4. Check for Normal Exit

Verify if the subprocess that executed the function terminated with a conventional exit such as `return` or `exit`, and if so, check if the exit value is `0` (indicating success):

```c
int main(void)
{
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        f();
        exit(0);
    }
    
    if (waitpid(pid, &status, 0) == -1) {
        // ... handle other cases ...
    }

    if (WIFEXITED(status)) {  // Exited with return or exit
        if (WEXITSTATUS(status) == 0) {  // Exited with positive signal
            if (verbose)
                printf("Nice function!\n");
            return (1);  // Successful execution
        }
    }
}
```

### 5. Check for Signal Termination

If the above condition doesn't match, the subprocess must have exited due to a signal, meaning the function didn't work as expected:

```c
int main(void)
{
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        f();
        exit(0);
    }

    if (WIFSIGNALED(status)) {  // Process exited with a signal
        int sig = WTERMSIG(status);  // Get signal index
        if (verbose)
            printf("Bad function: %s\n", strsignal(sig));
        return (0);
    }
    return (-1);  // Return error if everything else fails
}
```

## Implementation Summary

The ft_sandbox implementation follows this structure:

1. **Import Libraries** - Include necessary system headers
2. **Set Signal Handler** - Define handler for sigaction
3. **Define sigaction Structure** - Configure signal handling
4. **Create Subprocess** - Fork and execute target function
5. **Set Alarm and Wait** - Configure timeout and wait for completion
6. **Check Normal Exit** - Verify successful termination
7. **Check Signal Termination** - Handle abnormal termination cases

This implementation provides a robust sandbox environment for testing functions with timeout capabilities and comprehensive error handling.
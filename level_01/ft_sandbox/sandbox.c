/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 20:30:14 by lsilva-x          #+#    #+#             */
/*   Updated: 2025/08/04 16:57:35 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h> //bool
#include <unistd.h> //fork, alarm, kill
#include <sys/wait.h> //waitpid
#include <stdio.h> //printf
#include <stdlib.h> //exit
#include <signal.h> //sigaction, strsignal, sigemptyset
#include <string.h> //strsignal
#include <sys/types.h>
#include <errno.h>
/*
--------------- _FT_SANDBOX_ -----------------------

Return values for functions:
- -1 -> Error
-  0 -> Success
-  1 -> Failure (e.g., terminated or stopped by a signal, exited with non-zero code, or timed out)

Function prototype:
pid_t waitpid(pid_t pid, int *status_ptr, int options);

pid values:
- pid > 0  -> Waits for the termination of the specific child process with that PID.
- pid == 0 -> Waits for any child whose process group ID is equal to that of the calling process.
- pid == -1 -> Waits for termination of **any** child process.

status_ptr:
- Pointer to an `int` variable that will store the child's termination status.

options:
- 0 -> Default behavior (wait until the child terminates).

--------------------------------------
Process termination (normal or abnormal):

Normal termination (e.g., via `exit()`, `_exit()`, `_Exit()`):
- `WIFEXITED(status)`      -> True if the child terminated normally.
- `WEXITSTATUS(status)`    -> Returns the exit status of the child (only valid if WIFEXITED is true).

Abnormal termination (e.g., via signals like `SIGKILL`, `SIGSEGV`, `SIGTERM`):
- `WIFSIGNALED(status)`    -> True if the child was terminated by a signal.
- `WTERMSIG(status)`       -> Returns the signal number that caused the child to terminate.

--------------------------------------
EINTR:
- `EINTR` (Interrupted System Call) is an error code.
- Happens when system calls like `read()`, `write()`, or `wait()` are interrupted by a signal **before** they complete.
*/

static pid_t child_pid;

//Function that will be set in the sigaction
void	alarm_handler(int sig)
{
	(void)sig;
}

int sandbox(void (*f)(void), unsigned int timeout, bool verbose);

int sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
	struct sigaction	sa;
	pid_t				pid;
	int					status;

	//Set the sigaction function set
	sa.sa_handler = alarm_handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGALRM, &sa, NULL);

	pid = fork();
	//* Error handler
	if (pid == -1)
		return (-1);
	//! Child process
	if (pid == 0)
	{
		f();
		exit(0);
	}
	child_pid = pid;
	alarm(timeout); //!Set the alarm function to be able to recive the SIGALRM signal
	
	if (waitpid(pid, &status, 0) == -1)
	{
		if (errno == EINTR)
		{
			kill(pid, SIGKILL); //! kil the child process in case where the timeout is reached
			waitpid(pid, NULL , 0); // kill the process zombie
			if (verbose)
				printf ("Bad function: timed out after %d seconds\n", timeout);
			return (0);
		}
		return (-1);
	}

	//! Handle the case where de child process terminate by a function (return, exit...)
	if (WIFEXITED(status))
	{
		if (WEXITSTATUS(status) == 0) //!Normal function (a good one)
		{
			if (verbose)
				printf ("Nice function!\n");
			return (1); //nice function
		}
		else //!Bad function exit wiht a signal different from 0;
		{
			if (verbose)
				printf ("Bad function: exited with code %d\n", WEXITSTATUS(status));
			return (0); //bad function
		}
	}
	
	//! Case where the process terminate with a signal instead a function (SIGKILL, SIGFAULT, ...)
	if (WIFSIGNALED(status))
	{
		int sig = WTERMSIG(status);
		if (verbose)
			printf ("Bad function: %s\n", strsignal(sig));
		return (0);
	}
	return (-1);
}

//--------------------------Handler function to teste the code-----------
 void nice_function(void)
 {
    return;
 }


void bad_function_exit_code(void)
{
    // Esta función termina con código de error
    exit(1);
}

void bad_function_segfault(void)
{
    // Esta función causa segmentation fault
    int *ptr = NULL;
    *ptr = 42;
}

void bad_function_timeout(void)
{
    // Esta función nunca termina
    while (1) {}
}

void bad_function_abort(void)
{
    // Esta función llama abort()
    abort();
}

// Helper to check sandbox result and print outcome
void check_result(const char *test_name, int expected, int actual) {
	if (expected == actual)
		printf("[PASS] %s (expected: %d, got: %d)\n", test_name, expected, actual);
	else
		printf("[FAIL] %s (expected: %d, got: %d)\n", test_name, expected, actual);
}

// Additional test functions
void bad_function_divzero(void) {
	int x = 1 / 0;
	(void)x;
}

void bad_function_sigfpe(void) {
	raise(SIGFPE);
}

void bad_function_sigterm(void) {
	raise(SIGTERM);
}

void bad_function_sigkill(void) {
	raise(SIGKILL);
}

void nice_function_sleep(void) {
	sleep(1);
}

int main(void)
{
	int result;

	// Test: nice function
	result = sandbox(nice_function, 5, false);
	check_result("nice_function", 1, result);

	// Test: nice function with sleep
	result = sandbox(nice_function_sleep, 5, false);
	check_result("nice_function_sleep", 1, result);

	// Test: bad function exit code
	result = sandbox(bad_function_exit_code, 5, false);
	check_result("bad_function_exit_code", 0, result);

	// Test: bad function segfault
	result = sandbox(bad_function_segfault, 5, false);
	check_result("bad_function_segfault", 0, result);

	// Test: bad function timeout
	result = sandbox(bad_function_timeout, 2, false);
	check_result("bad_function_timeout", 0, result);

	// Test: bad function abort
	result = sandbox(bad_function_abort, 5, false);
	check_result("bad_function_abort", 0, result);

	// Test: bad function division by zero
	result = sandbox(bad_function_divzero, 5, false);
	check_result("bad_function_divzero", 0, result);

	// Test: bad function SIGFPE
	result = sandbox(bad_function_sigfpe, 5, false);
	check_result("bad_function_sigfpe", 0, result);

	// Test: bad function SIGTERM
	result = sandbox(bad_function_sigterm, 5, false);
	check_result("bad_function_sigterm", 0, result);

	// Test: bad function SIGKILL
	result = sandbox(bad_function_sigkill, 5, false);
	check_result("bad_function_sigkill", 0, result);

	// Test: zero timeout (should always timeout)
	result = sandbox(nice_function_sleep, 0, false);
	check_result("nice_function_sleep_zero_timeout", 1, result);

	// Test: NULL function pointer (should not crash, but sandbox will fork and exit)
	result = sandbox(NULL, 5, false);
	check_result("null_function_pointer", 0, result);

	return 0;
}
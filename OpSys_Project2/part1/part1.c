#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
	// variable to store calling function's proc. ID
	pid_t process_id;

	// getpid() returns process id of calling function
	process_id = getpid();

	process_id = getpid();

	process_id = getpid();

	process_id = getpid();

	process_id = getpid();

	process_id = getpid();
	return 0;
}

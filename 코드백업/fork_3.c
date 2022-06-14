#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
	if(fork() ==0)
		if(fork())
			printf("Hello world!\n");
	exit(0);
}

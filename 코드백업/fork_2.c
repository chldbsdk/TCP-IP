#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
int main() 
{
	fork();
	fork();
	fork();
	printf("Hello world!\n");
	return 0;
}

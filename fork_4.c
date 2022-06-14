#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int doWork() {
	fork();
	fork();
	printf("Hello world!\n");
}

int main() {
	doWork();
	printf("Hello world!\n");
	exit(0);
}

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
	char buffer[20];
	int iret;
	pthread_t tid = pthread_self();
	iret = pthread_getname_np(tid, buffer, 20);
	if (iret != 0) {
		printf("getname returned %d\n", iret);
		exit(iret);
	}
	printf("Getname buffer: %s\n", buffer);
	printf("Thread ID: %u\n", tid);

	char* buffer_2 = "name";
	iret = pthread_setname_np(tid, buffer_2);
	if (iret != 0) {
		printf("setname returned %d\n", iret);
		exit(iret);
	}

	iret = pthread_getname_np(tid, buffer, 20);
	if (iret != 0) {
		printf("getname returned %d\n", iret);
		exit(iret);
	}
	printf("Getname buffer: %s\n", buffer);
	printf("Thread ID: %u\n", tid);
	return 0;
}

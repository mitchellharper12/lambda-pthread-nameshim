#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
	pthread_t tid = pthread_self();
	char* buffer_2 = "name";
	int iret;
	iret = pthread_setname_np(tid, buffer_2);
	if (iret != 0) {
		printf("getname returned %d\n", iret);
		exit(iret);
	}
	char buffer[20];
	iret = pthread_getname_np(tid, buffer, 20);
	if (iret != 0) {
		printf("getname returned %d\n", iret);
		exit(iret);
	}
	printf("Getname buffer: %s\n", buffer);
	printf("Thread ID: %u\n", tid);
	return 0;
}

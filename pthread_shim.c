#define _GNU_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <stdio.h>
#include <errno.h>

#define __TMP_THREAD_FILE "/tmp/threads"
#define __SHIM_DEBUG 0

int pthread_getname_np(pthread_t thread, char* name, size_t len) {
	int (*orig_getname)(pthread_t thread, char* name, size_t len);
	FILE* file = fopen(__TMP_THREAD_FILE, "r");
	char line[48];
	pthread_t line_id;
	char line_name[20];
	char thread_id_str[20];
	int thread_found = 0;

	orig_getname = dlsym(RTLD_NEXT, "pthread_getname_np");

	if (file == NULL) {
		if (__SHIM_DEBUG)
			printf("No threads file, falling back\n");
		return orig_getname(thread, name, len);
	}

	while (!feof(file)) {
		fgets(line, sizeof(line), file);
		if (__SHIM_DEBUG) {
		    printf("In while loop\n");
		    printf("Current line: '%s'\n", line);
		}
		char cur_char = '\0';
		int cur_pos = -1;
		while (cur_char != '\t' && cur_pos < 20) {
			cur_pos++;
			cur_char = line[cur_pos];
			line_name[cur_pos] = cur_char;
		}
		if (cur_pos == 20) {
			fclose(file);
			return 2; // Should never hit this
		}
		// cur_char == '\t'
		cur_pos++;
		line_name[cur_pos] = '\0';
		int thread_id_pos = 0;
		while (cur_char != '\n') { // fgets retains the newline
			cur_char = line[cur_pos];
			thread_id_str[thread_id_pos] = cur_char;
			cur_pos++;
			thread_id_pos++;
		}
		thread_id_str[thread_id_pos] = '\0';
		if (__SHIM_DEBUG)
			printf("Thread id str: %s\n", thread_id_str);
		sscanf(thread_id_str, "%u", &line_id);

		// This line is quite the hack..but it works
		// TODO make this less terrible
		int diff =(int) ((size_t) line_id - (size_t) thread);
		if (__SHIM_DEBUG) {
			printf("current recovered line id: %u\n", line_id);
			printf("Diff: %d\n", diff);
		}

		if (diff == 0) {
			if (__SHIM_DEBUG) {
				printf("Name found for %u\n", line_id);
			}
			thread_found = 1;
			size_t name_len = strlen(line_name);
			if (name_len + 1 > len) {
				fclose(file);
				return 1; // Maybe do something better here
			}
			memcpy(name, line_name, len);
			// We can't return here, there may
			// have been a later update to the name
			// and we have to scan the whole file
			// to make sure
		}
	}
	fclose(file);
	if (thread_found) {
		// there was at least 1 processed
		// successfully
		return 0;
	}

	// Not found in file, fall back to default
	if (__SHIM_DEBUG) {
		printf("Falling back to orig getname\n");
	}
	
	return orig_getname(thread, name, len);
}

int pthread_setname_np(pthread_t thread, const char* name) {
	int len = strlen(name);
	if (len + 1 > 16)
		return 1;
	FILE* file = fopen(__TMP_THREAD_FILE, "a");
	char line_buffer[48];
	int line_len = snprintf(line_buffer, 48, "%s\t%u\n", name, thread);
	size_t written = fwrite(line_buffer, line_len, 1, file);
	fclose(file);
	return 0;
}

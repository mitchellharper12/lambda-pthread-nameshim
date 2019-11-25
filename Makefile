.PHONY: run, valgrind

run: test_shim pthread_shim.so
	LD_PRELOAD=./pthread_shim.so ./test_shim
valgrind: test_shim pthread_shim.so
	LD_PRELOAD=./pthread_shim.so valgrind ./test_shim

test_shim: pthread_shim.so test_shim.c
	gcc -lpthread test_shim.c -g -o test_shim

pthread_shim.so: pthread_shim.c
	gcc -ldl -fPIC -shared pthread_shim.c -g -o pthread_shim.so
	

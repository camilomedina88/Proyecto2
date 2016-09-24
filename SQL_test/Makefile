CC=gcc
CFLAGS=-I.
DEPS = 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

hellomake: test1.o 
	gcc -o test1 test1.o -l sqlite3

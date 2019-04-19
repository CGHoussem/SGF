all: main

main:
	gcc main.c -o main.o

clean:
	rm -r *.o
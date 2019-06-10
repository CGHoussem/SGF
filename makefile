CC = gcc
CFLAGS = -Wall -g
OBJS = save_load.o primitives.o command_shell.o main.o

all: program

program: $(OBJS)
	$(CC) $(CFLAGS) -o program $(OBJS) -lm

main.o: main.c
	$(CC) $(CFLAGS) -c -o main.o main.c

save_load.o: save_load_SGF.c save_load_SGF.h
	$(CC) $(CFLAGS) -c -o save_load.o save_load_SGF.c

primitives.o: primitives.c primitives.h
	$(CC) $(CFLAGS) -c -o primitives.o primitives.c

command_shell.o: command_shell.c command_shell.h
	$(CC) $(CFLAGS) -c -o command_shell.o command_shell.c

clean:
	rm -f $(OBJS) program

CC = gcc
CFLAGS = -std=c99
OBJS = utility.o parson.o save_load.o primitives.o command_shell.o main.o

all: CFLAGS += -D DEBUG=0
all: program

debug: CFLAGS += -Wall -g -D DEBUG=1 #-Werror
debug: program

program: $(OBJS)
	$(CC) $(CFLAGS) -o program $(OBJS) -lm

main.o: main.c
	$(CC) $(CFLAGS) -c -o main.o main.c

utility.o: utility.c utility.h
	$(CC) $(CFLAGS) -c -o utility.o utility.c

parson.o: parson.c parson.h
	$(CC) $(CFLAGS) -c -o parson.o parson.c

save_load.o: save_load.c save_load.h
	$(CC) $(CFLAGS) -c -o save_load.o save_load.c

primitives.o: primitives.c primitives.h
	$(CC) $(CFLAGS) -c -o primitives.o primitives.c

command_shell.o: command_shell.c command_shell.h
	$(CC) $(CFLAGS) -c -o command_shell.o command_shell.c

clean:
	rm -f $(OBJS) program

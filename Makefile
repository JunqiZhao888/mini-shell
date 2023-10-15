CC = gcc
CFLAGS = -g -pedantic -std=gnu17 -Wall -Werror -Wno-error -Wextra

.PHONY: all
all: nyush

nyush: builtIn.o command.o dealDirCmd.o main.o prompt.o
	$(CC) $(CFLAGS) -o $@ $^

builtIn.o: builtIn.c myHelp.h
	$(CC) $(CFLAGS) -c $< -o $@

command.o: command.c myHelp.h
	$(CC) $(CFLAGS) -c $< -o $@

dealDirCmd.o: dealDirCmd.c myHelp.h
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c myHelp.h
	$(CC) $(CFLAGS) -c $< -o $@

prompt.o: prompt.c myHelp.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f *.o nyush
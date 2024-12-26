# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -pthread -lrt

# Targets
all: chef serveur

chef: chef.c
	$(CC) $(CFLAGS) -o chef chef.c $(LDFLAGS)

serveur: serveur.c
	$(CC) $(CFLAGS) -o serveur serveur.c $(LDFLAGS)

clean:
	rm -f chef serveur *.o

.PHONY: all clean
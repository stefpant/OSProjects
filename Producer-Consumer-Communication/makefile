CC=gcc
CFLAG1=-Wall
CFLAG2=-lm

process: main.o
	$(CC) $(CFLAG1) -o process main.o $(CFLAG2)
main.o:main.c
	$(CC) $(CFLAG1) -c main.c $(CFLAG2)

.PHONY: clean
clean:
	rm -f process main.o

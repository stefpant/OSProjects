CC=gcc
CFLAG=-Wall

main: main.o entry.o inverted_pt.o
	$(CC) $(CFLAG) -o main main.o entry.o inverted_pt.o
main.o:main.c
	$(CC) $(CFLAG) -c main.c
entry.o:entry.c
	$(CC) $(CFLAG) -c entry.c
inverted_pt.o:inverted_pt.c
	$(CC) $(CFLAG) -c inverted_pt.c

.PHONY: clean
clean:
	rm -f main main.o entry.o inverted_pt.o

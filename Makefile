CC = gcc
RM = rm -f

all: raycaster2

ppm: raycaster2.c
	$(CC) raycaster2.c -o raycaster2

clean:
	$(RM) raycaster2 *~

#
#

CFLAGS= -g -Wall -O0

all: corn

corn: corn.c
	$(CC) $(CFLAGS) -o corn corn.c -lpng
	./corn ../testocr.png ../testocr.ppm

clean:
	rm -f corn *.o

VPATH=src
SRC_FILES=nmea.c parse.c gpgga.c gpgll.c
OBJ_FILES=$(patsubst %.c, %.o, $(SRC_FILES))
CC=gcc
CFLAGS=-c -fPIC -g -Wall
LDFLAGS=-s -shared -Wl,--no-as-needed,-soname,libnmea.so -Wall -g

nmea:	$(OBJ_FILES)
	$(CC) $(LDFLAGS) $(OBJ_FILES) -o libnmea.so

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

test:
	$(CC) test.c -I./src/ -L. -lnmea -o test

clean:
	@rm -f *.o
	@rm -f libnmea.so
	@rm -f test

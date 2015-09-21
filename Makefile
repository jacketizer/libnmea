VPATH=src
SRC_FILES=nmea.c parse.c sentences/parser.c
SRC_SENTENCE_FILES=parse.c sentences/gpgll.c
OBJ_FILES=$(patsubst %.c, %.o, $(SRC_FILES))
OBJ_SENTENCE_FILES=$(patsubst %.c, %.o, $(SRC_SENTENCE_FILES))
CC=gcc
CFLAGS=-c -fPIC -g -Wall
LDFLAGS=-s -shared -Wl,--no-as-needed,-soname,libnmea.so -ldl -Wall -g

nmea:	$(OBJ_FILES)
	$(CC) $(LDFLAGS) $(OBJ_FILES) -o libnmea.so

gpgll:	$(OBJ_SENTENCE_FILES)
	$(CC) -s -shared -Wl,--no-as-needed,-soname,libgpgll.so -Wall -g $(OBJ_SENTENCE_FILES) -o sentences/libgpgll.so

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

test:
	$(CC) test.c -I./src/ -L. -lnmea -o test

clean:
	@rm -f *.o
	@rm -f sentences/*
	@rm -f libnmea.so
	@rm -f test

VPATH=src
SRC_FILES=nmea.c parse.c parsers/parser.c
OBJ_FILES=$(patsubst %.c, %.o, $(SRC_FILES))

SRC_PARSER_DEP=parse.c
OBJ_PARSER_DEP=$(patsubst %.c, %.o, $(SRC_PARSER_DEP))

CC=gcc
CFLAGS=-c -fPIC -g -Wall
LDFLAGS=-s -shared -Wl,--no-as-needed,-soname,libnmea.so -ldl -Wall -g

nmea:	$(OBJ_FILES)
	$(CC) $(LDFLAGS) $(OBJ_FILES) -o libnmea.so

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

parsers/%: $(OBJ_PARSER_DEP)
	$(CC) -s -shared -Wl,--no-as-needed,-soname,$(patsubst parsers/%,lib%.so,$@) -Wall -g $(OBJ_PARSER_DEP) -o $(patsubst parsers/%,parsers/lib%.so,$@)

test:
	$(CC) test.c -I./src/ -L. -lnmea -o test

clean:
	@rm -f *.o
	@rm -f parsers/*.o

clean-all:
	@rm -f *.o
	@rm -f parsers/*
	@rm -f libnmea.so
	@rm -f test

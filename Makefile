VPATH=src
SRC_FILES=nmea.c parse.c parser.c
OBJ_FILES=$(patsubst %.c, %.o, $(SRC_FILES))

SRC_PARSER_DEP=parse.c
OBJ_PARSER_DEP=$(patsubst %.c, %.o, $(SRC_PARSER_DEP))
SRC_PARSERS=$(wildcard src/parsers/*.c)
OBJ_PARSERS=$(patsubst src/%.c, %, $(SRC_PARSERS))

CC=gcc
CFLAGS=-c -fPIC -g -Wall
LDFLAGS=-s -shared -Wl,--no-as-needed,-soname,libnmea.so -ldl -Wall -g

all: nmea parser-libs

nmea:	$(OBJ_FILES)
	@echo "Building libnmea.so..."
	$(CC) $(LDFLAGS) $(OBJ_FILES) -o libnmea.so

parser-libs: $(OBJ_PARSERS)

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

parsers/%: $(OBJ_PARSER_DEP)
	@mkdir -p parsers
	@echo Building $(patsubst parsers/%,lib%.so,$@)...
	$(CC) -s -fPIC -Wall -g -shared -Wl,--no-as-needed,-soname,$(patsubst parsers/%,lib%.so,$@) $(VPATH)/$@.c $(OBJ_PARSER_DEP) -o $(patsubst parsers/%,parsers/lib%.so,$@)

test: test.c
	$(CC) test.c -I./src/ -L. -lnmea -o test

install: all
	mkdir -p /usr/lib/nmea
	mkdir -p /usr/include/nmea
	cp libnmea.so /usr/lib/
	cp parsers/*.so /usr/lib/nmea/
	cp src/nmea.h /usr/include/
	cp src/parsers/*.h /usr/include/nmea/
	ldconfig -n /usr/lib

clean:
	@rm -f *.o
	@rm -f parsers/*.o

clean-all:
	@rm -f *.o
	@rm -f parsers/*
	@rm -f libnmea.so
	@rm -f parsers/*.so
	@rm -f test

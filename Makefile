SRC_FILES=src/nmea/nmea.c src/nmea/parser.c
OBJ_FILES=$(patsubst %.c, %.o, $(SRC_FILES))

SRC_PARSER_DEP=src/parsers/parse.c
OBJ_PARSER_DEP=$(patsubst %.c, %.o, $(SRC_PARSER_DEP))
SRC_PARSERS=$(shell find src/parsers/ -type f -name "*.c" | grep -v "parse.c")
OBJ_PARSERS=$(patsubst %.c, %, $(SRC_PARSERS))

CC=gcc
CFLAGS=-c -fPIC -g -Wall
LDFLAGS=-s -shared -fvisibility=hidden -Wl,--exclude-libs=ALL,--no-as-needed,-soname,libnmea.so -ldl -Wall -g

all: 	nmea parser-libs

nmea:	$(OBJ_FILES)
	@echo "Building libnmea.so..."
	$(CC) $(LDFLAGS) $(OBJ_FILES) -o libnmea.so

parser-libs:	$(OBJ_PARSERS)

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

src/parsers/%:	$(OBJ_PARSER_DEP)
	@mkdir -p nmea
	@echo Building $(patsubst src/parsers/%,lib%.so,$@)...
	$(CC) -s -fPIC -Wall -g -shared -Isrc/nmea -L. -lnmea -Wl,--no-as-needed,-soname,$(patsubst src/parsers/%,lib%.so,$@) $@.c $(OBJ_PARSER_DEP) -o $(patsubst src/parsers/%,nmea/lib%.so,$@)

test:	test.c
	$(CC) test.c -lnmea -o test

unit-tests:	tests/test_nmea.c
	gcc -lnmea tests/test_nmea.c -o utests && ./utests

install:	all
	mkdir -p /usr/lib/nmea
	mkdir -p /usr/include/nmea
	cp libnmea.so /usr/lib/
	cp nmea/*.so /usr/lib/nmea/
	cp src/nmea/nmea.h /usr/include/
	cp src/parsers/*.h /usr/include/nmea/
	ldconfig -n /usr/lib

clean:
	@rm -f *.o
	@rm -f src/nmea/*.o
	@rm -f src/parsers/*.o

clean-all:	clean
	@rm -rf nmea
	@rm -f libnmea.so
	@rm -f test
	@rm -f utests

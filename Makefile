SRC_FILES=src/nmea/nmea.c src/nmea/parser.c
OBJ_FILES=$(patsubst %.c, %.o, $(SRC_FILES))
BUILD_PATH=build
PREFIX ?= /usr

SRC_PARSER_DEP=src/parsers/parse.c
OBJ_PARSER_DEP=$(patsubst %.c, %.o, $(SRC_PARSER_DEP))
SRC_PARSERS=$(shell find src/parsers/ -type f -name "*.c" | grep -v "parse.c")
OBJ_PARSERS=$(patsubst %.c, %, $(SRC_PARSERS))

SRC_EXAMPLES=$(shell find examples/ -type f -name "*.c")
BIN_EXAMPLES=$(patsubst %.c, %, $(SRC_EXAMPLES))

CC=gcc
CFLAGS=-c -fPIC -finline-functions -g -Wall
LDFLAGS=-s -shared -fvisibility=hidden -Wl,--exclude-libs=ALL,--no-as-needed,-soname,libnmea.so -ldl -Wall -g

.PHONY: all
all: nmea parser-libs

.PHONY: nmea
nmea: $(OBJ_FILES)
	@mkdir -p $(BUILD_PATH)
	@echo "Building libnmea.so..."
	$(CC) $(LDFLAGS) $(OBJ_FILES) -o $(BUILD_PATH)/libnmea.so
	cp src/nmea/nmea.h $(BUILD_PATH)

.PHONY: parser-libs
parser-libs: $(OBJ_PARSERS)

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

src/parsers/%: $(OBJ_PARSER_DEP)
	@mkdir -p $(BUILD_PATH)/nmea
	@echo Building $(patsubst src/parsers/%,lib%.so,$@)...
	$(CC) -s -fPIC -Wall -g -shared -Isrc/nmea -L$(BUILD_PATH) -I$(BUILD_PATH) -Wl,--no-as-needed,-soname,$(patsubst src/parsers/%,lib%.so,$@) $@.c $(OBJ_PARSER_DEP) -o $(patsubst src/parsers/%,$(BUILD_PATH)/nmea/lib%.so,$@)
	cp $@.h $(BUILD_PATH)/nmea/

examples/%: examples/%.c
	$(CC) $< -lnmea -o $(BUILD_PATH)/$(patsubst examples/%,%,$@)

.PHONY: examples
examples: nmea parser-libs $(BIN_EXAMPLES)

.PHONY: unit-tests
unit-tests: tests/unit-tests/test_lib.c tests/unit-tests/test_parse.c tests/unit-tests/test_nmea_helpers.c
	@$(CC) tests/unit-tests/test_lib.c -lnmea -o utests
	@$(CC) src/parsers/parse.c tests/unit-tests/test_parse.c -o utests-parse
	@$(CC) src/nmea/parser.c tests/unit-tests/test_nmea_helpers.c -ldl -o utests-nmea
	@./utests && ./utests-parse && ./utests-nmea && (echo "All tests passed!")

.PHONY: check-memory-leaks
check-memory-leaks: tests/memory-leaks.sh
	@$(CC) tests/memcheck.c -lnmea -o memcheck
	@tests/memory-leaks.sh

.PHONY: check
check:
	export LIBRARY_PATH="$(BUILD_PATH)";
	export C_INCLUDE_PATH="$(BUILD_PATH)";
	export LD_LIBRARY_PATH="$(BUILD_PATH)";
	export NMEA_PARSER_PATH="$(BUILD_PATH)/nmea/"
	make unit-tests

.PHONY: install
install: all
	install --directory $(PREFIX)/lib/nmea
	install --directory $(PREFIX)/include/nmea
	install $(BUILD_PATH)/libnmea.so $(PREFIX)/lib/
	install $(BUILD_PATH)/nmea/*.so $(PREFIX)/lib/nmea/
	install $(BUILD_PATH)/*.h $(PREFIX)/include/
	install $(BUILD_PATH)/nmea/*.h $(PREFIX)/include/nmea/
	ldconfig -n $(PREFIX)/lib

.PHONY: clean
clean:
	@rm -f *.o
	@rm -f tests/*.o
	@rm -f src/nmea/*.o
	@rm -f src/parsers/*.o
	@rm -f utests utests-parse utests-nmea memcheck

.PHONY: clean-all
clean-all: clean
	@rm -rf $(BUILD_PATH)

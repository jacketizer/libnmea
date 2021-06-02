ifdef NMEA_STATIC
SRC_FILES := src/nmea/nmea.c src/nmea/parser_static.c
PARSER_DEF := $(shell echo "$(NMEA_STATIC)" | sed -e 's/^/-DENABLE_/g' -e 's/,/ -DENABLE_/g')
PARSER_CNT := $(shell echo "$(NMEA_STATIC)" | sed 's/,/ /g' | wc -w | tr -d ' ')
else
SRC_FILES := src/nmea/nmea.c src/nmea/parser.c
endif

OBJ_FILES := $(patsubst %.c, %.o, $(SRC_FILES))
BUILD_PATH := build
PREFIX ?= /usr

SRC_PARSER_DEP := src/parsers/parse.c
OBJ_PARSER_DEP := $(patsubst %.c, %.o, $(SRC_PARSER_DEP))
SRC_PARSERS := $(shell find src/parsers/ -type f -name "*.c" | grep -v "parse.c")
OBJ_PARSERS := $(patsubst %.c, %.o, $(SRC_PARSERS))
PARSERS := $(patsubst %.c, %, $(SRC_PARSERS))

ALL_SOURCES := $(SRC_FILES) $(SRC_PARSERS) $(SRC_PARSER_DEP)
ALL_DEPEND_FILES := $(patsubst %.c,%.d,$(ALL_SOURCES))

SRC_EXAMPLES := $(shell find examples/ -type f -name "*.c")
BIN_EXAMPLES := $(patsubst %.c, %, $(SRC_EXAMPLES))

CC := gcc
CFLAGS := -c -fPIC -g -Wall -Werror
LDFLAGS := -shared -fvisibility=hidden -Wl,--exclude-libs=ALL,--no-as-needed,-soname,libnmea.so -Wall -g
LDFLAGS_DL := -ldl

define PREFIX_SYMBOL =
	@objcopy --redefine-sym $(1)=nmea_$(2)_$(1) $(3)
endef

define PREFIX_PARSER_MODULE =
	$(call PREFIX_SYMBOL,init,$(1),$(2))
	$(call PREFIX_SYMBOL,parse,$(1),$(2))
	$(call PREFIX_SYMBOL,set_default,$(1),$(2))
	$(call PREFIX_SYMBOL,allocate_data,$(1),$(2))
	$(call PREFIX_SYMBOL,free_data,$(1),$(2))
endef

LIBRARY_FILE := $(BUILD_PATH)/libnmea.so

.PHONY: all
ifdef NMEA_STATIC
# #################### #
# Static build targets #
# #################### #
all: $(LIBRARY_FILE)

$(LIBRARY_FILE): $(PARSERS) $(OBJ_FILES) $(OBJ_PARSER_DEP)
	@mkdir -p $(BUILD_PATH)
	@echo "Building $@..."
	$(CC) $(LDFLAGS) $(OBJ_PARSER_DEP) $(OBJ_PARSERS) $(OBJ_FILES) -o $@
	@cp src/nmea/nmea.h $(BUILD_PATH)

src/parsers/%: src/parsers/%.c $(OBJ_PARSER_DEP)
	@mkdir -p $(BUILD_PATH)/nmea
	@echo Building static module $*...
	$(CC) $(CFLAGS) -Isrc/nmea $@.c -o $@.o
	$(call PREFIX_PARSER_MODULE,$*,$@.o)
	@cp $@.h $(BUILD_PATH)/nmea/

%.o: %.c
	$(CC) $(CFLAGS) -MM $< > $(patsubst %.o,%.d,$@)
	$(CC) $(CFLAGS) -DSTATIC -DPARSER_COUNT=$(PARSER_CNT) $(PARSER_DEF) $< -o $@
else
# ##################### #
# Dynamic build targets #
# ##################### #
all: $(LIBRARY_FILE) parser-libs

.PHONY: nmea
$(LIBRARY_FILE): $(OBJ_FILES)
	@mkdir -p $(BUILD_PATH)
	@echo "Building $@"
	$(CC) $(LDFLAGS) $(LDFLAGS_DL) $(OBJ_FILES) -o $@
	@cp src/nmea/nmea.h $(BUILD_PATH)

src/parsers/%: src/parsers/%.c $(OBJ_PARSER_DEP)
	@mkdir -p $(BUILD_PATH)/nmea
	@echo Building dynamic module lib$*.so...
	$(CC) -s -fPIC -Wall -g -shared -Isrc/nmea -L$(BUILD_PATH) -I$(BUILD_PATH) -Wl,--no-as-needed,-soname,lib$*.so $@.c $(OBJ_PARSER_DEP) -o $(BUILD_PATH)/nmea/lib$*.so
	@cp src/parsers/$*.h $(BUILD_PATH)/nmea/

%.o: %.c
	$(CC) $(CFLAGS) -MM $< > $(patsubst %.o,%.d,$@)
	$(CC) $(CFLAGS) -DPARSER_PATH=\"$(PREFIX)/lib/nmea/\" $< -o $@
endif
# ifdef NMEA_STATIC

.PHONY: parser-libs
parser-libs: $(PARSERS)

examples/%: examples/%.c
	$(CC) -g $< -lnmea -o $(BUILD_PATH)/$(patsubst examples/%,%,$@)

.PHONY: examples
examples: $(BIN_EXAMPLES)

.PHONY: unit-tests
unit-tests:
	@$(CC) tests/unit-tests/test_lib.c -lnmea -o utests
	@$(CC) src/parsers/parse.c tests/unit-tests/test_parse.c -o utests-parse
	@$(CC) src/nmea/parser.c tests/unit-tests/test_nmea_helpers.c -ldl -o utests-nmea
	@./utests && ./utests-parse && ./utests-nmea && (echo "All tests passed!")

.PHONY: check
check:
	LIBRARY_PATH="$(BUILD_PATH)" \
	C_INCLUDE_PATH="$(BUILD_PATH)" \
	LD_LIBRARY_PATH="$(BUILD_PATH)" \
	NMEA_PARSER_PATH="$(BUILD_PATH)/nmea/" \
	make unit-tests

.PHONY: install
install:
	install --directory $(PREFIX)/lib/nmea
	install $(LIBRARY_FILE) $(PREFIX)/lib/
ifneq ($(wildcard $(BUILD_PATH)/nmea/*.so),)
	install --directory $(PREFIX)/include/nmea
	install $(BUILD_PATH)/nmea/*.so $(PREFIX)/lib/nmea/
endif
	install $(BUILD_PATH)/*.h $(PREFIX)/include/
	install $(BUILD_PATH)/nmea/*.h $(PREFIX)/include/nmea/
	ldconfig -n $(PREFIX)/lib

.PHONY: clean
clean:
	@echo "Cleaning..."
	@rm -f *.o
	@rm -f tests/*.o
	@rm -f src/nmea/*.o
	@rm -f src/parsers/*.o
	@rm -f utests utests-parse utests-nmea memcheck
	@rm -f $(ALL_DEPEND_FILES)

.PHONY: clean-all
clean-all: clean
	@rm -rf $(BUILD_PATH)

-include $(ALL_DEPEND_FILES)

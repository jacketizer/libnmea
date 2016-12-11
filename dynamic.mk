.PHONY: all
all: nmea parser-libs

src/parsers/%: $(OBJ_PARSER_DEP)
	@mkdir -p $(BUILD_PATH)/nmea
	@echo Building $(patsubst src/parsers/%,lib%.so,$@)...
	$(CC) $(CFLAGS) -s -shared -Isrc/nmea -L$(BUILD_PATH) -I$(BUILD_PATH) -Wl,--no-as-needed,-soname,$(patsubst src/parsers/%,lib%.so,$@) $@.c $(OBJ_PARSER_DEP) -o $(patsubst src/parsers/%,$(BUILD_PATH)/nmea/lib%.so,$@)
	cp $@.h $(BUILD_PATH)/nmea/


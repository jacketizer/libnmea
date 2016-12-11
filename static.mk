define PREFIX_SYMBOL =
	objcopy --redefine-sym $(1)=$(2)_$(1) $(3)
endef

define PREFIX_PARSER_MODULE =
	$(call PREFIX_SYMBOL,init,$(1),$(2))
	$(call PREFIX_SYMBOL,parse,$(1),$(2))
	$(call PREFIX_SYMBOL,set_default,$(1),$(2))
	$(call PREFIX_SYMBOL,allocate_data,$(1),$(2))
	$(call PREFIX_SYMBOL,free_data,$(1),$(2))
endef

.PHONY: all
all: parser-libs static

src/parsers/%: $(OBJ_PARSER_DEP)
	@mkdir -p $(BUILD_PATH)/nmea
	@echo Building $@...
	$(CC) $(CFLAGS) -Isrc/nmea -L$(BUILD_PATH) -I$(BUILD_PATH) $@.c -o $(patsubst src/parsers/%,$(BUILD_PATH)/nmea/%.o,$@)
	$(call PREFIX_PARSER_MODULE,$(patsubst src/parsers/%,%,$@),$(patsubst src/parsers/%,$(BUILD_PATH)/nmea/%.o,$@))
	cp $@.h $(BUILD_PATH)/nmea/

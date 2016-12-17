#include "nmea.h"
#include "parser.h"

/* This file is included from src/nmea/parser.c when */

nmea_parser_module_s parsers[3];

#define DECLARE_PARSER_API(modname) \
	extern int gp##modname##_init(nmea_parser_s *parser); \
	extern int gp##modname##_allocate_data(nmea_parser_s *parser); \
	extern int gp##modname##_set_default(nmea_parser_s *parser); \
	extern int gp##modname##_free_data(nmea_s *data); \
	extern int gp##modname##_parse(nmea_parser_s *parser, char *value, int val_index);

#define PARSER_LOAD(modname, modtype) \
	parser = &(parsers[(int) modtype - 1]); \
	parser->handle = NULL; \
	parser->allocate_data = gp##modname##_allocate_data; \
	parser->set_default = gp##modname##_set_default; \
	parser->free_data = gp##modname##_free_data; \
	parser->parse = gp##modname##_parse; \
	if (-1 == gp##modname##_init((nmea_parser_s *) parser)) { \
		return -1; \
	} \

#define PARSER_COUNT (sizeof parsers / sizeof (parsers[0]))

DECLARE_PARSER_API(gll)
DECLARE_PARSER_API(gga)
DECLARE_PARSER_API(rmc)

nmea_parser_module_s *
nmea_init_parser(const char *filename)
{
	/* This function intentionally returns NULL */
	return NULL;
}

int
nmea_load_parsers()
{
	nmea_parser_module_s *parser;

	PARSER_LOAD(gll, NMEA_GPGLL);
	PARSER_LOAD(gga, NMEA_GPGGA);
	PARSER_LOAD(rmc, NMEA_GPRMC);

	return PARSER_COUNT;
}

void
nmea_unload_parsers()
{
	/* This function body is intentionally left empty,
	     because there is no dynamic memory allocations. */
}

nmea_parser_module_s *
nmea_get_parser_by_type(nmea_t type)
{
	return &(parsers[(int) type - 1]);
}

nmea_parser_module_s *
nmea_get_parser_by_sentence(const char *sentence)
{
	int i;
	nmea_parser_s *p;

	for (i = 0; i < PARSER_COUNT; i++) {
		p = (nmea_parser_s *) (parsers + i);
		if (NULL == p->type_word) {
			continue;
		}

		if (0 == strncmp(sentence + 1, p->type_word, NMEA_PREFIX_LENGTH)) {
			return &(parsers[i]);
		}
	}

	return (nmea_parser_module_s *) NULL;
}

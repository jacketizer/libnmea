/* This file is included from src/nmea/parser.c when */

#define DECLARE_PARSER_API(modname) \
	extern int gp##modname##_init(nmea_parser_s *parser); \
	extern int gp##modname##_allocate_data(nmea_parser_s *parser); \
	extern int gp##modname##_set_default(nmea_parser_s *parser); \
	extern int gp##modname##_free_data(nmea_s *data); \
	extern int gp##modname##_parse(nmea_parser_s *parser, char *value, int val_index);

#define PARSER_LOAD(modname) \
	parser = malloc(sizeof (nmea_parser_module_s)); \
	if (NULL == parser) { \
		return -1; \
	} \
	parser->handle = NULL; \
	parser->allocate_data = gp##modname##_allocate_data; \
	parser->set_default = gp##modname##_set_default; \
	parser->free_data = gp##modname##_free_data; \
	parser->parse = gp##modname##_parse; \
	if (-1 == gp##modname##_init((nmea_parser_s *) parser)) { \
		return -1; \
	} \
	parsers[(int) parser->parser.type - 1] = parser;

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

	/* Allocate parsers array */
	parsers = malloc((sizeof (nmea_parser_module_s *)) * 10);
	if (NULL == parsers) {
		return -1;
	}
	memset(parsers, 0, (sizeof (nmea_parser_module_s *)) * 10);

	/* GPGLL */
	PARSER_LOAD(gll);

	/* GPGGA */
	PARSER_LOAD(gga);

	/* GPRMC */
	PARSER_LOAD(rmc);

	n_parsers = 3;

	return 3;
}

void
nmea_unload_parsers()
{
	/* This function body is intentionally left empty */
}

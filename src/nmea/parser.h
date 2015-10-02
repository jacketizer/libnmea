#ifndef INC_NMEA_PARSER_H
#define INC_NMEA_PARSER_H

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <dlfcn.h>
#include "nmea.h"
#include "parser_types.h"

typedef nmea_s * (*allocate_data_f) ();
typedef void (*set_default_f) (nmea_s *);
typedef void (*free_data_f) (nmea_s *);
typedef int (*parse_f) (char *, int, nmea_s *);

typedef struct {
  nmea_parser_s parser;
	int errors;

  /* Functions */
	allocate_data_f allocate_data;
	set_default_f set_default;
	free_data_f free_data;
	parse_f parse;
} nmea_parser_module_s;

typedef int (*init_f) (nmea_parser_s *);

nmea_parser_module_s *parsers[12];

/**
 * Load the parser libs into array.
 *
 * Returns 0 on success, or -1 if an error occurs.
 */
int nmea_load_parsers();

/**
 * Initiate a parser.
 *
 * Returns a sentence parser struct, or (nmea_parser_module_s *) NULL if an error occurs.
 */
nmea_parser_module_s * nmea_init_parser(const char *filename);

/**
 * Get a parser for a sentence type.
 *
 * Returns the sentence parser struct, should be checked for NULL.
 */
nmea_parser_module_s * nmea_get_parser(nmea_t type);

#endif  /* INC_NMEA_PARSER_H */

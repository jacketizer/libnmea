#ifndef INC_NMEA_PARSER_TYPES_H
#define INC_NMEA_PARSER_TYPES_H

#include "nmea.h"

typedef struct {
	nmea_t type;
	char type_word[5];
	nmea_s *data;
} nmea_parser_s;

extern int init(nmea_parser_s *parser);
extern int allocate_data(nmea_parser_s *parser);
extern int set_default(nmea_parser_s *parser);
extern int free_data(nmea_s *data);
extern int parse(nmea_parser_s *parser, char *value, int val_index);

#define NMEA_PARSER_PREFIX(parser, type_prefix) strncpy(parser->type_word, type_prefix, NMEA_PREFIX_LENGTH)
#define NMEA_PARSER_TYPE(parser, nmea_type) parser->type = nmea_type

#endif  /* INC_NMEA_PARSER_TYPES_H */

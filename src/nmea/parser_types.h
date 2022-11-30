#ifndef INC_NMEA_PARSER_TYPES_H
#define INC_NMEA_PARSER_TYPES_H

#include "nmea.h"

typedef struct {
	nmea_t type;
	/* For compatibility with older versions, type_word also contains the talker ID (e.g. "GP".)
	 * However the talker ID is not taken account when looking for a parser. For example, the parser
	 * with type_word="GPRMC" will also be used for a "GNRMC" sentence.
	 */
	char type_word[NMEA_PREFIX_LENGTH];
	nmea_s *data;
} nmea_parser_s;

#define NMEA_PARSER_PREFIX(parser, type_prefix) memcpy(parser->type_word, type_prefix, NMEA_PREFIX_LENGTH)
#define NMEA_PARSER_TYPE(parser, nmea_type) parser->type = nmea_type

#endif  /* INC_NMEA_PARSER_TYPES_H */

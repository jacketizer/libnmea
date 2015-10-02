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
extern int free_data(nmea_parser_s *parser);
extern int parse(nmea_parser_s *parser, char *value, int val_index);

#endif  /* INC_NMEA_PARSER_TYPES_H */

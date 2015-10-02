#ifndef INC_NMEA_PARSER_TYPES_H
#define INC_NMEA_PARSER_TYPES_H

#include "nmea.h"

typedef struct {
	nmea_t type;
	nmea_s *data;
} nmea_parser_s;

extern int init(nmea_parser_s *parser);
extern nmea_s * allocate_data();
extern void set_default(nmea_s *nmea_data);
extern void free_data(nmea_s *nmea_data);
extern int parse(char *value, int val_index, nmea_s *nmea_data);

#endif  /* INC_NMEA_PARSER_TYPES_H */

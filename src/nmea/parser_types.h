#ifndef INC_NMEA_PARSER_TYPES_H
#define INC_NMEA_PARSER_TYPES_H

#include "nmea.h"

typedef nmea_s * (*allocate_data_f) ();
typedef void (*set_default_f) (nmea_s *);
typedef void (*free_data_f) (nmea_s *);
typedef int (*parse_f) (char *, int, nmea_s *);

typedef struct {
	nmea_t type;
	nmea_s *data;
	int errors;
	allocate_data_f allocate_data;
	set_default_f set_default;
	free_data_f free_data;
	parse_f parse;
} nmea_sentence_parser_s;

typedef int (*init_f) (nmea_sentence_parser_s *);

extern init_f init(nmea_sentence_parser_s *parser);
extern allocate_data_f allocate_data();
extern set_default_f set_default(nmea_s *nmea_data);
extern free_data_f free_data(nmea_s *nmea_data);
extern parse_f parse(char *value, int val_index, nmea_s *nmea_data);

#endif  /* INC_NMEA_PARSER_TYPES_H */

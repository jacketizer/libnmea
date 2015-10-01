#ifndef INC_NMEA_SENTENCE_H
#define INC_NMEA_SENTENCE_H

#include "../nmea.h"

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

#endif  /* INC_NMEA_SENTENCE_H */

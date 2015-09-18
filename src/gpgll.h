#ifndef INC_NMEA_GPGLL_H
#define INC_NMEA_GPGLL_H

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "parse.h"

typedef struct {
	nmea_s super;
	nmea_position longitude;
	nmea_position latitude;
	struct tm time;
} nmea_gpgll_s;

/* Value indexes */
#define NMEA_GPGLL_LATITUDE		0
#define NMEA_GPGLL_LATITUDE_CARDINAL	1
#define NMEA_GPGLL_LONGITUDE		2
#define NMEA_GPGLL_LONGITUDE_CARDINAL	3
#define NMEA_GPGLL_TIME			4

int nmea_gpgll_init(nmea_sentence_parser_s *parser);
void nmea_gpgll_default(nmea_s *data);
void nmea_gpgll_free(nmea_s *nmea_data);
int nmea_gpgll_parse(char *value, int val_index, nmea_s *nmea_data);

#endif  /* INC_NMEA_GPGLL_H */

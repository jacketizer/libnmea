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

#define NMEA_GPGLL_N_VALUES		4 // Minimum number of values in sentence
#define NMEA_GPGLL_LATITUDE		0
#define NMEA_GPGLL_LATITUDE_CARDINAL	1
#define NMEA_GPGLL_LONGITUDE		2
#define NMEA_GPGLL_LONGITUDE_CARDINAL	3
#define NMEA_GPGLL_TIME			4

nmea_gpgll_s * nmea_gpgll_parse(char **values, int length);

#endif  /* INC_NMEA_GPGLL_H */

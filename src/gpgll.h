#ifndef INC_NMEA_GPGLL_H
#define INC_NMEA_GPGLL_H

#include <stdlib.h>
#include <string.h>

typedef struct {
	nmea_t type;
	nmea_position logitude;
	nmea_position latitude;
} nmea_gpgll_s;

#define NMEA_GPGLL_N_VALUES	4
#define NMEA_GPGLL_LATITUDE	0
#define NMEA_GPGLL_LATITUDE_NS	1
#define NMEA_GPGLL_LONGITUDE	2
#define NMEA_GPGLL_LONGITUDE_EW	3

nmea_gpgll_s * nmea_gpgll_parse(const char *sentence, int length);

#endif  /* INC_NMEA_GPGLL_H */

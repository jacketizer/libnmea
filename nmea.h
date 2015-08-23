#ifndef INC_NMEA_H
#define INC_NMEA_H

#include <stdlib.h>
#include <string.h>

/* nmea sentence max length */
#define NMEA_MAX_LENGTH		50

/* NMEA sentence prefixes */
#define NMEA_PREFIX_LENGTH	5
#define NMEA_PREFIX_GPGLL	"GPGLL"	// Geographic Position, Latitude/Longitude

/* NMEA sentence types */
#define NMEA_GPGLL_T		1

typedef unsigned int nmea_t;

nmea_t nmea_get_type(const char *sentence);
char nmea_get_checksum(char *sentence);

#endif  /* INC_NMEA_H */

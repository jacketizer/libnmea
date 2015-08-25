#ifndef INC_NMEA_GPGLL_H
#define INC_NMEA_GPGLL_H

#include "nmea.h"

typedef struct {
	nmea_t type;
	nmea_position logitude;
	nmea_position latitude;
} nmea_gpgll_s;

nmea_gpgll_s nmea_gpgll_parse(const char *sentence);

#endif  /* INC_NMEA_H */

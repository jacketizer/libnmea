#ifndef INC_NMEA_GPGLL_H
#define INC_NMEA_GPGLL_H

#include <stdlib.h>
#include <string.h>

typedef struct {
	nmea_t type;
	nmea_position logitude;
	nmea_position latitude;
} nmea_gpgll_s;

#define NMEA_GPGLL_N_VALUES 7

nmea_gpgll_s nmea_gpgll_parse(const char *sentence, int length);

#endif  /* INC_NMEA_H */

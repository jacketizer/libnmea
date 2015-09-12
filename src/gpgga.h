#ifndef INC_NMEA_GPGGA_H
#define INC_NMEA_GPGGA_H

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "parse.h"

typedef struct {
	nmea_s super;
	struct tm time;
	nmea_position longitude;
	nmea_position latitude;
	int n_satellites;
	int altitude;
	char altitude_unit;
} nmea_gpgga_s;

#define NMEA_GPGGA_N_VALUES		5 // Minimum number of values in sentence
#define NMEA_GPGGA_TIME			0
#define NMEA_GPGGA_LATITUDE		1
#define NMEA_GPGGA_LATITUDE_CARDINAL	2
#define NMEA_GPGGA_LONGITUDE		3
#define NMEA_GPGGA_LONGITUDE_CARDINAL	4
#define NMEA_GPGGA_N_SATELLITES		6
#define NMEA_GPGGA_ALTITUDE		8
#define NMEA_GPGGA_ALTITUDE_UNIT	9

nmea_gpgga_s * nmea_gpgga_parse(char **values, int length);

#endif  /* INC_NMEA_GPGGA_H */

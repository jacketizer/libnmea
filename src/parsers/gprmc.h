#ifndef INC_NMEA_GPRMC_H
#define INC_NMEA_GPRMC_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <nmea.h>

typedef struct {
	nmea_s base;
	nmea_position longitude;
	nmea_position latitude;
	double speed;
	struct tm time;
	bool valid;
} nmea_gprmc_s;

/* Value indexes */
#define NMEA_GPRMC_LATITUDE		2
#define NMEA_GPRMC_LATITUDE_CARDINAL	3
#define NMEA_GPRMC_LONGITUDE		4
#define NMEA_GPRMC_LONGITUDE_CARDINAL	5
#define NMEA_GPRMC_TIME			0
#define NMEA_GPRMC_DATE			8
#define NMEA_GPRMC_SPEED		6
#define NMEA_GPRMC_STATUS		1
#endif  /* INC_NMEA_GPRMC_H */

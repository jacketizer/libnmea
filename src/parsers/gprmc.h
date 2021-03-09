#ifndef INC_NMEA_GPRMC_H
#define INC_NMEA_GPRMC_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <nmea.h>

typedef struct {
	nmea_s base;
	struct tm date_time;
	nmea_position longitude;
	nmea_position latitude;
	double gndspd_knots;
	double track_deg;
	double magvar_deg;
	nmea_cardinal_t magvar_cardinal;
	//The direction of the magnetic variation determines whether or not it
	//is additive - Easterly means subtract magvar_deg from track_deg and
	//westerly means add magvar_deg to track_deg for the correct course.
	bool valid;
} nmea_gprmc_s;

/* Value indexes */
#define NMEA_GPRMC_TIME			0
#define NMEA_GPRMC_STATUS		1
#define NMEA_GPRMC_LATITUDE		2
#define NMEA_GPRMC_LATITUDE_CARDINAL	3
#define NMEA_GPRMC_LONGITUDE		4
#define NMEA_GPRMC_LONGITUDE_CARDINAL	5
#define NMEA_GPRMC_GNDSPD_KNOTS		6
#define NMEA_GPRMC_TRUECOURSE_DEG	7
#define NMEA_GPRMC_DATE			8
#define NMEA_GPRMC_MAGVAR_DEG		9
#define NMEA_GPRMC_MAGVAR_CARDINAL	10
#endif  /* INC_NMEA_GPRMC_H */

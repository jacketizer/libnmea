#ifndef INC_NMEA_GPVTG_H
#define INC_NMEA_GPVTG_H

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <nmea.h>

typedef struct {
	nmea_s base;
	double track_deg;
	double gndspd_knots;
	double gndspd_kmph;
} nmea_gpvtg_s;

/* Value indexes */
#define NMEA_GPVTG_TRACKGOOD 0
#define NMEA_GPVTG_GNDSPD_KNOTS 4
#define NMEA_GPVTG_GNDSPD_KMPH 6

#endif  /* INC_NMEA_GPVTG_H */

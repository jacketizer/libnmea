#ifndef INC_NMEA_GPTXT_H
#define INC_NMEA_GPTXT_H

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <nmea.h>

typedef struct {
	nmea_s base;
	int id_00;
	int id_01;
	int id_02;
	char text[64];
} nmea_gptxt_s;

/* Value indexes */
#define NMEA_GPTXT_ID00 0
#define NMEA_GPTXT_ID01 1
#define NMEA_GPTXT_ID02 2
#define NMEA_GPTXT_TEXT 3

#endif  /* INC_NMEA_GPTXT_H */

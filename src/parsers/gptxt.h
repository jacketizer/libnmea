#ifndef INC_NMEA_GPTXT_H
#define INC_NMEA_GPTXT_H

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <nmea.h>

/* Value indexes */
#define NMEA_GPTXT_ID00 0
#define NMEA_GPTXT_ID01 1
#define NMEA_GPTXT_ID02 2
#define NMEA_GPTXT_TEXT 3
/* Text field length */
#define NMEA_GPTXT_TEXT_SIZE 64

typedef struct {
	nmea_s base;
	int id_00;
	int id_01;
	int id_02;
	char text[NMEA_GPTXT_TEXT_SIZE];
} nmea_gptxt_s;

#endif  /* INC_NMEA_GPTXT_H */

#ifndef INC_NMEA_PARSE_H
#define INC_NMEA_PARSE_H

#include <stdlib.h>
#include <string.h>
#include "nmea.h"

typedef struct {
	double minutes;
	int degrees;
	char dir;
} nmea_position;

/* returns number of tokens */
int nmea_sentence_split(char *sentence, int length, char **values);

/* return gps position longitude or latitude */
int nmea_position_parse(char *s, nmea_position *pos);

#endif  /* INC_NMEA_PARSE_H */

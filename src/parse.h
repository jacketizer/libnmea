#ifndef INC_NMEA_PARSE_H
#define INC_NMEA_PARSE_H

#include <stdlib.h>
#include <string.h>
#include "nmea.h"

typedef struct {
	double minutes;
	int degrees;
	char cardinal;
} nmea_position;

/**
 * Splits an NMEA sentence by comma.
 *
 * sentence is the string to split, will be manipulated.
 * length is the char length of the sentence string.
 * values is a char pointer array that will be filled with pointers to the
 * splitted values in the sentence string.
 */
int nmea_sentence_split(char *sentence, int length, char **values);

/* Return gps position longitude or latitude */
int nmea_position_parse(char *s, nmea_position *pos);

/* Returns a cardinal direction */
char nmea_cardinal_direction_parse(char *s);

#endif  /* INC_NMEA_PARSE_H */

#ifndef INC_NMEA_PARSE_H
#define INC_NMEA_PARSE_H

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "nmea.h"

#define NMEA_TIME_FORMAT	"%H%M%S"
#define NMEA_TIME_FORMAT_LEN	6

typedef struct {
	int (*parse)(char **, int, nmea_s *nmea_data);
	nmea_s *data;
} nmea_sentence_parser_s;

/**
 * Splits an NMEA sentence by comma.
 *
 * sentence is the string to split, will be manipulated.
 * length is the char length of the sentence string.
 * values is a char pointer array that will be filled with pointers to the
 * splitted values in the sentence string.
 */
int nmea_sentence_split(char *sentence, int length, char **values);

/* Check if a value is supplied and set */
int nmea_value_is_set(char *value);

/* Return gps position longitude or latitude */
int nmea_position_parse(char *s, nmea_position *pos);

/* Returns a cardinal direction */
nmea_cardinal_t nmea_cardinal_direction_parse(char *s);

/* Parse time form a string in format: hhmmss */
int nmea_time_parse(char *s, struct tm *time);

#endif  /* INC_NMEA_PARSE_H */

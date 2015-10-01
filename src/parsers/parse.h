#ifndef INC_NMEA_PARSE_H
#define INC_NMEA_PARSE_H

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../nmea/nmea.h"

#define NMEA_TIME_FORMAT	"%H%M%S"
#define NMEA_TIME_FORMAT_LEN	6

/* Return gps position longitude or latitude */
int nmea_position_parse(char *s, nmea_position *pos);

/* Returns a cardinal direction */
nmea_cardinal_t nmea_cardinal_direction_parse(char *s);

/* Parse time form a string in format: hhmmss */
int nmea_time_parse(char *s, struct tm *time);

#endif  /* INC_NMEA_PARSE_H */

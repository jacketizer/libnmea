#include "parse.h"

int
nmea_value_is_set(char *value)
{
	if (NULL == value || '\0' == *value) {
		return -1;
	}

	return 0;
}

int
nmea_position_parse(char *s, nmea_position *pos)
{
	pos->degrees = 0;
	pos->minutes = 0;

	if (s == NULL || *s == '\0') {
		return -1;
	}

	// decimal minutes
	char *cursor = memchr(s, '.', strlen(s));
	if (NULL == cursor) {
		return -1;
	}

	cursor -= 2; // minutes starts 2 digits before dot
	pos->minutes = atof(cursor);
	*cursor = '\0';

	// integer degrees
	cursor = s;
	pos->degrees = atoi(cursor);

	return 0;
}

nmea_cardinal_t
nmea_cardinal_direction_parse(char *s)
{
	if (s == NULL || *s == '\0') {
		return NMEA_CARDINAL_DIR_UNKNOWN;
	}

	switch (*s) {
		case NMEA_CARDINAL_DIR_NORTH:
			return NMEA_CARDINAL_DIR_NORTH;
		case NMEA_CARDINAL_DIR_EAST:
			return NMEA_CARDINAL_DIR_EAST;
		case NMEA_CARDINAL_DIR_SOUTH:
			return NMEA_CARDINAL_DIR_SOUTH;
		case NMEA_CARDINAL_DIR_WEST:
			return NMEA_CARDINAL_DIR_WEST;
		default:
			break;
	}

	return NMEA_CARDINAL_DIR_UNKNOWN;
}

int
nmea_time_parse(char *s, struct tm *time)
{
	char *rv;
	memset(time, 0, sizeof(struct tm));

	if (s == NULL || *s == '\0') {
		return -1;
	}

	rv = strptime(s, NMEA_TIME_FORMAT, time);
	if (NULL == rv || (int) (rv - s) != NMEA_TIME_FORMAT_LEN) {
		return -1;
	}

	return 0;
}

#include "parse.h"

int
nmea_position_parse(char *s, nmea_position *pos)
{
	char *cursor;

	pos->degrees = 0;
	pos->minutes = 0;

	if (s == NULL || *s == '\0') {
		return -1;
	}

	/* decimal minutes */
	if (NULL == (cursor = strchr(s, '.'))) {
		return -1;
	}

	/* minutes starts 2 digits before dot */
	cursor -= 2;
	pos->minutes = atof(cursor);
	*cursor = '\0';

	/* integer degrees */
	cursor = s;
	pos->degrees = atoi(cursor);

	return 0;
}

nmea_cardinal_t
nmea_cardinal_direction_parse(char *s)
{
	if (NULL == s || '\0'== *s) {
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

	memset(time, 0, sizeof (struct tm));

	if (s == NULL || *s == '\0') {
		return -1;
	}

	rv = strptime(s, NMEA_TIME_FORMAT, time);
	if (NULL == rv || (int) (rv - s) != NMEA_TIME_FORMAT_LEN) {
		return -1;
	}

	return 0;
}

int
nmea_date_parse(char *s, struct tm *time)
{
	char *rv;

	// Assume it has been already cleared
	// memset(time, 0, sizeof (struct tm));

	if (s == NULL || *s == '\0') {
		return -1;
	}

	rv = strptime(s, NMEA_DATE_FORMAT, time);
	if (NULL == rv || (int) (rv - s) != NMEA_DATE_FORMAT_LEN) {
		return -1;
	}

	return 0;
}
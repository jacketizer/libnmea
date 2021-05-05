#include "parse.h"

#define TM_YEAR_START 1900
#define RMC_YEAR_START 2000

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
	uint32_t x;

	if (s == NULL || *s == '\0') {
		return -1;
	}

	x = strtoul(s, &rv, 10);
	time->tm_hour = x / 10000;
	time->tm_min = (x % 10000) / 100;
	time->tm_sec = x % 100;
	if (time->tm_hour > 23 || time->tm_min > 59 || time->tm_sec > 59 || (int) (rv - s) < NMEA_TIME_FORMAT_LEN) {
		return -1;
	}
	if (*rv == '.') {
		/* TODO There is a sub-second field. */
	}

	return 0;
}

int
nmea_date_parse(char *s, struct tm *date)
{
	char *rv;
	uint32_t x;

	if (s == NULL || *s == '\0') {
		return -1;
	}

	x = strtoul(s, &rv, 10);
	date->tm_mday = x / 10000;
	date->tm_mon = ((x % 10000) / 100) - 1;
	date->tm_year = x % 100;

	// Normalize tm_year according to C standard library
	if (date->tm_year > 1900) { // ZDA message case
		date->tm_year -= TM_YEAR_START;
	} else { // RMC message case
		date->tm_year += (RMC_YEAR_START - TM_YEAR_START);
	}

	return 0;
}

#include "nmea.h"

nmea_t
nmea_get_type(const char *sentence)
{
	if (0 == strncmp(sentence + 1, NMEA_PREFIX_GPGLL, NMEA_PREFIX_LENGTH)) {
		return (nmea_t) NMEA_GPGLL_T;
	}

	return (nmea_t) 0;
}

char
nmea_get_checksum(char *sentence)
{
	char *n = sentence + 1;
	char checksum = 0;

	while (*n != '*' && n - sentence < NMEA_MAX_LENGTH) {
		checksum = checksum ^ *n;
		n++;
	}

	return checksum;
}

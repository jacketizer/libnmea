#include "nmea.h"
#include "gpgll.h"

nmea_gpgll_s *
nmea_gpgll_parse(const char *sentence, int length)
{
	nmea_gpgll_s data;

	// Parse sentence to struct.
	// .........................
	// Split values by comma.
	// Find dot, extract minutes by taking 2 digits before the dot
	// 	and then the rest until the comma.
	// Take the remaing digits in front of minutes and save it as degrees.
	// Get direction (N, S, E, W).

	
	char **values = malloc(200);
	int n_vals = _nmea_split_sentence(sentence, length, values);

	if (NMEA_GPGLL_N_VALUES != n_vals) {
		return NULL;
	}

	/* LATITUDE */

	nmea_position lat_pos;
	if (-1 == _nmea_get_position(values[NMEA_GPGLL_LATITUDE], &lat_pos)) {
		return NULL;
	}
	printf("Latitude:\n");
	printf("  Degrees: %d\n", lat_pos.degrees);
	printf("  Minutes: %f\n", lat_pos.minutes);

	/* LONGITUDE */

	nmea_position long_pos;
	if (-1 == _nmea_get_position(values[NMEA_GPGLL_LONGITUDE], &long_pos)) {
		return NULL;
	}
	printf("Longitude:\n");
	printf("  Degrees: %d\n", long_pos.degrees);
	printf("  Minutes: %f\n", long_pos.minutes);

	return &data;
}

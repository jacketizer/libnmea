#include "gpgll.h"

nmea_gpgll_s *
nmea_gpgll_parse(const char *sentence, int length)
{
	nmea_gpgll_s *data = malloc(sizeof(nmea_gpgll_s));
	if (NULL == data) {
		return NULL;
	}

	// Parse sentence to struct.
	// .........................
	// Split values by comma.
	// Find dot, extract minutes by taking 2 digits before the dot
	// 	and then the rest until the comma.
	// Take the remaing digits in front of minutes and save it as degrees.
	// Get direction (N, S, E, W).

	char **values = malloc(200);
	int n_vals = nmea_sentence_split(sentence, length, values);

	if (NMEA_GPGLL_N_VALUES > n_vals) {
		return NULL;
	}

	/* LATITUDE */
	memset(&data->latitude, 0, sizeof(nmea_position));
	if (0 == nmea_position_parse(values[NMEA_GPGLL_LATITUDE], &data->latitude)) {
		// report error?
	}

	/* LONGITUDE */
  	memset(&data->longitude, 0, sizeof(nmea_position));
	if (0 == nmea_position_parse(values[NMEA_GPGLL_LONGITUDE], &data->longitude)) {
		// report error?
	}

	return data;
}

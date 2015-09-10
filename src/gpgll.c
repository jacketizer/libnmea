#include "gpgll.h"

nmea_gpgll_s *
nmea_gpgll_parse(char *sentence, int length)
{
	int n_vals;
	char *values[200];
	char *dir;

	nmea_gpgll_s *data = malloc(sizeof(nmea_gpgll_s));
	if (NULL == data) {
		return NULL;
	}

	n_vals = nmea_sentence_split(sentence, length, values);
	if (NMEA_GPGLL_N_VALUES > n_vals) {
		free(data);
		return NULL;
	}

	/* LATITUDE */
	memset(&data->latitude, 0, sizeof(nmea_position));
	if (0 == nmea_position_parse(values[NMEA_GPGLL_LATITUDE], &data->latitude)) {
		// report error?
	}

	/* North or South */
	dir = values[NMEA_GPGLL_LATITUDE_CARDINAL];
	if (NULL == dir || (NMEA_CARDINAL_DIR_NORTH != *dir && NMEA_CARDINAL_DIR_SOUTH != *dir)) {
		free(data);
		return NULL;
	}
	data->latitude.cardinal = *dir;

	/* LONGITUDE */
  	memset(&data->longitude, 0, sizeof(nmea_position));
	if (0 == nmea_position_parse(values[NMEA_GPGLL_LONGITUDE], &data->longitude)) {
		// report error?
	}

	/* East or West */
	dir = values[NMEA_GPGLL_LONGITUDE_CARDINAL];
	if (NULL == dir || (NMEA_CARDINAL_DIR_EAST != *dir && NMEA_CARDINAL_DIR_WEST != *dir)) {
		free(data);
		return NULL;
	}
	data->longitude.cardinal = *dir;

	return data;
}

#include "gpgll.h"

nmea_gpgll_s *
nmea_gpgll_parse(char *sentence, int length)
{
	int n_vals;
	char *values[200];

	nmea_gpgll_s *data = malloc(sizeof(nmea_gpgll_s));
	if (NULL == data) {
		return NULL;
	}
	memset(data, 0, sizeof(nmea_gpgll_s));

	n_vals = nmea_sentence_split(sentence, length, values);
	if (NMEA_GPGLL_N_VALUES > n_vals) {
		free(data);
		return NULL;
	}

	/* LATITUDE */
	if (-1 == nmea_position_parse(values[NMEA_GPGLL_LATITUDE], &data->latitude)) {
		// report error
		data->error = 1;
	}

	/* Get cardinal direction */
	data->latitude.cardinal = nmea_cardinal_direction_parse(values[NMEA_GPGLL_LATITUDE_CARDINAL]);
	if (NMEA_CARDINAL_DIR_UNKNOWN == data->latitude.cardinal) {
		// report error
		data->error = 1;
	}

	/* LONGITUDE */
	if (-1 == nmea_position_parse(values[NMEA_GPGLL_LONGITUDE], &data->longitude)) {
		// report error
		data->error = 1;
	}

	/* Get cardinal direction */
	data->longitude.cardinal = nmea_cardinal_direction_parse(values[NMEA_GPGLL_LONGITUDE_CARDINAL]);
	if (NMEA_CARDINAL_DIR_UNKNOWN == data->longitude.cardinal) {
		// report error
		data->error = 1;
	}

	return data;
}

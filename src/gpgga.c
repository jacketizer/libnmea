#include "gpgga.h"

nmea_gpgga_s *
nmea_gpgga_parse(char *sentence, int length)
{
	int n_vals;
	char *values[200];
	nmea_s *nmea_data;

	nmea_gpgga_s *data = malloc(sizeof(nmea_gpgga_s));
	if (NULL == data) {
		return NULL;
	}
	memset(data, 0, sizeof(nmea_gpgga_s));
	nmea_data = (nmea_s *) data;

	n_vals = nmea_sentence_split(sentence, length, values);
	if (NMEA_GPGGA_N_VALUES > n_vals) {
		free(data);
		return NULL;
	}

	/* Parse time */
	if (-1 == nmea_time_parse(values[NMEA_GPGGA_TIME], &data->time)) {
		nmea_data->error = 1;
	}

	/* LATITUDE */
	if (-1 == nmea_position_parse(values[NMEA_GPGGA_LATITUDE], &data->latitude)) {
		// report error
		nmea_data->error = 1;
	}

	/* Get cardinal direction */
	data->latitude.cardinal = nmea_cardinal_direction_parse(values[NMEA_GPGGA_LATITUDE_CARDINAL]);
	if (NMEA_CARDINAL_DIR_UNKNOWN == data->latitude.cardinal) {
		// report error
		nmea_data->error = 1;
	}

	/* LONGITUDE */
	if (-1 == nmea_position_parse(values[NMEA_GPGGA_LONGITUDE], &data->longitude)) {
		// report error
		nmea_data->error = 1;
	}

	/* Get cardinal direction */
	data->longitude.cardinal = nmea_cardinal_direction_parse(values[NMEA_GPGGA_LONGITUDE_CARDINAL]);
	if (NMEA_CARDINAL_DIR_UNKNOWN == data->longitude.cardinal) {
		// report error
		nmea_data->error = 1;
	}

	/* Parse number of satellies, if supplied... */
	if (0 == nmea_value_is_set(values, n_vals, NMEA_GPGGA_N_SATELLITES)) {
		data->n_satellites = atoi(values[NMEA_GPGGA_N_SATELLITES]);
	}

	/* Parse altitude, if supplied... */
	if (0 == nmea_value_is_set(values, n_vals, NMEA_GPGGA_ALTITUDE)) {
		data->altitude = atoi(values[NMEA_GPGGA_ALTITUDE]);
		data->altitude_unit = *values[NMEA_GPGGA_ALTITUDE_UNIT];
	}

	return data;
}

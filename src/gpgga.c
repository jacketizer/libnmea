#include "gpgga.h"

nmea_gpgga_s *
nmea_gpgga_parse(char **values, int length)
{
	nmea_gpgga_s *data;
	nmea_s *nmea_data;

	/* Allocate data struct */
	data = malloc(sizeof(nmea_gpgga_s));
	if (NULL == data) {
		return NULL;
	}
	memset(data, 0, sizeof(nmea_gpgga_s));
	nmea_data = (nmea_s *) data;

	/* Enough values in sentence? */
	if (NMEA_GPGGA_N_VALUES > length) {
		free(data);
		return NULL;
	}

	int i = 0;
	char *value;
	while (i < length) {
		value = values[i];

		switch (i) {
			case NMEA_GPGGA_TIME:
				/* Parse time */
				if (-1 == nmea_time_parse(value, &data->time)) {
					nmea_data->error = 1;
				}
				break;

			case NMEA_GPGGA_LATITUDE:
				/* Parse latitude */
				if (-1 == nmea_position_parse(value, &data->latitude)) {
					nmea_data->error = 1;
				}
				break;

			case NMEA_GPGGA_LATITUDE_CARDINAL:
				/* Parse cardinal direction */
				data->latitude.cardinal = nmea_cardinal_direction_parse(value);
				if (NMEA_CARDINAL_DIR_UNKNOWN == data->latitude.cardinal) {
					nmea_data->error = 1;
				}
				break;

			case NMEA_GPGGA_LONGITUDE:
				/* Parse longitude */
				if (-1 == nmea_position_parse(value, &data->longitude)) {
					nmea_data->error = 1;
				}
				break;

			case NMEA_GPGGA_LONGITUDE_CARDINAL:
				/* Parse cardinal direction */
				data->longitude.cardinal = nmea_cardinal_direction_parse(value);
				if (NMEA_CARDINAL_DIR_UNKNOWN == data->longitude.cardinal) {
					nmea_data->error = 1;
				}
				break;

			case NMEA_GPGGA_N_SATELLITES:
				/* Parse number of satellies */
				if (0 == nmea_value_is_set(value)) {
					data->n_satellites = atoi(value);
				}
				break;

			case NMEA_GPGGA_ALTITUDE:
				/* Parse altitude */
				if (0 == nmea_value_is_set(value)) {
					data->altitude = atoi(value);
				}
				break;

			case NMEA_GPGGA_ALTITUDE_UNIT:
				/* Parse altitude unit */
				if (0 == nmea_value_is_set(value)) {
					data->altitude_unit = *value;
				}
				break;

			default:
				break;
		}

		i++;
	}

	return data;
}

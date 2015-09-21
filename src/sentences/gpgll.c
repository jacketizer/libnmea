#include "gpgll.h"

init_f
init(nmea_sentence_parser_s *parser)
{
	/* Declare what type to parse */
	parser->type = NMEA_GPGLL;

	/* Allocate data struct */
	nmea_s *data;
	data = malloc(sizeof(nmea_gpgll_s));
	if (NULL == data) {
		return -1;
	}
	memset(data, 0, sizeof(nmea_gpgll_s));
	parser->data = data;

	return 0;
}

set_default_f
set_default(nmea_s *nmea_data)
{
	printf("HEJSAN");
	memset(nmea_data, 0, sizeof(nmea_gpgll_s));
}

free_data_f
free_data(nmea_s *nmea_data)
{
	free(nmea_data);
}

parse_f
parse(char *value, int val_index, nmea_s *nmea_data)
{
	nmea_gpgll_s *data = (nmea_gpgll_s *) nmea_data;

	switch (val_index) {
		case NMEA_GPGLL_TIME:
			/* Parse time */
			if (-1 == nmea_time_parse(value, &data->time)) {
				return -1;
			}
			break;

		case NMEA_GPGLL_LATITUDE:
			/* Parse latitude */
			if (-1 == nmea_position_parse(value, &data->latitude)) {
				return -1;
			}
			break;

		case NMEA_GPGLL_LATITUDE_CARDINAL:
			/* Parse cardinal direction */
			data->latitude.cardinal = nmea_cardinal_direction_parse(value);
			if (NMEA_CARDINAL_DIR_UNKNOWN == data->latitude.cardinal) {
				return -1;
			}
			break;

		case NMEA_GPGLL_LONGITUDE:
			/* Parse longitude */
			if (-1 == nmea_position_parse(value, &data->longitude)) {
				return -1;
			}
			break;

		case NMEA_GPGLL_LONGITUDE_CARDINAL:
			/* Parse cardinal direction */
			data->longitude.cardinal = nmea_cardinal_direction_parse(value);
			if (NMEA_CARDINAL_DIR_UNKNOWN == data->longitude.cardinal) {
				return -1;
			}
			break;

		default:
			break;
	}

	return 0;
}

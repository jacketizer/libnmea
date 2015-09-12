#include "gpgll.h"

void
nmea_gpgll_init(nmea_sentence_parser_s *parser)
{
	memset(parser, 0, sizeof(nmea_sentence_parser_s));
	parser->parse = &nmea_gpgll_parse;
}

nmea_gpgll_s *
nmea_gpgll_parse(char **values, int length)
{
	nmea_gpgll_s *data;
	nmea_s *nmea_data;

	/* Allocate data struct */
	data = malloc(sizeof(nmea_gpgll_s));
	if (NULL == data) {
		return NULL;
	}
	memset(data, 0, sizeof(nmea_gpgll_s));
	nmea_data = (nmea_s *) data;

	int i = 0;
	char *value;
	while (i < length) {
		value = values[i];
		if (-1 == nmea_value_is_set(value)) {
			i++;
			continue;
		}

		switch (i) {
			case NMEA_GPGLL_TIME:
				/* Parse time */
				if (-1 == nmea_time_parse(value, &data->time)) {
					nmea_data->error = 1;
				}
				break;

			case NMEA_GPGLL_LATITUDE:
				/* Parse latitude */
				if (-1 == nmea_position_parse(value, &data->latitude)) {
					nmea_data->error = 1;
				}
				break;

			case NMEA_GPGLL_LATITUDE_CARDINAL:
				/* Parse cardinal direction */
				data->latitude.cardinal = nmea_cardinal_direction_parse(value);
				if (NMEA_CARDINAL_DIR_UNKNOWN == data->latitude.cardinal) {
					nmea_data->error = 1;
				}
				break;

			case NMEA_GPGLL_LONGITUDE:
				/* Parse longitude */
				if (-1 == nmea_position_parse(value, &data->longitude)) {
					nmea_data->error = 1;
				}
				break;

			case NMEA_GPGLL_LONGITUDE_CARDINAL:
				/* Parse cardinal direction */
				data->longitude.cardinal = nmea_cardinal_direction_parse(value);
				if (NMEA_CARDINAL_DIR_UNKNOWN == data->longitude.cardinal) {
					nmea_data->error = 1;
				}
				break;

			default:
				break;
		}

		i++;
	}

	return data;
}

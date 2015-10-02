#include "../nmea/parser_types.h"
#include "gpgll.h"
#include "parse.h"

int
init(nmea_parser_s *parser)
{
	/* Declare what sentence type to parse */
	parser->type = NMEA_GPGLL;

	return 0;
}

nmea_s *
allocate_data()
{
	return (nmea_s *) malloc(sizeof (nmea_gpgll_s));
}

void
set_default(nmea_s *nmea_data)
{
	memset(nmea_data, 0, sizeof (nmea_gpgll_s));
}

void
free_data(nmea_s *nmea_data)
{
	free(nmea_data);
}

int
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

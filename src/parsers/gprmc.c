#include "../nmea/parser_types.h"
#include "gprmc.h"
#include "parse.h"

int
init(nmea_parser_s *parser)
{
	/* Declare what sentence type to parse */
	NMEA_PARSER_TYPE(parser, NMEA_GPRMC);
	NMEA_PARSER_PREFIX(parser, "GPRMC");
	return 0;
}

int
allocate_data(nmea_parser_s *parser)
{
	parser->data = malloc(sizeof (nmea_gprmc_s));
	if (NULL == parser->data) {
		return -1;
	}

	return 0;
}

int
set_default(nmea_parser_s *parser)
{
	memset(parser->data, 0, sizeof (nmea_gprmc_s));
	return 0;
}

int
free_data(nmea_s *data)
{
	free(data);
	return 0;
}

int
parse(nmea_parser_s *parser, char *value, int val_index)
{
	nmea_gprmc_s *data = (nmea_gprmc_s *) parser->data;

	switch (val_index) {
	case NMEA_GPRMC_TIME:
		/* Parse time */
		if (-1 == nmea_time_parse(value, &data->date_time)) {
			return -1;
		}
		break;

	case NMEA_GPRMC_DATE:
		/* Parse date */
		if (-1 == nmea_date_parse(value, &data->date_time)) {
			return -1;
		}
		break;

	case NMEA_GPRMC_STATUS:
		/* Parse status */
		if (*value == 'A') {
			data->valid = true;
		} else if (*value == 'V') {
			data->valid = false;
		} else {
			return -1;
		}
		break;

	case NMEA_GPRMC_LATITUDE:
		/* Parse latitude */
		if (-1 == nmea_position_parse(value, &data->latitude)) {
			return -1;
		}
		break;

	case NMEA_GPRMC_LATITUDE_CARDINAL:
		/* Parse cardinal direction */
		data->latitude.cardinal = nmea_cardinal_direction_parse(value);
		if (NMEA_CARDINAL_DIR_UNKNOWN == data->latitude.cardinal) {
			return -1;
		}
		break;

	case NMEA_GPRMC_LONGITUDE:
		/* Parse longitude */
		if (-1 == nmea_position_parse(value, &data->longitude)) {
			return -1;
		}
		break;

	case NMEA_GPRMC_LONGITUDE_CARDINAL:
		/* Parse cardinal direction */
		data->longitude.cardinal = nmea_cardinal_direction_parse(value);
		if (NMEA_CARDINAL_DIR_UNKNOWN == data->longitude.cardinal) {
			return -1;
		}
		break;

	case NMEA_GPRMC_GNDSPD_KNOTS:
		/* Parse Ground speed, knots */
		data->gndspd_knots = strtod(value, NULL);
		break;

	case NMEA_GPRMC_TRUECOURSE_DEG:
		/* Parse the true course, degrees */
		data->track_deg = strtod(value, NULL);
		break;

	case NMEA_GPRMC_MAGVAR_DEG:
		/* Parse the Magnetic variation, degrees */
		data->magvar_deg = strtod(value, NULL);
		break;

	case NMEA_GPRMC_MAGVAR_CARDINAL:
		/* Parse the Magnetic variation direction, only E and W are valid */
		data->magvar_cardinal = nmea_cardinal_direction_parse(value);
		if (NMEA_CARDINAL_DIR_UNKNOWN == data->magvar_cardinal) {
			data->magvar_cardinal = NMEA_CARDINAL_DIR_UNKNOWN;
			return -1;
		}
		break;

	default:
		break;
	}

	return 0;
}

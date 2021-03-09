#include "../nmea/parser_types.h"
#include "gpgsv.h"
#include "parse.h"

int
init(nmea_parser_s *parser)
{
	/* Declare what sentence type to parse */
	NMEA_PARSER_TYPE(parser, NMEA_GPGSV);
	NMEA_PARSER_PREFIX(parser, "GPGSV");
	return 0;
}

int
allocate_data(nmea_parser_s *parser)
{
	parser->data = malloc(sizeof (nmea_gpgsv_s));
	if (NULL == parser->data) {
		return -1;
	}

	return 0;
}

int
set_default(nmea_parser_s *parser)
{
	memset(parser->data, 0, sizeof (nmea_gpgsv_s));
	((nmea_gpgsv_s *) parser->data)->sat[0].prn = -1;
	((nmea_gpgsv_s *) parser->data)->sat[1].prn = -1;
	((nmea_gpgsv_s *) parser->data)->sat[2].prn = -1;
	((nmea_gpgsv_s *) parser->data)->sat[3].prn = -1;

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
	nmea_gpgsv_s *data = (nmea_gpgsv_s *) parser->data;

	switch (val_index) {
	case NMEA_GPGSV_SENTENCES:
		/* Parse sentences */
		data->sentences = atoi(value);
		break;

	case NMEA_GPGSV_SENTENCE_NUMBER:
		/* Parse sentence number */
		data->sentence_number = atoi(value);
		break;

	case NMEA_GPGSV_SATELLITES:
		/* Parse number of SVs in view */
		data->satellites = atoi(value);
		break;

	case NMEA_GPGSV_PRN:
	case NMEA_GPGSV_PRN + 4:
	case NMEA_GPGSV_PRN + 8:
	case NMEA_GPGSV_PRN + 12:
		/* Parse longitude */
		data->sat[(val_index - NMEA_GPGSV_PRN) / 4].prn = atoi(value);
		break;

	case NMEA_GPGSV_ELEVATION:
	case NMEA_GPGSV_ELEVATION + 4:
	case NMEA_GPGSV_ELEVATION + 8:
	case NMEA_GPGSV_ELEVATION + 12:
		/* Parse cardinal direction */
		data->sat[(val_index - NMEA_GPGSV_ELEVATION) / 4].elevation = atoi(value);
		break;

	case NMEA_GPGSV_AZIMUTH:
	case NMEA_GPGSV_AZIMUTH + 4:
	case NMEA_GPGSV_AZIMUTH + 8:
	case NMEA_GPGSV_AZIMUTH + 12:
		/* Parse number of satellites */
		data->sat[(val_index - NMEA_GPGSV_AZIMUTH) / 4].azimuth = atoi(value);
		break;

	case NMEA_GPGSV_SNR:
	case NMEA_GPGSV_SNR + 4:
	case NMEA_GPGSV_SNR + 8:
	case NMEA_GPGSV_SNR + 12:
		/* Parse altitude */
		data->sat[(val_index - NMEA_GPGSV_SNR) / 4].snr = atoi(value);
		break;

	default:
		break;
	}

	return 0;
}

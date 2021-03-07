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

	case NMEA_GPGSV_S0_PRN:
		/* Parse SV0 PRN */
		data->s0_prn = atoi(value);
		break;

	case NMEA_GPGSV_S0_EL:
		/* Parse SV0 Elevation in degrees, 90 maximum */
		data->s0_el_deg = atoi(value);
		break;

	case NMEA_GPGSV_S0_AZ:
		/* Parse SV0 Azimuth, degrees from true north, 000 - 359  */
		data->s0_az_deg = atoi(value);
		break;

	case NMEA_GPGSV_S0_SNR:
		/* Parse SV0 SNR, 00-99 dB (null when not tracking) */
		data->s0_snr_db = atoi(value);
		break;

	case NMEA_GPGSV_S1_PRN:
		/* Parse SV1 PRN */
		data->s1_prn = atoi(value);
		break;

	case NMEA_GPGSV_S1_EL:
		/* Parse SV1 Elevation in degrees, 90 maximum */
		data->s1_el_deg = atoi(value);
		break;

	case NMEA_GPGSV_S1_AZ:
		/* Parse SV1 Azimuth, degrees from true north, 000 - 359  */
		data->s1_az_deg = atoi(value);
		break;

	case NMEA_GPGSV_S1_SNR:
		/* Parse SV1 SNR, 00-99 dB (null when not tracking) */
		data->s1_snr_db = atoi(value);
		break;

	case NMEA_GPGSV_S2_PRN:
		/* Parse SV2 PRN */
		data->s2_prn = atoi(value);
		break;

	case NMEA_GPGSV_S2_EL:
		/* Parse SV2 Elevation in degrees, 90 maximum */
		data->s2_el_deg = atoi(value);
		break;

	case NMEA_GPGSV_S2_AZ:
		/* Parse SV2 Azimuth, degrees from true north, 000 - 359  */
		data->s2_az_deg = atoi(value);
		break;

	case NMEA_GPGSV_S2_SNR:
		/* Parse SV2 SNR, 00-99 dB (null when not tracking) */
		data->s2_snr_db = atoi(value);
		break;

	case NMEA_GPGSV_S3_PRN:
		/* Parse SV3 PRN */
		data->s3_prn = atoi(value);
		break;

	case NMEA_GPGSV_S3_EL:
		/* Parse SV3 Elevation in degrees, 90 maximum */
		data->s3_el_deg = atoi(value);
		break;

	case NMEA_GPGSV_S3_AZ:
		/* Parse SV3 Azimuth, degrees from true north, 000 - 359  */
		data->s3_az_deg = atoi(value);
		break;

	case NMEA_GPGSV_S3_SNR:
		/* Parse SV3 SNR, 00-99 dB (null when not tracking) */
		data->s3_snr_db = atoi(value);
		break;

	default:
		break;
	}

	return 0;
}

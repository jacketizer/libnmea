#include "../nmea/parser_types.h"
#include "gptxt.h"
#include "parse.h"

int
init(nmea_parser_s *parser)
{
	/* Declare what sentence type to parse */
	NMEA_PARSER_TYPE(parser, NMEA_GPTXT);
	NMEA_PARSER_PREFIX(parser, "GPTXT");
	return 0;
}

int
allocate_data(nmea_parser_s *parser)
{
	parser->data = malloc(sizeof (nmea_gptxt_s));
	if (NULL == parser->data) {
		return -1;
	}

	return 0;
}

int
set_default(nmea_parser_s *parser)
{
	memset(parser->data, 0, sizeof (nmea_gptxt_s));
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
	nmea_gptxt_s *data = (nmea_gptxt_s *) parser->data;

	memset(data->text, 0, NMEA_GPTXT_TEXT_SIZE);

	switch (val_index) {
	case NMEA_GPTXT_ID00:
		data->id_00 = strtol(value, NULL, 10);
		break;
	case NMEA_GPTXT_ID01:
		data->id_01 = strtol(value, NULL, 10);
		break;
	case NMEA_GPTXT_ID02:
		data->id_02 = strtol(value, NULL, 10);
		break;
	case NMEA_GPTXT_TEXT:
		strncpy(data->text, value, NMEA_GPTXT_TEXT_SIZE);
		data->text[NMEA_GPTXT_TEXT_SIZE - 1] = '\0';
		break;
	default:
		break;
	}

	return 0;
}

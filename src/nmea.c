#include "nmea.h"

nmea_t
nmea_get_type(const char *sentence)
{
	if (0 == strncmp(sentence + 1, NMEA_PREFIX_GPGLL, NMEA_PREFIX_LENGTH)) {
		return NMEA_GPGLL;
	}

	if (0 == strncmp(sentence + 1, NMEA_PREFIX_GPGGA, NMEA_PREFIX_LENGTH)) {
		return NMEA_GPGGA;
	}

	return NMEA_UNKNOWN;
}

char
nmea_get_checksum(const char *sentence)
{
	const char *n = sentence + 1;
	char chk = 0;

	while (*n != '*' && n - sentence < NMEA_MAX_LENGTH) {
		chk ^= *n;
		n++;
	}

	return chk;
}

int
nmea_has_checksum(const char *sentence, int length)
{
	if ('*' == sentence[length - 5]) {
		return 0;
	}

	return -1;
}

int
nmea_validate(const char *sentence, int length, int check_checksum)
{
	/* should start with $ */
	if ('$' != *sentence) {
		return -1;
	}

	/* should end with \r\n, or other... */
	if ('\n' != sentence[length - 1] || '\n' != sentence[length - 2]) {
		return -1;
	}

	/* should have a 5 letter, uppercase word */
	const char *n = sentence;
	while (++n < sentence + 6) {
		if (*n < 65 || *n > 90) {
			/* not uppercase letter */
			return -1;
		}
	}

	/* should have a comma after the type word */
	if (',' != sentence[6]) {
		return -1;
	}

	/* check for checksum */
	if (1 == check_checksum && 0 == nmea_has_checksum(sentence, length)) {
		char actual_chk;
		long int expected_chk;
		char checksum[3];

		checksum[0] = sentence[length - 4];
		checksum[1] = sentence[length - 3];
		checksum[2] = '\0';
		actual_chk = nmea_get_checksum(sentence);
		expected_chk = strtol(checksum, NULL, 16);
		if (expected_chk != (long int) actual_chk) {
			return -1;
		}
	}

	return 0;
}

void
nmea_free(nmea_s *data)
{
	if (NULL == data) {
		return;
	}

	//data->free_data(data);
}

int
nmea_sentence_split(char *sentence, int length, char **values)
{
	char *cursor = sentence + 7; // skip type word
	int i = 0;

	values[i++] = cursor;
	while (cursor != NULL && cursor - sentence < length) {
		cursor = (char *) memchr(cursor, ',', length - (cursor - sentence));
		if (NULL == cursor) {
			break;
		}

		*cursor = '\0';
		cursor++;
		if (*cursor == ',') {
		    	  values[i++] = NULL;
		} else {
		    	  values[i++] = cursor;
		}
	}

	/* null terminate the last value */
	cursor = values[i - 1];
	cursor = (char *) memchr(cursor, '*', length - (cursor - sentence));
	if (NULL != cursor) {
		/* has checksum */
		*cursor = '\0';
	} else {
		/* no checksum */
		sentence[length - 2] = '\0';
	}

	return i;
}

nmea_s *
nmea_parse(char *sentence, int length, nmea_t type, int check_checksum)
{
	if (NMEA_UNKNOWN == type) {
		return (nmea_s *) NULL;
	}

	int n_vals;
	int val_index = 0;
	char *value;
	char *values[200];
	nmea_sentence_parser_s *parser;

	if (-1 == nmea_validate(sentence, length, check_checksum)) {
		return (nmea_s *) NULL;
	}

	n_vals = nmea_sentence_split(sentence, length, values);
	if (0 == n_vals) {
		return (nmea_s *) NULL;
	}

	parser = nmea_create_parser(type);
	if (NULL == parser) {
		return (nmea_s *) NULL;
	}

	/* Loop through the values and parse them... */
	while (val_index < n_vals) {
		value = values[val_index];
		if (-1 == nmea_value_is_set(value)) {
			val_index++;
			continue;
		}

		if (-1 == parser->parse(value, val_index, parser->data)) {
			parser->errors++;
		}

		val_index++;
	}

	parser->data->type = type;
	parser->data->errors = parser->errors;

	nmea_s *data = parser->data;
	free(parser);
	return data;
}

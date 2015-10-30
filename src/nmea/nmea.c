#include "nmea.h"
#include "parser.h"
#include "parser_types.h"


#ifdef __APPLE__

#include <limits.h>
#include "../memchr.c"

#endif /* __APPLE__ */

/**
 * Check if a value is supplied and set.
 *
 * Returns 0 if set, otherwise -1.
 */
static inline int
_is_value_set(const char *value)
{
	if (NULL == value || '\0' == *value) {
		return -1;
	}

	return 0;
}

/**
 * Crop a sentence from the type word and checksum.
 *
 * The type word at the beginning along with the dollar sign ($) will be
 * removed. If there is a checksum, it will also be removed. The two end
 * characters (usually <CR><LF>) will not be included in the new string.
 *
 * sentence is a validated NMEA sentence string.
 * length is the char length of the sentence string.
 *
 * Returns pointer (char *) to the new string.
 */
static inline char *
_crop_sentence(char *sentence, int length)
{
	char *start, *cursor;

	/* Skip type word, 7 characters */
	start = sentence + 7;

	/* Null terminate before end of line/sentence, 2 characters */
	start[length - 9] = '\0';

	/* Remove checksum, if there is one */
	cursor = (char *) memrchr(start, '*', length - (start - sentence));
	if (NULL != cursor) {
		/* Has checksum */
		*cursor = '\0';
	}

	return start;
}

/**
 * Splits a string by comma.
 *
 * string is the string to split, will be manipulated. Needs to be
 *        null-terminated.
 * values is a char pointer array that will be filled with pointers to the
 *        splitted values in the string.
 *
 * Returns the number of values found in string.
 */
static inline int
_split_string(char *string, char **values)
{
	int i = 0;
	char *cursor = string;
	char *end;

	/* Get end of string */
	end = (char *) rawmemchr(cursor, '\0');

	values[i++] = cursor;
	while (cursor != NULL && end - cursor > 0) {
		cursor = (char *) memchr(cursor, ',', end - cursor);
		if (NULL == cursor) {
			break;
		}

		*cursor = '\0';
		cursor++;
		values[i++] = cursor;
	}

	return i;
}

/**
 * Initiate the NMEA library and load the parser modules.
 *
 * This function will be called before the main() function.
 */
void __attribute__ ((constructor)) nmea_init(void);
void nmea_init()
{
	(void) nmea_load_parsers();
}

/**
 * Unload the parser modules.
 *
 * This function will be called after the exit() function.
 */
void __attribute__ ((destructor)) nmea_cleanup(void);
void nmea_cleanup()
{
	nmea_unload_parsers();
}

nmea_t
nmea_get_type(const char *sentence)
{
	nmea_parser_module_s *parser = nmea_get_parser_by_sentence(sentence);
	if (NULL == parser) {
		return NMEA_UNKNOWN;
	}

	return parser->parser.type;
}

uint8_t
nmea_get_checksum(const char *sentence)
{
	const char *n = sentence + 1;
	uint8_t chk = 0;

	/* While current char isn't '*' or sentence ending (newline) */
	while ('*' != *n && NMEA_END_CHAR_1 != *n) {
		if ('\0' == *n || n - sentence > NMEA_MAX_LENGTH) {
		/* Sentence too long or short */
			return 0;
		}
		chk ^= (uint8_t) *n;
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
		uint8_t actual_chk;
		uint8_t expected_chk;
		char checksum[3];

		checksum[0] = sentence[length - 4];
		checksum[1] = sentence[length - 3];
		checksum[2] = '\0';
		actual_chk = nmea_get_checksum(sentence);
		expected_chk = (uint8_t) strtol(checksum, NULL, 16);
		if (expected_chk != actual_chk) {
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

	nmea_parser_module_s *parser = nmea_get_parser_by_type(data->type);
	if (NULL == parser) {
		return;
	}

	parser->free_data(data);
}

nmea_s *
nmea_parse(char *sentence, int length, int check_checksum)
{
	nmea_t type = nmea_get_type(sentence);
	if (NMEA_UNKNOWN == type) {
		return (nmea_s *) NULL;
	}

	int n_vals;
	int val_index = 0;
	char *value;
	char *values[255];
	nmea_parser_module_s *parser;

	/* Validate */
	if (-1 == nmea_validate(sentence, length, check_checksum)) {
		return (nmea_s *) NULL;
	}

	/* Crop sentence from type word and checksum */
	char *val_string = _crop_sentence(sentence, length);
	if (NULL == val_string) {
	      	return (nmea_s *) NULL;
	}

	/* Split the sentence into values */
	n_vals = _split_string(val_string, values);
	if (0 == n_vals) {
		return (nmea_s *) NULL;
	}

	/* Get the right parser */
	parser = nmea_get_parser_by_type(type);
	if (NULL == parser) {
		return (nmea_s *) NULL;
	}

	/* Allocate memory for parsed data */
	parser->allocate_data((nmea_parser_s *) parser);
	if (NULL == parser->parser.data) {
		return (nmea_s *) NULL;
	}

	/* Set default values */
	parser->set_default((nmea_parser_s *) parser);
	parser->errors = 0;

	/* Loop through the values and parse them... */
	while (val_index < n_vals) {
		value = values[val_index];
		if (-1 == _is_value_set(value)) {
			val_index++;
			continue;
		}

		if (-1 == parser->parse((nmea_parser_s *) parser, value, val_index)) {
			parser->errors++;
		}

		val_index++;
	}

	parser->parser.data->type = type;
	parser->parser.data->errors = parser->errors;

	return parser->parser.data;
}

#include "nmea.h"
#include "gpgll.h"

int
_nmea_split_sentence(char *sentence, int length, char **values)
{
	char *cursor = sentence + 7; // skip type type word
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

int
_nmea_get_position(char *s, nmea_position *pos)
{
	pos->degrees = 0;
	pos->minutes = 0;

	if (*s == NULL) {
		return -1;
	}

	// decimal minutes
	char *cursor = memchr(s, '.', strlen(s));
	if (NULL == cursor) {
		return -1;
	}

	cursor -= 2; // minutes starts 2 digits before dot
	pos->minutes = atof(cursor);
	*cursor = '\0';

	// integer degrees
	cursor = s;
	pos->degrees = atoi(cursor);

	return 0;
}

nmea_t
nmea_get_type(const char *sentence)
{
	if (0 == strncmp(sentence + 1, NMEA_PREFIX_GPGLL, NMEA_PREFIX_LENGTH)) {
		return NMEA_GPGLL;
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
nmea_validate(const char *sentence, int length)
{
	/* should start with $ */
	if ('$' != *sentence) {
		return -1;
	}

	/* should end with \r\n */
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
  if (0 == nmea_has_checksum(sentence, length)) {
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

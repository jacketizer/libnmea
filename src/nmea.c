#include "nmea.h"
#include "gpgll.h"
#include "gpgga.h"

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
nmea_validate(const char *sentence, int length)
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

nmea_s *
nmea_parse(char *sentence, int length, nmea_t type)
{
	nmea_s *data;

	switch (type) {
		case NMEA_UNKNOWN:
			break;
		case NMEA_GPGGA:
			if (-1 == nmea_validate(sentence, length)) {
				break;
			}

			data = (nmea_s *) nmea_gpgga_parse(sentence, length);
			if (NULL == data) {
				break;
			}

			data->type = type;
			return data;
		case NMEA_GPGLL:
			if (-1 == nmea_validate(sentence, length)) {
				break;
			}

			data = (nmea_s *) nmea_gpgll_parse(sentence, length);
			if (NULL == data) {
				break;
			}

			data->type = type;
			return data;
		default:
			break;
	}

	return (nmea_s *) NULL;
}

#include "parse.h"

int
nmea_sentence_split(char *sentence, int length, char **values)
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
nmea_position_parse(char *s, nmea_position *pos)
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

#ifndef INC_NMEA_H
#define INC_NMEA_H

#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "sentences/types.h"
#include "sentences/parser.h"

/* nmea sentence max length */
#define NMEA_MAX_LENGTH		50

/* nmea sentence endings */
#define NMEA_END_CHAR_1		'\n'
#define NMEA_END_CHAR_2		'\n'

/* NMEA sentence prefixes */
#define NMEA_PREFIX_LENGTH	5
#define NMEA_PREFIX_GPGGA	"GPGGA"	// Global Positioning System Fix Data
#define NMEA_PREFIX_GPGLL	"GPGLL"	// Geographic Position, Latitude/Longitude/Time

/**
 * Splits an NMEA sentence by comma.
 *
 * sentence is the string to split, will be manipulated.
 * length is the char length of the sentence string.
 * values is a char pointer array that will be filled with pointers to the
 * splitted values in the sentence string.
 */
int nmea_sentence_split(char *sentence, int length, char **values);

/**
 * Get the type of the sentence.
 *
 * sentence needs to be a validated NMEA sentence string.
 */
extern nmea_t nmea_get_type(const char *sentence);

/**
 * Calculate the checksum of the sentence.
 *
 * sentence needs to be a validated NMEA sentence string.
 */
extern char nmea_get_checksum(const char *sentence);

/**
 * Check if the sentence has a precalculated checksum.
 *
 * sentence needs to be a validated NMEA sentence string.
 * length is the byte length of the sentence string.
 */
extern int nmea_has_checksum(const char *sentence, int length);

/**
 * Validate the sentence according to NMEA 0183.
 *
 * - Should start with a dollar sign.
 * - The next five chars should be uppercase letters.
 * - If it has a checksum, it checks it.
 * - Ends with \r\n (<CR><LF>).
 *
 * length is the byte length of the sentence string.
 */
extern int nmea_validate(const char *sentence, int length, int check_checksum);

/**
 * Parses an NMEA sentence string.
 *
 * sentence needs to be a validated NMEA sentence string.
 * length is the byte length of the sentence string.
 * check_checksum, if 1 and there is a checksum, validate it.
 */
extern nmea_s *nmea_parse(char *sentence, int length, nmea_t type, int check_checksum);

#endif  /* INC_NMEA_H */

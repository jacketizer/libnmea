#ifndef INC_NMEA_H
#define INC_NMEA_H

#include <stdlib.h>
#include <string.h>

/* nmea sentence max length */
#define NMEA_MAX_LENGTH		50

/* nmea sentence endings */
#define NMEA_END_CHAR_1		'\n'
#define NMEA_END_CHAR_2		'\n'

/* NMEA sentence prefixes */
#define NMEA_PREFIX_LENGTH	5
#define NMEA_PREFIX_GPGLL	"GPGLL"	// Geographic Position, Latitude/Longitude

/* NMEA sentence types */
typedef enum {NMEA_UNKNOWN, NMEA_GPGLL} nmea_t;

/* NMEA cardinal direction types */
typedef char nmea_cardinal_t;
#define NMEA_CARDINAL_DIR_NORTH		(nmea_cardinal_t) 'N'
#define NMEA_CARDINAL_DIR_EAST		(nmea_cardinal_t) 'E'
#define NMEA_CARDINAL_DIR_SOUTH		(nmea_cardinal_t) 'S'
#define NMEA_CARDINAL_DIR_WEST		(nmea_cardinal_t) 'W'
#define NMEA_CARDINAL_DIR_UNKNOWN	(nmea_cardinal_t) '\0'

typedef struct {
	nmea_t type;
	int error;
} nmea_s;

typedef struct {
	double minutes;
	int degrees;
	nmea_cardinal_t cardinal;
} nmea_position;

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
extern int nmea_validate(const char *sentence, int length);

/**
 * Parses an NMEA sentence string.
 *
 * sentence needs to be a validated NMEA sentence string.
 * length is the byte length of the sentence string.
 */
extern nmea_s *nmea_parse(char *sentence, int length, nmea_t type);

#endif  /* INC_NMEA_H */

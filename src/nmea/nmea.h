#ifndef INC_NMEA_H
#define INC_NMEA_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* NMEA sentence types */
typedef enum {NMEA_UNKNOWN, NMEA_GPGGA, NMEA_GPGLL} nmea_t;

/* NMEA cardinal direction types */
typedef char nmea_cardinal_t;
#define NMEA_CARDINAL_DIR_NORTH		(nmea_cardinal_t) 'N'
#define NMEA_CARDINAL_DIR_EAST		(nmea_cardinal_t) 'E'
#define NMEA_CARDINAL_DIR_SOUTH		(nmea_cardinal_t) 'S'
#define NMEA_CARDINAL_DIR_WEST		(nmea_cardinal_t) 'W'
#define NMEA_CARDINAL_DIR_UNKNOWN	(nmea_cardinal_t) '\0'

/* NMEA data base struct */
typedef struct {
	nmea_t type;
	int errors;
} nmea_s;

/* GPS position struct */
typedef struct {
	double minutes;
	int degrees;
	nmea_cardinal_t cardinal;
} nmea_position;

/* NMEA sentence max length */
#define NMEA_MAX_LENGTH		50

/* NMEA sentence endings */
#define NMEA_END_CHAR_1		'\n'
#define NMEA_END_CHAR_2		'\n'

/* NMEA sentence prefix length */
#define NMEA_PREFIX_LENGTH	5

/**
 * Initiate the NMEA library.
 *
 * This function should be called once before starting to use the library.
 *
 * Returns 0 on success, otherwise -1.
 */
extern int nmea_init();

/**
 * Get the type of the sentence.
 *
 * sentence needs to be a validated NMEA sentence string.
 *
 * Returns nmea_t (int).
 */
extern nmea_t nmea_get_type(const char *sentence);

/**
 * Calculate the checksum of the sentence.
 *
 * sentence needs to be a validated NMEA sentence string.
 *
 * Returns the calculated checksum (uint8).
 */
extern uint8_t nmea_get_checksum(const char *sentence);

/**
 * Check if the sentence has a precalculated checksum.
 *
 * sentence needs to be a validated NMEA sentence string.
 * length is the byte length of the sentence string.
 *
 * Return 0 if checksum exists, otherwise -1.
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
 *
 * Returns 0 if sentence is valid, otherwise -1.
 */
extern int nmea_validate(const char *sentence, int length, int check_checksum);

/**
 * Parses an NMEA sentence string.
 *
 * sentence needs to be a validated NMEA sentence string.
 * length is the byte length of the sentence string.
 * check_checksum, if 1 and there is a checksum, validate it.
 *
 * Returns a pointer to a nmea data struct, or (nmea_s *) NULL if an error occurs.
 */
extern nmea_s *nmea_parse(char *sentence, int length, nmea_t type, int check_checksum);

#endif  /* INC_NMEA_H */

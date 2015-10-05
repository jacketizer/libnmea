C Library for Parsing NMEA 0183 Sentences
=========================================

The library can validate and parse NMEA sentences into C structs. If you find
any sentence missing, please add it by contributing to the code.

To build:

```sh
$ make && sudo make install && make test
```

Try it:

```sh
$ echo -ne "\$GPGLL,4916.45,N,12311.12,W,225444,A,*1D\n\n" | ./test
```

How to use it
-------------

Include ´nmea.h´ and compile with ´-lnmea´.

Library functions
-----------------

### ´extern int nmea_init();´

Initiate the NMEA library.

This function should be called once before starting to use the library.

Returns 0 on success, otherwise -1.


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

Implement a new sentence type
-----------------------------

To create a new sentence parser, create the following files and replace
the `<TYPE>` with the sentence type word in uppercase letters and `<type>` in
lowercase. Make sure that the sentence type is defined in *src/types.h*.

src/parsers/<type>.c:

```C
#ifndef INC_NMEA_<TYPE>_H
#define INC_NMEA_<TYPE>_H

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <nmea.h>

typedef struct {
	nmea_s base;
	/* sentence values */
} nmea_<type>_s;

/* Value indexes */
#define NMEA_<TYPE>_XXX			0
/* more value indexes... */

#endif  /* INC_NMEA_<TYPE>_H */
```

src/parsers/<type>.c:

```C
#include "../nmea/parser_types.h"
#include "<type>.h"
#include "parse.h"

init_f
init(nmea_sentence_parser_s *parser)
{
	/* Declare what sentence type to parse */
	parser->type = NMEA_<TYPE>;

	return 0;
}

allocate_data_f
allocate_data()
{
	return malloc(sizeof (nmea_<type>_s));
}

set_default_f
set_default(nmea_s *nmea_data)
{
	memset(nmea_data, 0, sizeof (nmea_<type>_s));
}

free_data_f
free_data(nmea_s *nmea_data)
{
	free(nmea_data);
}

parse_f
parse(char *value, int val_index, nmea_s *nmea_data)
{
	nmea_<type>_s *data = (nmea_<type>_s *) nmea_data;

	switch (val_index) {
		case NMEA_<TYPE>_XXX:
			/* Parse some value and save in data struct */
			break;

		default:
			break;
	}

	return 0;
}
```

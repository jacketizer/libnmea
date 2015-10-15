C Library for Parsing NMEA 0183 Sentences
=========================================

![Travis-CI Build and Test Status](https://api.travis-ci.org/jacketizer/libnmea.svg)

The library can validate and parse NMEA sentences into C structs. If you find
any sentence missing, please add it by contributing to the code.

To build:

```sh
$ make && sudo make install && make unit-tests
```

Try it:

```sh
$ make examples
$ echo -ne "\$GPGLL,4916.45,N,12311.12,W,225444,A,*1D\n\n" | build/parse_stdin
```

How to use it
-------------

First, include `nmea.h` and the header files for the desired sentence types:

```c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <nmea.h>
#include <nmea/gpgll.h>
#include <nmea/gpgga.h>

int
main(void)
{
	/* ... */
	return 0;
}
```

To parse an NMEA sentence string, use `nmea_parse()`:

```c
// Sentence string to be parsed
char *sentence = strdup("$GPGLL,4916.45,N,12311.12,W,225444,A,*1D\n\n");

// Pointer to struct containing the parsed data
nmea_s *data;

// Parse it...
data = nmea_parse(sentence, strlen(sentence), 0);
if (NULL == data) {
	exit(EXIT_FAILURE);
}
```

The parsed data can be found in the data variable and for example printed to
screen:

```c
if (NMEA_GPGGA == data->type) {
	nmea_gpgga_s *gpgga = (nmea_gpgga_s *) data;

	printf("GPGGA Sentence\n");
	printf("Number of satellites: %d\n", gpgga->n_satellites);
	printf("Altitude: %d %c\n", gpgga->altitude, gpgga->altitude_unit);
}

if (NMEA_GPGLL == data->type) {
	nmea_gpgll_s *gpgll = (nmea_gpgll_s *) data;

	printf("GPGLL Sentence\n");
	printf("Longitude:\n");
	printf("  Degrees: %d\n", gpgll->longitude.degrees);
	printf("  Minutes: %f\n", gpgll->longitude.minutes);
	printf("  Cardinal: %c\n", (char) gpgll->longitude.cardinal);
	printf("Latitude:\n");
	printf("  Degrees: %d\n", gpgll->latitude.degrees);
	printf("  Minutes: %f\n", gpgll->latitude.minutes);
	printf("  Cardinal: %c\n", (char) gpgll->latitude.cardinal);
}
```

Free the memory used by the data variable:

```c
nmea_free(data);
```

Compile with `-lnmea`:

```sh
$ gcc example.c -lnmea -o example
```

Environment variables
---------------------

`NMEA_PARSER_PATH` - The path where the parser libraries are located. Default
is `/usr/lib/nmea/`.

Library functions
-----------------

Check `nmea.h` for more detailed info about functions. The header files for the
sentences (ex: `nmea/gpgll.h`) contains the struct definitions.

Implement a new sentence type
-----------------------------

To create a new sentence parser, create the following files and replace
the `<TYPE>` with the sentence type word in uppercase letters and `<type>` in
lowercase. Make sure that the sentence type is defined in *src/nmea.h*.

src/parsers/<type>.h:

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
#define NMEA_<TYPE>_<index>	0
/* more value indexes... */

#endif  /* INC_NMEA_<TYPE>_H */
```

src/parsers/<type>.c:

```C
#include "../nmea/parser_types.h"
#include "<type>.h"
#include "parse.h"

int
init(nmea_parser_s *parser)
{
	/* Declare what sentence type to parse */
	NMEA_PARSER_TYPE(parser, NMEA_<TYPE>);
	NMEA_PARSER_PREFIX(parser, "<TYPE>");
	return 0;
}

int
allocate_data(nmea_parser_s *parser)
{
	parser->data = malloc(sizeof (nmea_<type>_s));
	return 0;
}

int
set_default(nmea_parser_s *parser)
{
	memset(parser->data, 0, sizeof (nmea_<type>_s));
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
	nmea_gpgll_s *data = (nmea_<type>_s *) parser->data;

	switch (val_index) {
		case NMEA_<TYPE>_<index>:
			/* Parse some value */
			data->some_value = value;
			break;

		default:
			break;
	}

	return 0;
}
```

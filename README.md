# C Library for Parsing NMEA 0183 Sentences

[![Build Status](https://github.com/jacketizer/libnmea/workflows/builds%20and%20tests/badge.svg)](https://github.com/jacketizer/libnmea/actions?query=workflow%3A%22builds+and+tests%22+branch%3Amaster)
![Memory Leaks](https://img.shields.io/badge/memory%20leaks-0%20bytes-brightgreen.svg)
[![License](https://img.shields.io/npm/l/express.svg)](https://raw.githubusercontent.com/jacketizer/libnmea/master/LICENSE)

Libnmea is a lightweight C library that parses NMEA 0183 sentence strings into
structs. It is written in a modular architecture that dynamically loads a parser
module for each implemented sentence type. This way, new sentences can easily
be added to the library without modifying the core code. It is also possible to
statically link the parser modules at build time to enable libnmea to be used in
environments where a dynamic loader isn't available.

If you find any sentence missing, please add it by contributing to the code. I
am open to suggestions regarding the code and architecture, so if you have any
ideas or improvements, please tell me or submit a merge request :-).

Supported sentences: `GPGGA`, `GPGLL`, `GPGSA`, `GPGSV`, `GPRMC`, `GPTXT`, and `GPVTG`.

## To build

```sh
$ make && make check
```

When running `make`, the library will be built to a local build directory
(*./build*).

## Installation

Run `make install` to install *libnmea*. The files will be installed in */usr/*
by default. Use the environment variable `PREFIX` to set a different
installation prefix.

Ex. to build and install the library and header files locally, in the *./target*
directory, run the following commands:

```sh
$ make
$ PREFIX=target make install
```

## Try it

When the library is built and installed, you can compile the example programs:

```sh
$ make examples
$ echo -ne "\$GPGLL,4916.45,N,12311.12,W,225444,A,*1D\r\n" | build/parse_stdin
```

If the library was installed with a custom prefix, you may have to set the
following environment variables before running `make`:

```sh
export LIBRARY_PATH="<prefix>/lib"
export C_INCLUDE_PATH="<prefix>/include"
export LD_LIBRARY_PATH="<prefix>/lib"
```

Additionally, set `NMEA_PARSER_PATH` variable before running the program:
```sh
export NMEA_PARSER_PATH="<prefix>/lib/nmea/"
```
Note that the trailing slash is required!

## How to use it

First, include *nmea.h* and the header files for the desired sentence types:

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
char sentence[] = "$GPGLL,4916.45,N,12311.12,W,225444,A,*1D\r\n";

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

## Environment variables

### Run time:

`NMEA_PARSER_PATH` - The path where the parser libraries are located. Default
is `/usr/lib/nmea`. If a custom prefix was used when installing, they will be
located in *PREFIX/lib/nmea*. This variable isn't used when libnmea is built
with static parser module loading (see next chapter).

### Build time:

`NMEA_STATIC` - If defined, it forces libnmea to be built with static parser
module loading (see next chapter). It should contain a comma seperated list of
parser modules to be included in the build, ex: `NMEA_STATIC=GPRMC,GPGGA`.

## Static build

It is possible to statically link the parser modules at build time which is
useful when a dynamic loader isn't available. To do this, the environment
variable `NMEA_STATIC` must be defined along with a comma-seperated list of
selected parser modules to include in the library. Note that both dynamic and
static module loading cannot be utilized at the same time.

```sh
$ NMEA_STATIC=GPGLL,GPRMC make
```

## Run tests

After `make`, run the tests against the build directory:

```sh
$ make check
```

To run the unit tests against the installation directory, run the following
command:

```sh
$ make unit-tests
```

To check for memory leaks, run:

```sh
$ make check-memory-leaks
```

To verify that `parse_stdin` example works, build it and run the test script:
```sh
$ make examples
$ tests/parse_stdin/test.sh build/parse_stdin
```

## Library functions

Check *nmea.h* for more detailed info about functions. The header files for the
sentences (ex: *nmea/gpgll.h*) contains the struct definitions for each
sentence.

## Implement a new sentence type

To create a new sentence parser, create the following files and replace
the `<TYPE>` with the sentence type word in uppercase letters and `<type>` in
lowercase. Make sure that the sentence type is defined in *src/nmea.h* and add
it to *src/nmea/parser_static.h*.

*src/parsers/<type>.h*:

```c
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

*src/parsers/<type>.c*:

```c
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

## Contributing

Contributions are more than welcome. Be sure to read this chapter before
submitting a merge request.

### Coding style

The code should conform to the
[KNF](https://en.wikipedia.org/wiki/Indent_style#Variant:_BSD_KNF)
formatting guidelines.

Example `indent` command:

```sh
$ indent -st -bad -bap -bbb -bc -blf -bli0 -br -brs -bs -cbi0 -ce -cli0 -cs -nbfda -npcs -nprs -nsob -saf -saw -sai src/nmea/nmea.c
```

Example `astyle` command:
```sh
astyle --style=knf --indent=tab src/nmea/nmea.c
```

Use hard tabs. Example vim options:
```
:set noexpandtab
:set preserveindent
:set softtabstop=0
:set shiftwidth=4
:set tabstop=4
```

### Testing

Every merge request must pass all the tests.

### New sentence types

When contributing support for new sentence types, please do the following:

* Update the [parse_stdin.c](examples/parse_stdin.c) example to handle the newly added sentence type, printing all the parsed data.
* Add an example message of the newly added type to [parse_stdin_test_in.txt](tests/parse_stdin_test_in.txt).
* Add expected output to [parse_stdin_out_expected.txt](tests/parse_stdin_test_out_expected.txt).
* Update README.md where the supported sentence types are listed.

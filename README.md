C Library for Parsing NMEA 0183 Sentences
=========================================

The library can validate and parse NMEA sentences into C structs. If you find
any sentence missing, please add it by contributing to the code.

To build:

```sh
$ make && make test
```

To test:

```sh
$ export LD_LIBRARY_PATH=./
$ echo -ne "\$GPGLL,4916.45,N,12311.12,W,225444,A,*1D\n\n" | ./test
```

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
#include "types.h"
#include "../parse.h"

typedef struct {
	nmea_s base;
  /* sentence values */
} nmea_<type>_s;

/* Value indexes */
#define NMEA_<TYPE>_XXX			0
/* more value indexes... */

extern init_f init(nmea_sentence_parser_s *parser);
extern allocate_data_f allocate_data();
extern set_default_f set_default(nmea_s *nmea_data);
extern free_data_f free_data(nmea_s *nmea_data);
extern parse_f parse(char *value, int val_index, nmea_s *nmea_data);

#endif  /* INC_NMEA_<TYPE>_H */
```

src/parsers/<type>.c:

```C
#include "<type>.h"

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
	return malloc(sizeof(nmea_<type>_s));
}

set_default_f
set_default(nmea_s *nmea_data)
{
	memset(nmea_data, 0, sizeof (nmea_gpgga_s));
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
			/* Parse some value */
			break;

		default:
			break;
	}

	return 0;
}
```



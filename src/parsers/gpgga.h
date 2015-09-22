#ifndef INC_NMEA_GPGGA_H
#define INC_NMEA_GPGGA_H

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "types.h"
#include "../parse.h"

typedef struct {
	nmea_s base;
	struct tm time;
	nmea_position longitude;
	nmea_position latitude;
	int n_satellites;
	int altitude;
	char altitude_unit;
} nmea_gpgga_s;

/* Value indexes */
#define NMEA_GPGGA_TIME			0
#define NMEA_GPGGA_LATITUDE		1
#define NMEA_GPGGA_LATITUDE_CARDINAL	2
#define NMEA_GPGGA_LONGITUDE		3
#define NMEA_GPGGA_LONGITUDE_CARDINAL	4
#define NMEA_GPGGA_N_SATELLITES		6
#define NMEA_GPGGA_ALTITUDE		8
#define NMEA_GPGGA_ALTITUDE_UNIT	9

extern init_f init(nmea_sentence_parser_s *parser);
extern allocate_data_f allocate_data();
extern set_default_f set_default(nmea_s *nmea_data);
extern free_data_f free_data(nmea_s *nmea_data);
extern parse_f parse(char *value, int val_index, nmea_s *nmea_data);

#endif  /* INC_NMEA_GPGGA_H */

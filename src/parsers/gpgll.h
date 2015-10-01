#ifndef INC_NMEA_GPGLL_H
#define INC_NMEA_GPGLL_H

#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
	nmea_s base;
	nmea_position longitude;
	nmea_position latitude;
	struct tm time;
} nmea_gpgll_s;

/* Value indexes */
#define NMEA_GPGLL_LATITUDE		0
#define NMEA_GPGLL_LATITUDE_CARDINAL	1
#define NMEA_GPGLL_LONGITUDE		2
#define NMEA_GPGLL_LONGITUDE_CARDINAL	3
#define NMEA_GPGLL_TIME			4

extern init_f init(nmea_sentence_parser_s *parser);
extern allocate_data_f allocate_data();
extern set_default_f set_default(nmea_s *nmea_data);
extern free_data_f free_data(nmea_s *nmea_data);
extern parse_f parse(char *value, int val_index, nmea_s *nmea_data);

#endif  /* INC_NMEA_GPGLL_H */

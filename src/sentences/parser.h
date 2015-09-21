#ifndef INC_NMEA_PARSER_H
#define INC_NMEA_PARSER_H

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <dlfcn.h>
#include "../types.h"
#include "types.h"

nmea_sentence_parser_s * nmea_create_parser(nmea_t type);

#endif  /* INC_NMEA_PARSER_H */

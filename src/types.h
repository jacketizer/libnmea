#ifndef INC_NMEA_TYPES_H
#define INC_NMEA_TYPES_H

/* NMEA sentence types */
typedef enum {NMEA_UNKNOWN, NMEA_GPGGA, NMEA_GPGLL} nmea_t;

/* NMEA cardinal direction types */
typedef char nmea_cardinal_t;
#define NMEA_CARDINAL_DIR_NORTH		(nmea_cardinal_t) 'N'
#define NMEA_CARDINAL_DIR_EAST		(nmea_cardinal_t) 'E'
#define NMEA_CARDINAL_DIR_SOUTH		(nmea_cardinal_t) 'S'
#define NMEA_CARDINAL_DIR_WEST		(nmea_cardinal_t) 'W'
#define NMEA_CARDINAL_DIR_UNKNOWN	(nmea_cardinal_t) '\0'

typedef struct {
	nmea_t type;
	int errors;
} nmea_s;

typedef struct {
	double minutes;
	int degrees;
	nmea_cardinal_t cardinal;
} nmea_position;

#endif  /* INC_NMEA_TYPES_H */

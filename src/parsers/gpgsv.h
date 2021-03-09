#ifndef INC_NMEA_GPGSV_H
#define INC_NMEA_GPGSV_H

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <nmea.h>

typedef struct {
	nmea_s base;
	unsigned int sentences; //Number of sentences for full data
	unsigned int sentence_number; //Current sentence number
	unsigned int satellites; //Number of satellites in view
	struct {
		int prn; //Satellite PRN number
		int elevation; //Elevation, degrees
		int azimuth; //Azimuth, degrees
		int snr; //SNR - higher is better
	} sat[4];
} nmea_gpgsv_s;

/* Value indexes */
enum {
	NMEA_GPGSV_SENTENCES,
	NMEA_GPGSV_SENTENCE_NUMBER,
	NMEA_GPGSV_SATELLITES,
	NMEA_GPGSV_PRN,
	NMEA_GPGSV_ELEVATION,
	NMEA_GPGSV_AZIMUTH,
	NMEA_GPGSV_SNR
};

#endif  /* INC_NMEA_GPGSV_H */

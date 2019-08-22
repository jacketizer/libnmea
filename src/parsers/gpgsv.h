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

	unsigned int s0_prn; //Satellite PRN number
	int s0_el_deg; //Elevation, degrees
	int s0_az_deg; //Azimuth, degrees
	int s0_snr_db; //SNR - higher is better

	unsigned int s1_prn; //Satellite PRN number
	int s1_el_deg; //Elevation, degrees
	int s1_az_deg; //Azimuth, degrees
	int s1_snr_db; //SNR - higher is better

	unsigned int s2_prn; //Satellite PRN number
	int s2_el_deg; //Elevation, degrees
	int s2_az_deg; //Azimuth, degrees
	int s2_snr_db; //SNR - higher is better

	unsigned int s3_prn; //Satellite PRN number
	int s3_el_deg; //Elevation, degrees
	int s3_az_deg; //Azimuth, degrees
	int s3_snr_db; //SNR - higher is better
} nmea_gpgsv_s;

/* Value indexes */
enum {
	NMEA_GPGSV_SENTENCES,
	NMEA_GPGSV_SENTENCE_NUMBER,
	NMEA_GPGSV_SATELLITES,
	NMEA_GPGSV_S0_PRN,
	NMEA_GPGSV_S0_EL,
	NMEA_GPGSV_S0_AZ,
	NMEA_GPGSV_S0_SNR,
	NMEA_GPGSV_S1_PRN,
	NMEA_GPGSV_S1_EL,
	NMEA_GPGSV_S1_AZ,
	NMEA_GPGSV_S1_SNR,
	NMEA_GPGSV_S2_PRN,
	NMEA_GPGSV_S2_EL,
	NMEA_GPGSV_S2_AZ,
	NMEA_GPGSV_S2_SNR,
	NMEA_GPGSV_S3_PRN,
	NMEA_GPGSV_S3_EL,
	NMEA_GPGSV_S3_AZ,
	NMEA_GPGSV_S3_SNR
};

#endif  /* INC_NMEA_GPGSV_H */

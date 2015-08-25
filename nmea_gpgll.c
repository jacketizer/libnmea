#include "nmea_gpgll.h"

nmea_gpgll_s
nmea_gpgll_parse(const char *sentence)
{
	nmea_gpgll_s data;

	// Parse sentence to struct.
	// .........................
	// Split values by comma.
	// Find dot, extract minutes by taking 2 digits before the dot
	// 	and then the rest until the comma.
	// Take the remaing digits in front of minutes and save it as degrees.
	// Get direction (N, S, E, W).

	return data;
}

#include "nmea.h"
#include "nmea_gpgll.h"

nmea_gpgll_s
nmea_gpgll_parse(const char *sentence, int length)
{
	nmea_gpgll_s data;

	// Parse sentence to struct.
	// .........................
	// Split values by comma.
	// Find dot, extract minutes by taking 2 digits before the dot
	// 	and then the rest until the comma.
	// Take the remaing digits in front of minutes and save it as degrees.
	// Get direction (N, S, E, W).

	
	char **values = malloc(200);
	int n_vals = _nmea_split_sentence(sentence, length, values);
	while (n_vals > 0) {
		fprintf(stderr, "  value[%d]: %s\n", n_vals, values[--n_vals]);
	}

	if (NMEA_GPGLL_N_VALUES != n_vals) {
		return NULL;
	}

	/* parse logitude */
	char *dot = memchr(values[0], '.', strlen(values[0]));	
	if (NULL == dot) {
		return NULL;
	}

	dot -= 2; // minutes takes 2 digits before dot
	fprintf(stderr, "  Minutes: %s\n", dot);

	return data;
}

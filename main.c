#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

struct nmea_gpgll_s
{
	char	type[5];
	// latitude
	unsigned int	lat_deg;
	float	lat_min;
	unsigned int	n_or_s;
	// longitude
	unsigned int	long_deg;
	float	long_min;
	unsigned int	e_or_w;
};

/* NMEA ENTRY TYPES */
#define NMEA_GPBOD	"GPBOD"	// Bearing, origin to destination
#define NMEA_GPBWC 	"GPBWC"	// Bearing and distance to waypoint, great circle
#define NMEA_GPGGA 	"GPGAA"	// Global Positioning System Fix Data
#define NMEA_GPGLL 	"GPGLL"	// Geographic position, latitude / longitude
#define NMEA_GPGSA 	"GPGSA"	// GPS DOP and active satellites 
#define NMEA_GPGSV 	"GPGSV"	// GPS Satellites in view
#define NMEA_GPHDT 	"GPHDT"	// Heading, True
#define NMEA_GPR00 	"GPR00"	// List of waypoints in currently active route
#define NMEA_GPRMA 	"GPRMA"	// Recommended minimum specific Loran-C data
#define NMEA_GPRMB 	"GPRMB"	// Recommended minimum navigation info
#define NMEA_GPRMC 	"GPRMC"	// Recommended minimum specific GPS/Transit data
#define NMEA_GPRTE 	"GPRTE"	// Routes
#define NMEA_GPTRF 	"GPTRF"	// Transit Fix Data
#define NMEA_GPSTN 	"GPSTN"	// Multiple Data ID
#define NMEA_GPVBW 	"GPVBW"	// Dual Ground / Water Speed
#define NMEA_GPVTG 	"GPVTG"	// Track made good and ground speed
#define NMEA_GPWPL 	"GPWPL"	// Waypoint location
#define NMEA_GPXTE 	"GPXTE"	// Cross-track error, Measured
#define NMEA_GPZDA 	"GPZDA"	// Date & Time

int
main(void)
{
	int gps_fd;
	int read_bytes, total_bytes;
	char *buffer, *start, *end;

	buffer = malloc(4096);
	if (NULL == buffer) {
		perror("malloc buffer");
		exit(EXIT_FAILURE);
	}

	gps_fd = open("/dev/ttyUSB0", O_RDONLY);
	if (-1 == gps_fd) {
		perror("open ttyUSB0");
		exit(EXIT_FAILURE);
	}

	while (1) {
		read_bytes = read(gps_fd, buffer + total_bytes, 100);
		if (-1 == read_bytes) {
			perror("read ttyUSB0");
			exit(EXIT_FAILURE);
		}
		total_bytes += read_bytes;

		/* process entire buffer */
		
		/* find start ($) */
		start = memchr(buffer, '$', total_bytes);
		if (NULL == start || start == buffer + total_bytes) {
			total_bytes = 0;
			continue;
		}

		/* find end of line */
		end = memchr(start, '\n', total_bytes - (start - buffer));
		if (NULL == end) {
			continue;
		}

		/* end of buffer? */
		if (end == buffer + total_bytes) {
			total_bytes = 0;
			continue;
		}

		end++;

		/* handle data */
		if (0 == strncmp(start + 1, NMEA_GPGLL, 5)) {
		      write(1, start, end - start);
		}

		/* copy rest of buffer to beginning */
		if (buffer != memmove(buffer, end, total_bytes - (end - buffer))) {
			total_bytes = 0;
			continue;
		}

		total_bytes -= end - buffer;
	}

	close(gps_fd);
	return 0;
}

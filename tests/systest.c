// This file parses standard input and then exits.
// Should be used together with valgrind to check for memory leaks.
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <nmea.h>
#include <nmea/gpgll.h>
#include <nmea/gpgga.h>
#include <nmea/gprmc.h>

int
main(void)
{
	char buffer[4096];
	int gps_fd = 0; // stdin
	int read_bytes = 0;
	int total_bytes = 0;
	char *start, *end;
	sigset_t block_mask;

	while (1) {
		char buf[255];
		nmea_s *data;

		read_bytes = read(gps_fd, buffer + total_bytes, 20);
		if (-1 == read_bytes) {
			perror("read stdin");
			exit(EXIT_FAILURE);
		}
		if (0 == read_bytes) {
			break;
 		}

		total_bytes += read_bytes;

		/* find start (a dollar $ign) */
		start = memchr(buffer, '$', total_bytes);
		if (NULL == start) {
			total_bytes = 0;
			continue;
		}

		/* find end of line */
		end = memchr(start, NMEA_END_CHAR_1, total_bytes - (start - buffer));
		if (NULL == end || NMEA_END_CHAR_2 != *(++end)) {
			continue;
		}

		/* handle data */
		data = nmea_parse(start, end - start + 1, 0);
		if (NULL != data) {
			if (0 < data->errors) {
				printf("{ type: 'GPWRN', data: { message:'The following sentence contains parse errors!' } }\n");
			}

			if (NMEA_GPGGA == data->type) {
				nmea_gpgga_s *gpgga = (nmea_gpgga_s *) data;
				printf("{ type: 'GPGGA', data: { satellites: %d, altitude: '%.0lf%c' } }\n", gpgga->n_satellites, gpgga->altitude, gpgga->altitude_unit);
			}

			if (NMEA_GPGLL == data->type) {
				nmea_gpgll_s *pos = (nmea_gpgll_s *) data;
				strftime(buf, sizeof(buf), "%H:%M:%S", &pos->time);
				printf("{ type: 'GPGLL', data: { long: '%c%d:%f', lat: '%c%d:%f', time: '%s' } }\n",\
					(char) pos->longitude.cardinal, pos->longitude.degrees, pos->longitude.minutes,\
					(char) pos->latitude.cardinal, pos->latitude.degrees, pos->latitude.minutes,\
					buf);
			}

			if (NMEA_GPRMC == data->type) {
				nmea_gprmc_s *pos = (nmea_gprmc_s *) data;
				strftime(buf, sizeof(buf), "%H:%M:%S", &pos->date_time);
				printf("{ type: 'GPRMC', data: { long: '%c%d:%f', lat: '%c%d:%f', time: '%s' } }\n",\
					(char) pos->longitude.cardinal, pos->longitude.degrees, pos->longitude.minutes,\
					(char) pos->latitude.cardinal, pos->latitude.degrees, pos->latitude.minutes,\
					buf);
			}

			nmea_free(data);
		}

		/* buffer empty? */
		if (end == buffer + total_bytes) {
			total_bytes = 0;
			continue;
		}

		/* copy rest of buffer to beginning */
		if (buffer != memmove(buffer, end, total_bytes - (end - buffer))) {
			total_bytes = 0;
			continue;
		}

		total_bytes -= end - buffer;
	}

	return 0;
}

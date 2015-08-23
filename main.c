#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "nmea.h"

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

		*end = '\n';
		end++;

		/* handle data */
		nmea_t type = nmea_get_type(start);
		if (NMEA_GPGLL_T == type) {
			char actual_chk = nmea_get_checksum(start);
			long int expected_chk =strtol(end - 3, NULL, 16);
			if (expected_chk != (long int) actual_chk) {
				fprintf(stderr, "Invalid checksum!\n");
			}
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

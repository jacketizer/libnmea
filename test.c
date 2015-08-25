#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <nmea.h>

int
main(void)
{
	int gps_fd;
	int read_bytes, total_bytes = 0;
	char *buffer, *start, *end;

	buffer = malloc(4096);
	if (NULL == buffer) {
		perror("malloc buffer");
		exit(EXIT_FAILURE);
	}

	gps_fd = 0; // stdin
	//gps_fd = open("/dev/ttyUSB0", O_RDONLY);
	//if (-1 == gps_fd) {
	//	perror("open ttyUSB0");
	//	exit(EXIT_FAILURE);
	//}

	while (1) {
		read_bytes = read(gps_fd, buffer + total_bytes, 20);
		if (-1 == read_bytes) {
			perror("read ttyUSB0");
			exit(EXIT_FAILURE);
		}
		total_bytes += read_bytes;

		/* find start (a dollar $ign) */
		start = memchr(buffer, '$', total_bytes);
		if (NULL == start) {
			total_bytes = 0;
			continue;
		}

		/* find end of line */
		end = memchr(start, '\r', total_bytes - (start - buffer));
		if (NULL == end || '\n' != *(++end)) {
			continue;
		}

		/* handle data */
		nmea_t type = nmea_get_type(start);
		switch (type) {
			case NMEA_UNKNOWN:
				fprintf(stderr, "Unknown NMEA sentence type.\n");
				break;
			case NMEA_GPGLL:
				if (-1 == nmea_validate(start, end - start + 1)) {
					fprintf(stderr, "Invalid NMEA sentence!\n");
					break;
				}

				char **values = malloc(200);
				int n_vals = _nmea_split_sentence(start, end - start + 1, values);
				fprintf(stderr, "Number of values: %d\n", n_vals);
				while (n_vals > 0) {
					fprintf(stderr, "  value[%d]: %s\n", n_vals, values[--n_vals]);
				}
				break;
			default:
				fprintf(stderr, "Unhandled NMEA sentence type.\n");
				
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

	close(gps_fd);
	return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <nmea.h>
#include <gpgll.h>

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
		end = memchr(start, NMEA_END_CHAR_1, total_bytes - (start - buffer));
		if (NULL == end || NMEA_END_CHAR_2 != *(++end)) {
			continue;
		}

		/* handle data */
		nmea_t type = nmea_get_type(start);
		switch (type) {
			case NMEA_UNKNOWN:
				break;
			case NMEA_GPGLL:
				if (-1 == nmea_validate(start, end - start + 1)) {
					fprintf(stderr, "Invalid NMEA sentence!\n");
					break;
				}

				//write(1, start, end - start + 1);
				nmea_gpgll_s *pos = (nmea_gpgll_s *) nmea_parse(start, end - start + 1, NMEA_GPGLL);
				if (NULL != pos && NMEA_GPGLL == pos->type) {
					printf("Longitude:\n");
					printf("  Degrees: %d\n", pos->longitude.degrees);
					printf("  Minutes: %f\n", pos->longitude.minutes);
					printf("  Cardinal: %c\n", pos->longitude.cardinal);
					printf("Latitude:\n");
					printf("  Degrees: %d\n", pos->latitude.degrees);
					printf("  Minutes: %f\n", pos->latitude.minutes);
					printf("  Cardinal: %c\n", pos->latitude.cardinal);
					if (1 == pos->error) {
						printf("The position struct has errors!\n");
					}
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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <nmea.h>
#include <nmea/gpgll.h>
#include <nmea/gpgga.h>
#include <nmea/gprmc.h>
#include <nmea/gpgsa.h>
#include <nmea/gpvtg.h>
#include <nmea/gptxt.h>
#include <nmea/gpgsv.h>

char buffer[4096];
int gps_fd;

/**
 * Callback function when receiving SIGINT signal. Frees memory and quits
 * the application.
 *
 * signum The current signal invoked, always SIGINT.
 */
void
sig_quit(int signum)
{
	close(gps_fd);
	exit(EXIT_SUCCESS);
}

/**
 * Register the signal callback function.
 *
 * Returns RETURN_SUCCESS on success and RETURN_FAILURE on error.
 */
int
register_signals()
{
	struct sigaction kill_action;
	kill_action.sa_handler = sig_quit;

	if (-1 == sigemptyset(&kill_action.sa_mask)) {
		perror("sigemptyset");
		return -1;
	}
	kill_action.sa_flags = 0;
	if (-1 == sigaction(SIGINT, &kill_action, NULL)) {
		perror("sigaction");
		return -1;
	}

	return 0;
}

int
main(void)
{
	int read_bytes, total_bytes = 0;
	char *start, *end;
	sigset_t block_mask;


	gps_fd = STDIN_FILENO;

	/* Register signal handler for SIGINT */
	if (-1 == register_signals()) {
		exit(EXIT_FAILURE);
	}

	/* Prepare signal blocking */
	if (-1 == sigemptyset(&block_mask) || -1 == sigaddset(&block_mask, SIGINT)) {
		perror("prepare signal blocking");
		exit(EXIT_FAILURE);
	}

	while (1) {
		char buf[255];
		nmea_s *data;

		/* Unlock signal */
		sigprocmask(SIG_UNBLOCK, &block_mask, NULL);

		read_bytes = read(gps_fd, buffer + total_bytes, 20);
		if (-1 == read_bytes) {
			perror("read stdin");
			exit(EXIT_FAILURE);
		}
		if (0 == read_bytes) {
			sig_quit(SIGINT);
		}

		total_bytes += read_bytes;

		/* Block signal */
		sigprocmask(SIG_BLOCK, &block_mask, NULL);

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
				printf("WARN: The sentence struct contains parse errors!\n");
			}

			if (NMEA_GPGGA == data->type) {
				printf("GPGGA sentence\n");
				nmea_gpgga_s *gpgga = (nmea_gpgga_s *) data;
				printf("Number of satellites: %d\n", gpgga->n_satellites);
				printf("Altitude: %f %c\n", gpgga->altitude, gpgga->altitude_unit);
			}

			if (NMEA_GPGLL == data->type) {
				printf("GPGLL sentence\n");
				nmea_gpgll_s *pos = (nmea_gpgll_s *) data;
				printf("Longitude:\n");
				printf("  Degrees: %d\n", pos->longitude.degrees);
				printf("  Minutes: %f\n", pos->longitude.minutes);
				printf("  Cardinal: %c\n", (char) pos->longitude.cardinal);
				printf("Latitude:\n");
				printf("  Degrees: %d\n", pos->latitude.degrees);
				printf("  Minutes: %f\n", pos->latitude.minutes);
				printf("  Cardinal: %c\n", (char) pos->latitude.cardinal);
				strftime(buf, sizeof(buf), "%H:%M:%S", &pos->time);
				printf("Time: %s\n", buf);
			}

			if (NMEA_GPRMC == data->type) {
				printf("GPRMC sentence\n");
				nmea_gprmc_s *pos = (nmea_gprmc_s *) data;
				printf("Longitude:\n");
				printf("  Degrees: %d\n", pos->longitude.degrees);
				printf("  Minutes: %f\n", pos->longitude.minutes);
				printf("  Cardinal: %c\n", (char) pos->longitude.cardinal);
				printf("Latitude:\n");
				printf("  Degrees: %d\n", pos->latitude.degrees);
				printf("  Minutes: %f\n", pos->latitude.minutes);
				printf("  Cardinal: %c\n", (char) pos->latitude.cardinal);
				strftime(buf, sizeof(buf), "%d %b %T %Y", &pos->date_time);
				printf("Date & Time: %s\n", buf);
				printf("Speed, in Knots: %f:\n", pos->gndspd_knots);
				printf("Track, in degrees: %f\n", pos->track_deg);
				printf("Magnetic Variation:\n");
				printf("  Degrees: %f\n", pos->magvar_deg);
				printf("  Cardinal: %c\n", (char) pos->magvar_cardinal);
				double adjusted_course = pos->track_deg;
				if (NMEA_CARDINAL_DIR_EAST == pos->magvar_cardinal) {
					adjusted_course -= pos->magvar_deg;
				} else if (NMEA_CARDINAL_DIR_WEST == pos->magvar_cardinal) {
					adjusted_course += pos->magvar_deg;
				} else {
					printf("Invalid Magnetic Variation Direction!!\n");
				}

				printf("Adjusted Track (heading): %f\n", adjusted_course);
			}

			if (NMEA_GPGSA == data->type) {
				nmea_gpgsa_s *gpgsa = (nmea_gpgsa_s *) data;

				printf("GPGSA Sentence:\n");
				printf("  Mode: %c\n", gpgsa->mode);
				printf("  Fix:  %d\n", gpgsa->fixtype);
				printf("  PDOP: %.2lf\n", gpgsa->pdop);
				printf("  HDOP: %.2lf\n", gpgsa->hdop);
				printf("  VDOP: %.2lf\n", gpgsa->vdop);
			}

			if (NMEA_GPGSV == data->type) {
				nmea_gpgsv_s *gpgsv = (nmea_gpgsv_s *) data;

				printf("GPGSV Sentence:\n");
				printf("  Num: %d\n", gpgsv->sentences);
				printf("  ID:  %d\n", gpgsv->sentence_number);
				printf("  SV:  %d\n", gpgsv->satellites);
				printf("  #1:  %d %d %d %d\n", gpgsv->sat[0].prn, gpgsv->sat[0].elevation, gpgsv->sat[0].azimuth, gpgsv->sat[0].snr);
				printf("  #2:  %d %d %d %d\n", gpgsv->sat[1].prn, gpgsv->sat[1].elevation, gpgsv->sat[1].azimuth, gpgsv->sat[1].snr);
				printf("  #3:  %d %d %d %d\n", gpgsv->sat[2].prn, gpgsv->sat[2].elevation, gpgsv->sat[2].azimuth, gpgsv->sat[2].snr);
				printf("  #4:  %d %d %d %d\n", gpgsv->sat[3].prn, gpgsv->sat[3].elevation, gpgsv->sat[3].azimuth, gpgsv->sat[3].snr);
			}

			if (NMEA_GPTXT == data->type) {
				nmea_gptxt_s *gptxt = (nmea_gptxt_s *) data;

				printf("GPTXT Sentence:\n");
				printf("  ID: %d %d %d\n", gptxt->id_00, gptxt->id_01, gptxt->id_02);
				printf("  %s\n", gptxt->text);
			}

			if (NMEA_GPVTG == data->type) {
				nmea_gpvtg_s *gpvtg = (nmea_gpvtg_s *) data;

				printf("GPVTG Sentence:\n");
				printf("  Track [deg]:   %.2lf\n", gpvtg->track_deg);
				printf("  Speed [kmph]:  %.2lf\n", gpvtg->gndspd_kmph);
				printf("  Speed [knots]: %.2lf\n", gpvtg->gndspd_knots);
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

	close(gps_fd);
	return 0;
}

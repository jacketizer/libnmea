#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <nmea.h>
#include "../minunit.h"

int tests_run = 0;

static char *
test_get_type_ok()
{
	char *sentence;
	nmea_t res;

	sentence = strdup("$GPGLL,4916.45,N,12311.12,W,225444,A,*1D\r\n");
	res = nmea_get_type(sentence);
	mu_assert("should return correct type (GPGLL)", NMEA_GPGLL == res);
	free(sentence);

	sentence = strdup("$GPGGA,4916.45,N,12311.12,W,225444,A\r\n");
	res = nmea_get_type(sentence);
	mu_assert("should return correct type (GPGGA)", NMEA_GPGGA == res);
	free(sentence);

	sentence = strdup("$GNGGA,4916.45,N,12311.12,W,225444,A\r\n");
	res = nmea_get_type(sentence);
	mu_assert("should return correct type (GPGGA) even if talker is GN", NMEA_GPGGA == res);
	free(sentence);

	return 0;
}

static char *
test_get_type_unknown()
{
	char *sentence;
	nmea_t res;

	sentence = strdup("THISISWRONG");
	res = nmea_get_type(sentence);
	mu_assert("should return NMEA_UNKNOWN on unknown sentence type", NMEA_UNKNOWN == res);
	free(sentence);

	sentence = strdup("$UNKNOWN");
	res = nmea_get_type(sentence);
	mu_assert("should return NMEA_UNKNOWN on unknown sentence type", NMEA_UNKNOWN == res);
	free(sentence);

	sentence = strdup("");
	res = nmea_get_type(sentence);
	mu_assert("should return nmea_unknown on empty sentence", NMEA_UNKNOWN == res);
	free(sentence);

	return 0;
}

static char *
test_get_checksum_with_crc()
{
	// Sentence with checksum (0x1D == 29)
	char *sentence = strdup("$GPGLL,4916.45,N,12311.12,W,225444,A,*1D\r\n");
	uint8_t res = nmea_get_checksum(sentence);
	mu_assert("should return correct checksum", 29 == res);
	free(sentence);

	return 0;
}

static char *
test_get_checksum_without_crc()
{
	// Sentence without checksum (0x1D == 29)
	char *sentence = strdup("$GPGLL,4916.45,N,12311.12,W,225444,A,\r\n");
	uint8_t res = nmea_get_checksum(sentence);
	mu_assert("should return correct checksum", 29 == res);
	free(sentence);

	return 0;
}

static char *
test_has_checksum_yes()
{
	// Sentence with checksum
	char *sentence = strdup("$GPGLL,4916.45,N,12311.12,W,225444,A*1D\r\n");
	int res = nmea_has_checksum(sentence, strlen(sentence));
	mu_assert("should return 0 when sentence has a checksum", 0 == res);
	free(sentence);

	return 0;
}

static char *
test_has_checksum_no()
{
	// Sentence without checksum
	char *sentence = strdup("$GPGLL,4916.45,N,12311.12,W,225444,A\r\n");
	int res = nmea_has_checksum(sentence, strlen(sentence));
	mu_assert("should return -1 when sentence does not have a checksum", -1 == res);
	free(sentence);

	return 0;
}

static char *
test_validate_ok_with_crc()
{
	// Valid sentence with checksum
	char *sentence = strdup("$GPGLL,4916.45,N,12311.12,W,225444,A,*1D\r\n");
	int res = nmea_validate(sentence, strlen(sentence), 1);
	mu_assert("should return 0 when sentence is valid", 0 == res);
	free(sentence);

	return 0;
}

static char *
test_validate_ok_without_crc()
{
	char *sentence;
	int res;

	// Valid sentence without checksum
	sentence = strdup("$GPGLL,4916.45,N,12311.12,W,225444,A\r\n");
	res = nmea_validate(sentence, strlen(sentence), 1);
	mu_assert("should return 0 when sentence is valid", 0 == res);
	free(sentence);

	// Valid sentence with invalid checksum
	sentence = strdup("$GPGLL,4916.45,N,12311.12,W,225444,A*FF\r\n");
	res = nmea_validate(sentence, strlen(sentence), 0);
	mu_assert("should return 0 when check_checksum is 0 and crc is invalid", 0 == res);
	free(sentence);

	return 0;
}

static char *
test_validate_fail_type()
{
	// Invalid sentence type
	char *sentence = strdup("$GPgll,4916.45,N,12311.12,W,225444,A\r\n");
	int res = nmea_validate(sentence, strlen(sentence), 1);
	mu_assert("should return -1 when sentence type is invalid", -1 == res);
	free(sentence);

	return 0;
}

static char *
test_validate_fail_start()
{
	// Invalid sentence start (no $ sign)
	char *sentence = strdup("£GPGLL,4916.45,N,12311.12,W,225444,A\r\n");
	int res = nmea_validate(sentence, strlen(sentence), 1);
	mu_assert("should return -1 when sentence start is invalid", -1 == res);
	free(sentence);

	return 0;
}

static char *
test_validate_fail_end()
{
	char *sentence;
	int res;

	// Invalid sentence ending (no \n)
	sentence = strdup("$GPGLL,4916.45,N,12311.12,W,225444,A");
	res = nmea_validate(sentence, strlen(sentence), 1);
	mu_assert("should return -1 when sentence ending is invalid", -1 == res);
	free(sentence);

	// Too short sentence
	sentence = strdup("$GP");
	res = nmea_validate(sentence, strlen(sentence), 1);
	mu_assert("should return -1 when sentence is too short", -1 == res);
	free(sentence);

	return 0;
}

static char *
test_validate_fail_empty()
{
	// Invalid sentence (empty string)
	char *sentence = strdup("");
	int res = nmea_validate(sentence, strlen(sentence), 1);
	mu_assert("should return -1 when sentence is empty", -1 == res);
	free(sentence);

	return 0;
}

static char *
test_parse_ok()
{
	char *sentence;
	nmea_s *res;

	// With crc\n
	sentence = strdup("$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n");
	res = nmea_parse(sentence, strlen(sentence), 1);
	mu_assert("should be able to parse a GPGGA sentence", NULL != res);
	free(sentence);
	nmea_free(res);

	// With invalid crc, but check disabled
	sentence = strdup("$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*FF\r\n");
	res = nmea_parse(sentence, strlen(sentence), 0);
	mu_assert("should be able to parse a GPGGA sentence", NULL != res);
	free(sentence);
	nmea_free(res);

	return 0;
	}

static char *
test_parse_unknown()
{
	char *sentence;
	nmea_s *res;

	sentence = strdup("$JACK1,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n");
	res = nmea_parse(sentence, strlen(sentence), 1);
	mu_assert("should return NULL when sentence type is unknown", NULL == res);
	free(sentence);
	nmea_free(res);

	return 0;
}

static char *
test_parse_invalid()
{
	char *sentence;
	nmea_s *res;

	sentence = strdup("$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*FF\r\n");
	res = nmea_parse(sentence, strlen(sentence), 1);
	mu_assert("should return NULL when checksum is invalid", NULL == res);
	free(sentence);
	nmea_free(res);

	sentence = strdup("");
	res = nmea_parse(sentence, strlen(sentence), 1);
	mu_assert("should return NULL when sentence is empty", NULL == res);
	free(sentence);
	nmea_free(res);

	sentence = strdup("invalid");
	res = nmea_parse(sentence, strlen(sentence), 1);
	mu_assert("should return NULL when sentence is invalid", NULL == res);
	free(sentence);
	nmea_free(res);

	res = nmea_parse(NULL, 0, 1);
	mu_assert("should return NULL when sentence is NULL", NULL == res);
	nmea_free(res);

	return 0;
}

static char *
all_tests()
{
	mu_group("nmea_get_type()");
	mu_run_test(test_get_type_ok);
	mu_run_test(test_get_type_unknown);

	mu_group("nmea_get_checksum()");
	mu_run_test(test_get_checksum_with_crc);
	mu_run_test(test_get_checksum_without_crc);

	mu_group("nmea_has_checksum()");
	mu_run_test(test_has_checksum_yes);
	mu_run_test(test_has_checksum_no);

	mu_group("nmea_validate()");
	mu_run_test(test_validate_ok_with_crc);
	mu_run_test(test_validate_ok_without_crc);

	mu_run_test(test_validate_fail_type);
	mu_run_test(test_validate_fail_start);
	mu_run_test(test_validate_fail_end);
	mu_run_test(test_validate_fail_empty);

	mu_group("nmea_parse()");
	mu_run_test(test_parse_ok);
	mu_run_test(test_parse_unknown);
	mu_run_test(test_parse_invalid);
	return 0;
}

int
main(void)
{
	tests_run = 0;

	char *result = all_tests();
	if (result != 0) {
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

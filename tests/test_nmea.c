#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <nmea.h>
#include <nmea/gpgll.h>

#include "minunit.h"

int tests_run = 0;

static char *
test_get_checksum_with_crc()
{
	// Sentence with checksum (0x1D == 29)
	const char *sentence = strdup("$GPGLL,4916.45,N,12311.12,W,225444,A,*1D\n\n");
  uint8_t res = nmea_get_checksum(sentence);
  mu_assert("nmea_get_checksum() should return correct checksum", 29 == res);

  return 0;
}

static char *
test_get_checksum_without_crc()
{
	// Sentence without checksum (0x1D == 29)
	const char *sentence = strdup("$GPGLL,4916.45,N,12311.12,W,225444,A,\n\n");
  uint8_t res = nmea_get_checksum(sentence);
  mu_assert("nmea_get_checksum() should return correct checksum", 29 == res);

  return 0;
}

static char *
test_get_checksum_too_long_sentence()
{
	// Sentence without correct ending (ex: \r\n)
	const char *sentence = strdup("$GPGLL,4916.45,N,12311.12,W,225444,A,");
  uint8_t res = nmea_get_checksum(sentence);
  mu_assert("nmea_get_checksum() should return 0 when sentence is too long", 0 == res);

  return 0;
}

static char *
test_has_checksum_yes()
{
	// Sentence with checksum
	const char *sentence = strdup("$GPGLL,4916.45,N,12311.12,W,225444,A*1D\n\n");
  int res = nmea_has_checksum(sentence, strlen(sentence));
  mu_assert("nmea_has_checksum() should return 0 when sentence has a checksum", 0 == res);

  return 0;
}

static char *
test_has_checksum_no()
{
	// Sentence without checksum
	const char *sentence = strdup("$GPGLL,4916.45,N,12311.12,W,225444,A\n\n");
  int res = nmea_has_checksum(sentence, strlen(sentence));
  mu_assert("nmea_has_checksum() should return -1 when sentence does not have a checksum", -1 == res);

  return 0;
}

static char *
test_validate_ok_with_crc()
{
	// Valid sentence with checksum
	const char *sentence = strdup("$GPGLL,4916.45,N,12311.12,W,225444,A,*1D\n\n");
  int res = nmea_validate(sentence, strlen(sentence), 1);
  mu_assert("nmea_validate() should return 0 when sentence is valid", 0 == res);

  return 0;
}

static char *
test_validate_ok_without_crc()
{
	// Valid sentence without checksum
	const char *sentence = strdup("$GPGLL,4916.45,N,12311.12,W,225444,A\n\n");
  int res = nmea_validate(sentence, strlen(sentence), 1);
  mu_assert("nmea_validate() should return 0 when sentence is valid", 0 == res);

  return 0;
}

static char *
test_validate_fail_type()
{
	// Invalid sentence type
	const char *sentence = strdup("$GPgll,4916.45,N,12311.12,W,225444,A\n\n");
  int res = nmea_validate(sentence, strlen(sentence), 1);
  mu_assert("nmea_validate() should return -1 when sentence type is invalid", -1 == res);

  return 0;
}

static char *
test_validate_fail_start()
{
	// Invalid sentence start (no $ sign)
	const char *sentence = strdup("£GPGLL,4916.45,N,12311.12,W,225444,A\n\n");
  int res = nmea_validate(sentence, strlen(sentence), 1);
  mu_assert("nmea_validate() should return -1 when sentence start is invalid", -1 == res);

  return 0;
}

static char *
test_validate_fail_end()
{
	// Invalid sentence ending (no \n)
	const char *sentence = strdup("$GPGLL,4916.45,N,12311.12,W,225444,A");
  int res = nmea_validate(sentence, strlen(sentence), 1);
  mu_assert("nmea_validate() should return -1 when sentence ending is invalid", -1 == res);

  return 0;
}

static char *
all_tests()
{
  mu_run_test(test_get_checksum_with_crc);
  mu_run_test(test_get_checksum_without_crc);
  mu_run_test(test_get_checksum_too_long_sentence);
  mu_run_test(test_has_checksum_yes);
  mu_run_test(test_has_checksum_no);
  mu_run_test(test_validate_ok_with_crc);
  mu_run_test(test_validate_ok_without_crc);
  mu_run_test(test_validate_fail_type);
  mu_run_test(test_validate_fail_start);
  mu_run_test(test_validate_fail_end);
  return 0;
}

int
main(void)
{
  char *result = all_tests();
  if (result != 0) {
    printf("%s\n", result);
  } else {
    printf("ALL TESTS PASSED\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}

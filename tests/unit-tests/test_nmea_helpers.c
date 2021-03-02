#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../../src/nmea/nmea.c"
#include "../minunit.h"

int tests_run;

int
verify_values(char **values, char **expected, int n)
{
	int i = 0;

	while (i < n) {
		if (0 != strcmp(values[i], expected[i])) {
			return -1;
		}
		i++;
	}

	return 0;
}

static char *
test_split_string_by_comma_ok()
{
	int rv;
	char *test_str;
	char *values[24];

	/* Normal test */
	test_str = strdup("JACK,ENGQVIST,JOHANSSON,89");
	rv = _split_string_by_comma(test_str, values, ARRAY_LENGTH(values));
	mu_assert("should return the correct number of values", 4 == rv);

	char *expected[] = { "JACK", "ENGQVIST", "JOHANSSON", "89" };
	mu_assert("should be able to split a comma seperated string", 0 == verify_values(values, expected, rv));
	free(test_str);

	/* Empty values */
	test_str = strdup(",SOME,EMPTY,VALUES,,");
	rv = _split_string_by_comma(test_str, values, ARRAY_LENGTH(values));
	mu_assert("should return the correct number of values even when there is empty values (,,)", 6 == rv);

	char *expected2[] = { "", "SOME", "EMPTY", "VALUES", "", "" };
	mu_assert("should be able to split empty values (,,)", 0 == verify_values(values, expected2, rv));
	free(test_str);

	return 0;
}

static char *
test_crop_sentence_ok()
{
	char *rv;
	char *test_str;

	/* With checksum */
	test_str = strdup("$GPGGA,ENGQVIST,JOHANSSON,89*D1\r\n");
	rv = _crop_sentence(test_str, strlen(test_str));
	mu_assert("should return a cropped string", 0 == strcmp(rv, "ENGQVIST,JOHANSSON,89"));
	free(test_str);

	/* Without checksum */
	test_str = strdup("$GPGGA,ENGQVIST,JOHANSSON,89\r\n");
	rv = _crop_sentence(test_str, strlen(test_str));
	mu_assert("should return a cropped string without checksum", 0 == strcmp(rv, "ENGQVIST,JOHANSSON,89"));
	free(test_str);

	/* Empty values */
	test_str = strdup("$GPGGA,,ENGQVIST,,JOHANSSON,,89,,\r\n");
	rv = _crop_sentence(test_str, strlen(test_str));
	mu_assert("should work with empty values", 0 == strcmp(rv, ",ENGQVIST,,JOHANSSON,,89,,"));
	free(test_str);

	/* Empty values and checksum */
	test_str = strdup("$GPGGA,,ENGQVIST,,JOHANSSON,,89,,*1D\r\n");
	rv = _crop_sentence(test_str, strlen(test_str));
	mu_assert("should work with empty values and checksum", 0 == strcmp(rv, ",ENGQVIST,,JOHANSSON,,89,,"));
	free(test_str);

	return 0;
}

static char *
test_is_value_set()
{
	int rv;

	/* With regular string */
	rv = _is_value_set("JACK");
	mu_assert("should return 0 when there is a value", 0 == rv);

	/* With empty string */
	rv = _is_value_set("");
	mu_assert("should return -1 when the string is empty", -1 == rv);

	/* With NULL */
	rv = _is_value_set(NULL);
	mu_assert("should return -1 on NULL", -1 == rv);

	return 0;
}

static char *
all_tests()
{
	mu_group("_split_string_by_comma()");
	mu_run_test(test_split_string_by_comma_ok);

	mu_group("_crop_sentence()");
	mu_run_test(test_crop_sentence_ok);

	mu_group("_is_value_set()");
	mu_run_test(test_is_value_set);

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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../../src/parsers/parse.h"
#include "../minunit.h"

int tests_run = 0;

static char *
test_position_parse_ok()
{
	int res;
	char *s;
	nmea_position *pos = malloc(sizeof (nmea_position));

	/* 3 digit degrees */
	s = strdup("12311.12");
	res = nmea_position_parse(s, pos);
	mu_assert("should return return 0 when successfull", 0 == res);
	mu_assert("pos->degrees should be 123", 123 == pos->degrees);
	mu_assert("pos->minutes should be 11.12", 11.12 == pos->minutes);
	free(s);

	/* 2 digit degrees */
	s = strdup("2621.44");
	res = nmea_position_parse(s, pos);
	mu_assert("should return return 0 when successfull", 0 == res);
	mu_assert("pos->degrees should be 123", 26 == pos->degrees);
	mu_assert("pos->minutes should be 11.12", 21.44 == pos->minutes);
	free(s);

	/* 1 digit degrees */
	s = strdup("901.01");
	res = nmea_position_parse(s, pos);
	mu_assert("should return return 0 when successfull", 0 == res);
	mu_assert("pos->degrees should be 123", 9 == pos->degrees);
	mu_assert("pos->minutes should be 11.12", 1.01 == pos->minutes);
	free(s);

	/* 0 decimals in minutes */
	s = strdup("901.");
	res = nmea_position_parse(s, pos);
	mu_assert("should return return 0 when successfull", 0 == res);
	mu_assert("pos->degrees should be 123", 9 == pos->degrees);
	mu_assert("pos->minutes should be 11.12", 1.0 == pos->minutes);
	free(s);

	free(pos);
	return 0;
}

static char *
test_position_parse_fail()
{
	int res;
	char *s;
	nmea_position *pos = malloc(sizeof (nmea_position));

	/* no minutes */
	s = strdup("123");
	res = nmea_position_parse(s, pos);
	mu_assert("should return return -1 on failure", -1 == res);
	free(s);

	/* empty string */
	s = strdup("");
	res = nmea_position_parse(s, pos);
	mu_assert("should return return -1 on empty string", -1 == res);
	free(s);

	/* NULL */
	res = nmea_position_parse(NULL, pos);
	mu_assert("should return return -1 on NULL", -1 == res);

	free(pos);
	return 0;
}

static char *
test_cardinal_dir_parse_ok()
{
	nmea_cardinal_t card;

	/* North */
	card = nmea_cardinal_direction_parse("N");
	mu_assert("should return NMEA_CARDINAL_NORTH on \"N\"", NMEA_CARDINAL_DIR_NORTH == card);

	/* East */
	card = nmea_cardinal_direction_parse("E");
	mu_assert("should return NMEA_CARDINAL_EAST on \"E\"", NMEA_CARDINAL_DIR_EAST == card);

	/* South */
	card = nmea_cardinal_direction_parse("S");
	mu_assert("should return NMEA_CARDINAL_SOUTH on \"S\"", NMEA_CARDINAL_DIR_SOUTH == card);

	/* West */
	card = nmea_cardinal_direction_parse("W");
	mu_assert("should return NMEA_CARDINAL_WEST on \"W\"", NMEA_CARDINAL_DIR_WEST == card);

	return 0;
}

static char *
test_cardinal_dir_parse_fail()
{
	nmea_cardinal_t card;

	/* Empty string */
	card = nmea_cardinal_direction_parse("");
	mu_assert("should return NMEA_CARDINAL_UNKNOWN on empty string", NMEA_CARDINAL_DIR_UNKNOWN == card);

	/* NULL */
	card = nmea_cardinal_direction_parse(NULL);
	mu_assert("should return NMEA_CARDINAL_UNKNOWN on NULL", NMEA_CARDINAL_DIR_UNKNOWN == card);

	/* Unknown character (J) */
	card = nmea_cardinal_direction_parse("J");
	mu_assert("should return NMEA_CARDINAL_UNKNOWN on \"J\"", NMEA_CARDINAL_DIR_UNKNOWN == card);

	/* Unknown character (n) */
	card = nmea_cardinal_direction_parse("n");
	mu_assert("should return NMEA_CARDINAL_UNKNOWN on \"n\"", NMEA_CARDINAL_DIR_UNKNOWN == card);

	return 0;
}

static char *
test_time_parse_ok()
{
	int rv;
	struct tm t;

	/* Parse time, no leading zeros */
	rv = nmea_time_parse("171521", &t);
	mu_assert("should return 0 on success", 0 == rv);
	mu_assert("t.tm_sec should be 21", 21 == t.tm_sec);
	mu_assert("t.tm_min should be 15", 15 == t.tm_min);
	mu_assert("t.tm_hour should be 17", 17 == t.tm_hour);

	/* Parse time, with leading zeros */
	rv = nmea_time_parse("090801", &t);
	mu_assert("should return 0 on success", 0 == rv);
	mu_assert("t.tm_sec should be 1", 1 == t.tm_sec);
	mu_assert("t.tm_min should be 8", 8 == t.tm_min);
	mu_assert("t.tm_hour should be 9", 9 == t.tm_hour);

	return 0;
}

static char *
test_time_parse_fail()
{
	int rv;
	struct tm t;

	/* Invalid time, hours over 23 */
	rv = nmea_time_parse("240000", &t);
	mu_assert("should return -1 when hours is over 23", -1 == rv);

	/* Invalid time, minutes over 59 */
	rv = nmea_time_parse("006000", &t);
	mu_assert("should return -1 when minutes is over 59", -1 == rv);

	/*
	* Invalid time, seconds over 61 (tm_sec is generally 0-59. The extra range is
	* to accommodate for leap seconds in certain systems).
	**/
	rv = nmea_time_parse("000062", &t);
	mu_assert("should return -1 when seconds is over 59", -1 == rv);

	/* Too short string */
	rv = nmea_time_parse("2345", &t);
	mu_assert("should return -1 on too short string", -1 == rv);

	/* Invalid characters */
	rv = nmea_time_parse("2345XX", &t);
	mu_assert("should return -1 on invalid characters", -1 == rv);

	/* Empty string */
	rv = nmea_time_parse("", &t);
	mu_assert("should return -1 on empty string", -1 == rv);

	/* NULL */
	rv = nmea_time_parse(NULL, &t);
	mu_assert("should return -1 on NULL", -1 == rv);

	return 0;
}

static char *
all_tests()
{
	mu_group("nmea_position_parse()");
	mu_run_test(test_position_parse_ok);
	mu_run_test(test_position_parse_fail);

	mu_group("nmea_cardinal_dir_parse()");
	mu_run_test(test_cardinal_dir_parse_ok);
	mu_run_test(test_cardinal_dir_parse_fail);

	mu_group("nmea_time_parse()");
	mu_run_test(test_time_parse_ok);
	mu_run_test(test_time_parse_fail);

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

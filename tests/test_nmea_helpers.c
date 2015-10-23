#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../src/nmea/nmea.c"
#include "minunit.h"

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
test_split_sentence_ok()
{
  int rv;
  char *test_str;
  char **values = malloc((sizeof (char *)) * 24);

  test_str = strdup("$ABCDE,JACK,ENGQVIST,JOHANSSON,89");
  rv = _split_sentence(test_str, strlen(test_str), values);
  mu_assert("should return the correct number of values", 4 == rv);

  test_str = strdup("$ABCDE,,SOME,EMPTY,VALUES,,");
  rv = _split_sentence(test_str, strlen(test_str), values);
  mu_assert("should return the correct number of values even when there is empty values (,,)", 6 == rv);
  char *expected[6] = { "", "SOME", "EMPTY", "VALUES", "", "" };
  mu_assert("should be able to handle empty values (,,)", 0 == verify_values(values, expected, rv));

  return 0;
}

static char *
all_tests()
{
  mu_group("_split_sentence()");
  mu_run_test(test_split_sentence_ok);

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

#define mu_assert(message, test) do { printf("."); tests_run++; if (!(test)) return message; } while (0)
#define mu_run_test(test) do { char *message = test(); \
                                   if (message) return message; } while (0)
 extern int tests_run;

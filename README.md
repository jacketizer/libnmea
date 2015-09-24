C Library for Parsing NMEA 0183 Sentences
-----------------------------------------

The library can validate and parse NMEA sentences into C structs. If you find
any sentence missing, please add it by contributing to the code.

To build:

```sh
$ make parser-libs && make nmea && make test
```

To test:

```sh
$ export LD_LIBRARY_PATH=./
$ echo -ne "\$GPGLL,4916.45,N,12311.12,W,225444,A,*1D\n\n" | ./test
```

#!/bin/bash
# This script tests all the NMEA sentences by piping in a NMEA stream to stdin
# and then verifies the output.
# To run: tests/systests.sh

# GPGLL
EXP=$'{ type: \'GPGLL\', data: { long: \'W123:11.120000\', lat: \'N49:16.450000\', time: \'22:54:44\' } }\n'
EXP+=$'{ type: \'GPGLL\', data: { long: \'W123:11.120000\', lat: \'N49:16.450000\', time: \'22:54:44\' } }\n'
EXP+=$'{ type: \'GPGLL\', data: { long: \'W123:11.120000\', lat: \'N49:16.450000\', time: \'22:54:44\' } }\n'

# GPGGA
EXP+=$'{ type: \'GPGGA\', data: { satellites: 8, altitude: \'545M\' } }\n'
EXP+=$'{ type: \'GPGGA\', data: { satellites: 8, altitude: \'545M\' } }\n'
EXP+=$'{ type: \'GPGGA\', data: { satellites: 8, altitude: \'545M\' } }\n'

# GPRMC
EXP+=$'{ type: \'GPRMC\', data: { long: \'E11:31.000000\', lat: \'N48:7.038000\', time: \'12:35:19\' } }'

RESULT=$(tests/output-nmea.sh | ./systest)

if [ $? -eq 0 ]; then
	if [ "$EXP" == "$RESULT" ]; then
		echo "All tests passed!"
		exit 0;
	fi
	echo "Tests failed! Output is not correct."
	echo "Expected:"
	echo "$EXP"
	echo "Actual:"
	echo "$RESULT"
	exit 1
fi

echo "Tests failed! Test program exited with status $?."
exit 1

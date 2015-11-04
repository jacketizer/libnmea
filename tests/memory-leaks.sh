#!/bin/sh
# This script tests the stdin example for memory leaks by piping in some
# NMEA sentences.
# To run: tests/memory-leaks.sh

tests/output-nmea.sh | valgrind --leak-check=full --track-origins=yes --error-exitcode=1 ./memcheck

if [ $? -eq 0 ]; then
  echo "No memory leaks found!"
  exit 0
fi

echo "Memory leaks found!"
exit 1

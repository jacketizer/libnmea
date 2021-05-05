#!/bin/bash

if [[ -z "$1" ]] || [[ ! -x "$1" ]]; then
    echo "usage: $0 <path/to/parse_stdin>"
    exit 1
fi

set -euo pipefail
TESTS_PATH=$(dirname $0)

$1 <${TESTS_PATH}/parse_stdin_test_in.txt >parse_stdin_test_out.txt
diff parse_stdin_test_out.txt ${TESTS_PATH}/parse_stdin_test_out_expected.txt


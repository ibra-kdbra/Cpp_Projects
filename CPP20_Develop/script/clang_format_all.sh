#!/bin/bash

THIS_PATH="$(realpath "$0")"
THIS_DIR="$(dirname "$THIS_PATH")"
PROJECT_DIR="$(dirname "$THIS_DIR")"
# echo -e $PROJECT_DIR

FILE_LIST="$(find "$PROJECT_DIR" -not -path '*/googletest/*' | grep -E ".*(\.ino|\.cpp|\.c|\.h|\.hpp|\.hh)$")"
# echo -e "Files found to format = \n\"\"\"\n$FILE_LIST\n\"\"\""

# note: chunks file_list in 250 entry batches to not exceed path limits
xargs -n 250 clang-format --verbose -i --style=file --fallback-style=none <<<$FILE_LIST

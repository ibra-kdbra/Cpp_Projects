#!/bin/bash
set -e # stop on first error

# default arguments:
#   script/test_cmake.sh clang debug
ConfigPreset="${1:-clang}"
Config="${2:-debug}"
BuildPreset="${ConfigPreset}-${Config}"
TestPreset="test-${BuildPreset}"

BUILD_PATH="build/${ConfigPreset}"

cd "$(dirname "${BASH_SOURCE[0]}")/.."

if [ ! -f "${BUILD_PATH}/compile_commands.json" ] ; then
  cmake -S . --preset "${ConfigPreset}"
fi
cmake --build --preset "${BuildPreset}"

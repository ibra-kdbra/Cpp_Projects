#!/bin/bash
set -e # stop on first error

# default arguments:
#   script/create_conan.sh gcc12
ConanProfile=${1:-gcc12}
shift

cd "$(dirname "${BASH_SOURCE[0]}")/.."
BASE_DIR="$(pwd)"

cd conan
conan create . --build=missing --profile "profiles/${ConanProfile}"

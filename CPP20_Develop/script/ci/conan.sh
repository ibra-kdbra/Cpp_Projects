#!/bin/bash
set -e # stop on first error

# default arguments:
#   script/create_conan.sh gcc12
ConanProfile=${1:-gcc12}
shift

cd "$(dirname "${BASH_SOURCE[0]}")/../.."
BASE_DIR="$(pwd)"

script/create_conan.sh "${ConanProfile}"

for test in conan/tests/*/
do
  script/test_conan.sh "$(basename "${test}")" --profile "${BASE_DIR}/conan/profiles/${ConanProfile}"
done

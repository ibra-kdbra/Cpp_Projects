#!/bin/bash
if [ ! -d "$(dirname "$0")/googletest" ]; then
    git clone --depth 1 --no-tags https://github.com/google/googletest.git "$(dirname "$0")/googletest"
fi
pushd "$(dirname "$0")/googletest"
git fetch origin 210aab09deef06e64ab82414697abd0d7cb5978a
git reset --hard FETCH_HEAD
popd

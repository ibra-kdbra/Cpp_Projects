@echo off

set "PROJECT=co-cpp19"
set "IMAGE=arbmind/qbs-clang-libstdcpp:latest"
set "BUILD_VOLUME=%PROJECT%-qbs-build"

pushd "%~dp0.."
docker run -it --rm --mount "src=%cd%,target=/project,type=bind" -w /project --mount "src=%BUILD_VOLUME%,target=/build,type=volume" %IMAGE% build --build-directory /build --file %project%.qbs -p autotest-runner
popd

@echo off

set "PROJECT=co-cpp19"
set "IMAGE=arbmind/cmake-clang:latest"
set "BUILD_VOLUME=%PROJECT%-cmake-build"

pushd "%~dp0.."
docker run -it --rm --mount src="%cd%",target=/project,type=bind -w /project --mount src=%BUILD_VOLUME%,target=/project/build,type=volume %IMAGE% %*
popd

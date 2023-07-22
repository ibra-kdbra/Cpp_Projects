FROM mcr.microsoft.com/devcontainers/base:alpine-3.17

RUN \
  apk add --update --no-cache \
    clang15 \
    cmake \
    ninja \
    git \
    gdb \
    linux-headers \
    py3-pip \
  && pip install conan

ENV \
  CC=/usr/bin/clang \
  CXX=/usr/bin/clang++

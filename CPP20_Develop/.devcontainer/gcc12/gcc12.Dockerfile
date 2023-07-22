FROM mcr.microsoft.com/devcontainers/base:alpine-3.17

RUN \
  apk add --update --no-cache \
    build-base \
    cmake \
    ninja \
    git \
    gdb \
    linux-headers \
    py3-pip \
  && pip install conan

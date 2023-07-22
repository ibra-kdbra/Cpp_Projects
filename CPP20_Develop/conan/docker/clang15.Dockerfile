FROM alpine:3.17

RUN \
  apk add --update --no-cache \
    bash \
    build-base \
    clang15 \
    cmake \
    ninja \
    py3-pip \
  && pip install conan

ENV \
  CC=/usr/bin/clang \
  CXX=/usr/bin/clang++

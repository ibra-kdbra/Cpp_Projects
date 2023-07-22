FROM alpine:3.17

RUN \
  apk add --update --no-cache \
    bash \
    build-base \
    cmake \
    ninja \
    py3-pip \
  && pip install conan

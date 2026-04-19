FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=America/Detroit

RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update && apt-get install -y \
    build-essential \
    clang \
    libsdl2-dev \
    libsdl2-image-dev \
    libsdl2-mixer-dev \
    libsdl2-ttf-dev \
    lua5.4 \
    liblua5.4-dev \
    && rm -rf /var/lib/apt/lists/*

FROM ubuntu:24.04

# Wyłączenie interaktywnych zapytań podczas instalacji
ENV DEBIAN_FRONTEND=noninteractive

# Instalacja niezbędnych narzędzi
RUN apt-get update && apt-get install -y \
    gcc-arm-none-eabi \
    libnewlib-arm-none-eabi \
    gdb-multiarch \
    build-essential \
    cmake \
    ninja-build \
    git \
    python3 \
    stlink-tools \
    openocd \
    usbutils \
    && rm -rf /var/lib/apt/lists/*

# Domyślny katalog roboczy
WORKDIR /workspace
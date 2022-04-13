# docker build . -t elfexcavator-dev
# docker run --rm -it --name elfexcavator-dev -v"$PWD":"home/host" elfexcavator-dev bash
FROM ubuntu:22.04
RUN export DEBIAN_FRONTEND=NONINTERACTIVE && \
  apt-get update && \
  apt-get install -y && \
    build-essentail \
    cmake \
    git \
    git-lfs \
    libqt5charts5-dev \
    llvm-dev \
    ninja-build \
    qtbase5-dev \
    qtbase5-private-dev \
    qttools5-dev \
  && rm -rf /var/lib/apt/lists/*

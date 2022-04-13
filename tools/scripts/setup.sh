#!/bin/bash
if [ -f "/etc/apt/sources.list" ]; then
  # Install apt packages only if they exist
  # This includes both apt dev dependencies and vcpkg build dependencies
  # https://github.com/microsoft/vcpkg/blob/master/scripts/azure-pipelines/linux/provision-image.sh
  apt-cache --generate pkgnames \
    | grep --line-regexp --fixed-strings \
    -e autoconf \
    -e bison \
    -e cmake \
    -e git \
    -e git-lfs \
    -e gperf \
    -e libgl1-mesa-dev \
    -e libmesa-dev \
    -e libqt5charts5-dev \
    -e libx11-dev \
    -e libxcb-glx0-dev \
    -e libxcb-icccm4-dev \
    -e libxcb-image0-dev \
    -e libxcb-keysyms1-dev \
    -e libxcb-randr0-dev \
    -e libxcb-render-util0-dev \
    -e libxcb-shape0-dev \
    -e libxcb-shm0-dev \
    -e libxcb-sync0-dev \
    -e libxcb-util10-dev \
    -e libxcb-xfixes0-dev \
    -e libxcb-xinerama0-dev \
    -e libxcb-xinput-dev \
    -e libxcb-xkb-dev \
    -e libxcb1-dev \
    -e libxext-dev \
    -e libxfixes-dev \
    -e libxi-dev \
    -e libxkbcommon-dev \
    -e libxrender-dev \
    -e llvm-dev \
    -e ninja-build \
    -e qtbase5-dev \
    -e qtbase5-private-dev \
    -e qttools5-dev \
    | xargs sudo apt-get install -y
else
  echo "Sorry, your platform is currently unsupported."
  echo "If you install the packages listed in setup.sh it will probably work, but no promises!."
fi

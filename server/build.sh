#!/bin/bash

/bin/echo "Building server"

# For whatever reason, wsl.exe from tasks.json doesn't recognize '~' and has different PATH
cmake_dir="/home/$(whoami)/.local/bin"
if [ -d $cmake_dir ]; then
    /bin/echo "Adding $cmake_dir to path"
    PATH="${PATH:+"$PATH:"}$cmake_dir"
fi

if [ -d "build/" ]; then
    rm -r build/
fi

mkdir build
cd build

# TODO: Add custom flags to build script for add'l cmake options
cmake .. -DCMAKE_BUILD_TYPE=Debug
make

if [ $? -eq 0 ]
then
    echo "Success!"
    exit 0
else
    echo "Build failed. Aborting"
    exit 1
fi;
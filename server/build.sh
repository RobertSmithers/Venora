#!/bin/bash

/bin/echo "Building server"

# For whatever reason, wsl.exe from tasks.json doesn't recognize '~' and has different PATH
cmake_dir="/home/$(whoami)/.local/bin"
if [ -d $cmake_dir ]; then
    /bin/echo "Adding $cmake_dir to path"
    PATH="${PATH:+"$PATH:"}$cmake_dir"
fi

rm -r build/
mkdir build
cd build
cmake ..
make

if [ $? -eq 0 ]
then
    echo "Success!"
else
    echo "Build failed. Aborting"
fi;
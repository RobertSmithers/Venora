#!/bin/bash

/bin/echo "Building server"

# gcc server.c -o server
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
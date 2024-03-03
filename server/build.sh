#!/bin/bash

/bin/echo "Building server"

# gcc server.c -o server
mkdir build
cd build
cmake ..
make

if [ $? -eq 0 ]
then
    echo "Success! Running server"
    ./VenoraServer
else
    echo "Build failed. Aborting"
fi;
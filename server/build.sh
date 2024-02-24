#!/bin/bash

/bin/echo "Building server"

gcc server.c -o server

if [ $? -eq 0 ]
then
    echo "Success! Running server"
    ./server
else
    echo "Build failed. Aborting"
fi;
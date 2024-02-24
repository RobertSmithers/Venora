# Creates the server environment
FROM gcc:latest

# Env vars for srv dir subfolders
ENV PROJECT_SRVHOME=/srv

WORKDIR $PROJECT_SRVHOME
RUN mkdir logs

ADD . ${PROJECT_SRVHOME}

WORKDIR $PROJECT_SRVHOME

# RUN gcc -o server server.c

# CMD ["./server"]
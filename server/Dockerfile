# Creates the server environment
FROM gcc:latest

RUN apt-get update && \
    apt-get install -y \
    libcurl4-openssl-dev libjson-c-dev cmake build-essential \
    postgresql-client valgrind gdb liblog4c-dev \
    gdbserver iputils-ping

# Env vars for srv dir subfolders
ENV PROJECT_SRVHOME=/srv

WORKDIR $PROJECT_SRVHOME
RUN mkdir logs

EXPOSE 9393

CMD ["/bin/bash", "-c", "./build.sh && ./start_srv_after_db.sh"]
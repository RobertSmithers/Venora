# Creates the server environment
FROM gcc:latest

RUN apt-get update && \
    apt-get install -y \
    cmake postgresql-client valgrind gdb build-essential \
    gdbserver iputils-ping

# Env vars for srv dir subfolders
ENV PROJECT_SRVHOME=/srv

WORKDIR $PROJECT_SRVHOME
RUN mkdir logs

EXPOSE 9393

CMD ["/bin/bash", "-c", "./build.sh && ./start_srv_after_db.sh"]
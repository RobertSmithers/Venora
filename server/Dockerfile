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

ADD . ${PROJECT_SRVHOME}

# WORKDIR $PROJECT_SRVHOME

EXPOSE 9393
RUN ls
# RUN export DB_PASSWORD="$DB_PASS"
# "export DB_PASSWORD=$(cat /run/secrets/db-password)"

# Should I just move to env and then not make .env public?
CMD ["/bin/bash", "-c", "./build.sh && ./start_srv_after_db.sh"]
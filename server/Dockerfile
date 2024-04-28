# Creates the server environment
FROM gcc:latest

RUN apt-get update && \
    apt-get install -y \
    cmake postgresql-client

# Env vars for srv dir subfolders
ENV PROJECT_SRVHOME=/srv

WORKDIR $PROJECT_SRVHOME
RUN mkdir logs

ADD . ${PROJECT_SRVHOME}

WORKDIR $PROJECT_SRVHOME

# Should I just move to env and then not make .env public?
CMD ["/bin/bash", "-c", "export DB_PASSWORD=$(cat /run/secrets/db-password) && ./build.sh && ./start_srv_after_db.sh"]
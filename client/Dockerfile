FROM python:latest

# Install Python dependencies
ADD requirements.txt /tmp/
RUN pip3 install -U pip
RUN pip3 install --no-cache-dir -r /tmp/requirements.txt

ADD . /client/

# Env vars for client dir subfolders
ENV PROJECT_SRVHOME=/client

WORKDIR $PROJECT_SRVHOME
RUN mkdir logs

CMD ["/bin/bash", "-c", "./install_client_pkg.sh && venora-client"]
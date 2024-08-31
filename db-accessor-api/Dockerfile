FROM python:3.9-slim

RUN apt-get update && apt-get install -y \
        libpq-dev gcc postgresql-client \
        iputils-ping 

WORKDIR /app

COPY requirements.txt /app/

RUN pip install --no-cache-dir -r requirements.txt

COPY . .

EXPOSE 443

# Define environment variable for flask
ENV FLASK_APP=web-service.py

# Run web service when the container launches
CMD ["./start_srv_after_db.sh"]

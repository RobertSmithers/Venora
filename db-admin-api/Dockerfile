FROM python:3.9-slim

WORKDIR /usr/src/app

COPY . .

RUN pip install --no-cache-dir -r requirements.txt

EXPOSE 5000

# Define environment variable for flask
ENV FLASK_APP=web-service.py

# Run web service when the container launches
CMD ["flask", "run", "--host=0.0.0.0"]

#!/bin/bash

if [ -f "server.crt" ]; then
  echo "server.crt already generated"
  exit 0
else
  echo "server.crt not found. Generating..."
fi

# Load .env into env vars
cd ..
if [ -f .env ]; then
  export $(cat .env | grep -v '#' | awk '/=/ {print $1}')
fi
cd SSL/

if [ -z "$SSL_KEY_PASSPHRASE" ]; then
  echo "Error: SSL_KEY_PASSPHRASE not set in .env file."
  exit 1
fi

# openssl req -x509 -newkey rsa:4096 -nodes -out server_cert.pem -passout pass:"$SSL_KEY_PASSPHRASE" -config csr_config.cnf -keyout server_key.pem -days 365
openssl genrsa -des3 -passout pass:"$SSL_KEY_PASSPHRASE" -out server.key 2048
openssl req -new -key server.key -passin pass:"$SSL_KEY_PASSPHRASE" -config csr_config.cnf -out server.csr

# Remove passphrase per https://kracekumar.com/post/54437887454/ssl-for-flask-local-development/
# cp server.key server.key.local

openssl rsa -in server.key -passin pass:"$SSL_KEY_PASSPHRASE" -out server.key
openssl x509 -req -days 365 -in server.csr -signkey server.key -out server.crt

# Copy to shared volume for other containers to use
cp server.crt ../../SSL-Share/
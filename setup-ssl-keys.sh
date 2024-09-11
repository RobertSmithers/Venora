#!/bin/bash

# Check if an argument is provided
if [ -z "$1" ]; then
  echo "Usage: $0 <container-name>"
  exit 1
fi

CONTAINER_NAME=$1

# Verify container folder exists
if [ ! -d "./$CONTAINER_NAME" ]; then
  echo "Error: Folder $CONTAINER_NAME not found"
  exit 1
fi

TARGET_DIR="./$CONTAINER_NAME/SSL"

# Create the target directory if it doesn't exist
mkdir -p "$TARGET_DIR"

# Check if the certificate already exists
if [ -f "$TARGET_DIR/server.crt" ]; then
  echo "$TARGET_DIR/server.crt already generated"
  exit 0
else
  echo "$TARGET_DIR/server.crt not found. Generating..."
fi

# Load .env into env vars
if [ -f "$CONTAINER_NAME/.env" ]; then
  export $(cat "$CONTAINER_NAME/.env" | grep -v '#' | awk '/=/ {print $1}')
fi

if [ -z "$SSL_KEY_PASSPHRASE" ]; then
  echo "Error: SSL_KEY_PASSPHRASE not set in .env file."
  exit 1
fi

# openssl req -x509 -newkey rsa:4096 -nodes -out server_cert.pem -passout pass:"$SSL_KEY_PASSPHRASE" -config csr_config.cnf -keyout server_key.pem -days 365
openssl genrsa -des3 -passout pass:"$SSL_KEY_PASSPHRASE" -out $TARGET_DIR/server.key 2048
openssl req -new -key $TARGET_DIR/server.key -passin pass:"$SSL_KEY_PASSPHRASE" -config $TARGET_DIR/csr_config.cnf -out $TARGET_DIR/server.csr

# Remove passphrase per https://kracekumar.com/post/54437887454/ssl-for-flask-local-development/
# cp server.key server.key.local

openssl rsa -in $TARGET_DIR/server.key -passin pass:"$SSL_KEY_PASSPHRASE" -out $TARGET_DIR/server.key
openssl x509 -req -days 365 -in $TARGET_DIR/server.csr -signkey $TARGET_DIR/server.key -out $TARGET_DIR/server.crt

echo "SSL key and certificate generated in $TARGET_DIR"

# Depending on which container is generating keys, copy to shared volume for clients to use
# db-accessor-api cert should be visible to server
if [ "$CONTAINER_NAME" == "db-accessor-api" -o "$CONTAINER_NAME" == "server" ]; then
  cp "$TARGET_DIR/server.crt" "./SSL/$CONTAINER_NAME/$CONTAINER_NAME.crt"
fi

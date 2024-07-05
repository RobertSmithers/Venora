#!/bin/bash

# PostgreSQL connection parameters
PGHOST="db"
PGPORT="5432"
PGUSER="postgres"
PGDATABASE="VenoraDB"


MAX_RETRIES=30
RETRY_INTERVAL=5

# Check if PostgreSQL is ready
n=0
echo "Waiting for PostgreSQL to become ready..."
echo "WARNING -- All pg connections will soon be prohibited from server. Switch to using the db's API for health checks"
while ! pg_isready -h "$PGHOST" -p "$PGPORT" -U "$PGUSER" -d "$PGDATABASE" >/dev/null 2>&1; do
    if [ $n -ge $MAX_RETRIES ]; then
        echo "Max retries exceeded. PostgreSQL is not ready."
        exit 1
    fi

    n=$((n+1))
    echo "Retrying in $RETRY_INTERVAL seconds (attempt $n/$MAX_RETRIES)..."
    sleep $RETRY_INTERVAL
done

echo "PostgreSQL is ready. Starting server..."

# valgrind --leak-check=full --track-origins=yes ./build/server
./build/server
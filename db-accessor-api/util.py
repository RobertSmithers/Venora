import bcrypt
import psycopg2
from psycopg2 import OperationalError
import os


conn = None

with open('/run/secrets/db-password', 'r') as f:
    db_password = f.read().strip()


def generate_salt():
    return bcrypt.gensalt().decode('utf-8')


def hash_password(password, salt):
    return bcrypt.hashpw(password.encode('utf-8'), salt.encode('utf-8')).decode('utf-8')


# Singleton to setup initial connection and reuse for future requests
def get_conn():
    global conn
    if not conn:
        conn = psycopg2.connect(
            dbname=os.environ.get("POSTGRES_DB"),
            user=os.environ.get("POSTGRES_USER"),
            password=db_password,
            host="db",
            port="5432"
        )
    return conn


# TODO: We should make our own stateful class to handle our use of psycopg2
# Will be especially important if we decide to implement threading
def check_db_connection():
    try:
        cursor = conn.cursor()
        cursor.execute("SELECT 1")
        cursor.close()
        return True
    except OperationalError:
        return False

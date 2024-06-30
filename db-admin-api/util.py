import bcrypt
import psycopg2
import os

with open('/run/secrets/password.txt', 'r') as f:
    db_password = f.read().strip()


def generate_salt():
    return bcrypt.gensalt().decode('utf-8')


def hash_password(password, salt):
    return bcrypt.hashpw(password.encode('utf-8'), salt.encode('utf-8')).decode('utf-8')


def get_conn():
    return psycopg2.connect(
        dbname=os.getenv("POSTGRES_DB"),
        user=os.getenv("POSTGRES_USER"),
        password=db_password,
        host="db",
        port="5432"
    )


salt = generate_salt()
print(salt, hash_password("todoChangeMe", salt))

import bcrypt
import os

SRV_API_KEY = os.environ.get("SRV_API_KEY")


def generate_salt():
    return bcrypt.gensalt().decode('utf-8')


def hash_password(password, salt):
    return bcrypt.hashpw(password.encode('utf-8'), salt.encode('utf-8')).decode('utf-8')


def verify_api_key(api_key):
    return api_key == SRV_API_KEY

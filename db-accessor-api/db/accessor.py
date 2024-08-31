import os

# Database imports
import psycopg2
from psycopg2 import OperationalError, pool
from psycopg2.extras import RealDictCursor

from typing import Dict

from auth import generate_salt, hash_password
from db.setup import DBConn

db_conn = DBConn()

##### db get accessor funcs #####


def db_get_user(username: str):
    conn = db_conn.get_conn()
    try:
        with conn.cursor(cursor_factory=RealDictCursor) as cursor:
            cursor.execute(
                'SELECT * FROM account WHERE username = %s', (username,))
            return cursor.fetchone()
    finally:
        db_conn.put_conn(conn)


def db_get_wperm_user(username: str):
    conn = db_conn.get_conn()
    try:
        with conn.cursor(cursor_factory=RealDictCursor) as cursor:
            cursor.execute(
                'SELECT * FROM wperm WHERE username = %s', (username,))
            return cursor.fetchone()
    finally:
        db_conn.put_conn(conn)


def db_get_pack(uuid: str):
    conn = db_conn.get_conn()
    try:
        with conn.cursor(cursor_factory=RealDictCursor) as cursor:
            cursor.execute(
                '''SELECT cat.category_name, pack.strike_name, pack.description
                FROM account_strike_auth as auth 
                JOIN account as acc on auth.account_id = acc.account_id
                JOIN strike_permissions as perm on perm.permissions_id = auth.strike_permissions_id
                JOIN strike_packs as pack on pack.permissions_group_id = perm.permissions_id
                JOIN categories as cat on cat.category_id = pack.category_id
                WHERE pack.uuid = %s 
                AND acc.username = %s
                ''', (uuid,))
            return cursor.fetchone()
    finally:
        db_conn.put_conn(conn)

##### db insert funcs #####


def db_insert_user(username: str, password: str, token: str, email: str = None):
    conn = db_conn.get_conn()
    try:
        with conn.cursor(cursor_factory=RealDictCursor) as cursor:
            salt = generate_salt()
            passhash = hash_password(password, salt)

            cursor.execute(
                '''INSERT INTO account (username, token, salt, passhash)
                VALUES (%s, %s, %s, %s)
                RETURNING account_id
                ''', (username, token, salt, passhash))

            # TODO: Add email to account

            conn.commit()
            return cursor.fetchone()
    finally:
        db_conn.put_conn(conn)
    return False


def db_insert_pack(pack: Dict[str, str]):
    req_keys = ['category_name', 'permissions_name',
                'strike_name', 'description']

    # Check they all required fields exist
    if not all(key in pack for key in req_keys):
        return False
# Need to fix to account for different tables
    # cursor = conn.cursor()
    # cursor.execute('''
    #     INSERT INTO strike_packs (category_id, permissions_group_id, strike_name, description)
    #     VALUES (%s, %s, %s, %s)
    # ''', (pack['category_id'], pack['permissions_group_id'], pack['strike_name'], pack['description']))
    # conn.commit()
    # # TODO: Account for exec failure:)
    # return True

    return False


def db_pack_exists(uuid: str):
    conn = db_conn.get_conn()
    try:
        with conn.cursor(cursor_factory=RealDictCursor) as cursor:
            cursor.execute(
                '''SELECT 1 FROM strike_packs WHERE uuid = %s''', (uuid,))
            res = cursor.fetchone()
            return res is not None
    finally:
        db_conn.put_conn(conn)
        return False


def has_pack_permission(username: str, uuid: str):
    conn = db_conn.get_conn()
    try:
        with conn.cursor(cursor_factory=RealDictCursor) as cursor:
            # See Venora.png DDL diagram for easier understanding
            cursor.execute(
                '''SELECT 1
                FROM account_strike_auth as auth 
                JOIN account as acc on auth.account_id = acc.account_id
                JOIN strike_packs as pack on pack.permissions_group_id = perm.permissions_id
                WHERE pack.uuid = %s 
                AND acc.username = %s
                ''', (uuid, username,))
            res = cursor.fetchone()
            return res is not None
    finally:
        db_conn.put_conn(conn)


def check_db_connection():
    conn = db_conn.get_conn()
    try:
        with conn.cursor(cursor_factory=RealDictCursor) as cursor:
            cursor.execute("SELECT 1")
            cursor.fetchone()
            return True
    except OperationalError:
        return False
    finally:
        db_conn.put_conn(conn)

from psycopg2 import pool
import os

with open('/run/secrets/db-password', 'r') as f:
    db_password = f.read().strip()


class DBConn():

    db_pool = None

    def __init__(self):
        self._init_db_pool()

    def _init_db_pool(self):
        # https://www.psycopg.org/docs/pool.html
        self.db_pool = pool.SimpleConnectionPool(1, 5, user=os.environ.get('POSTGRES_USER'),
                                                 password=db_password,
                                                 host="db",
                                                 port="5432",
                                                 database=os.getenv('POSTGRES_DB'))

    def get_conn(self):
        return self.db_pool.getconn()

    def put_conn(self, conn):
        return self.db_pool.putconn(conn)

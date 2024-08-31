import datetime
import jwt
import os

from flask_jwt_extended import JWTManager
from flask import Flask


class WebApp:

    def __init__(self):
        self.app = Flask(__name__)
        self.jwt = JWTManager(self.app)
        self._configure_app()

    def _configure_app(self):
        self.app.config['JWT_SECRET_KEY'] = os.environ.get('JWT_SECRET_KEY')
        self.app.config['JWT_ALGORITHM'] = os.environ.get(
            'JWT_ALGORITHM', 'HS256')

    # def generate_server_token(self):
    #     payload = {
    #         'iss': 'VenoraServer',
    #         'exp': datetime.datetime.now(datetime.UTC) + datetime.timedelta(hours=1)
    #     }
    #     secret = self.app.config['JWT_SECRET_KEY']
    #     algorithm = self.app.config['JWT_ALGORITHM']
    #     token = jwt.encode(payload, secret, algorithm=algorithm)
    #     return token

    def generate_jwt(self, username):
        payload = {
            'username': username,
            'exp': datetime.datetime.now(datetime.timezone.utc) + datetime.timedelta(hours=1)
        }
        return jwt.encode(payload, self.app.config['JWT_SECRET_KEY'], algorithm=self.app.config['JWT_ALGORITHM'])

    def verify_jwt(self, token):
        try:
            payload = jwt.decode(
                token, self.app.config['JWT_SECRET_KEY'], algorithm=self.app.config['JWT_ALGORITHM'])
            return payload
        except jwt.ExpiredSignatureError:
            return None
        except jwt.InvalidTokenEror:
            return None

    def run(self, **kwargs):
        self.app.run(**kwargs)

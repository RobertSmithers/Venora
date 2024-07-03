from flask import Flask, request, jsonify
from flask_jwt_extended import JWTManager, create_access_token, jwt_required, get_jwt_identity
import psycopg2
from psycopg2.extras import RealDictCursor
import datetime
import os

from util import get_conn, hash_password

app = Flask(__name__)

app.config['JWT_SECRET_KEY'] = 'your_secret_key'
jwt = JWTManager(app)


conn = get_conn()


@app.route('/login', methods=['POST'])
def login():
    username = request.json.get('username')
    password = hash_password(request.json.get('password'))

    # Replace with actual user authentication
    cursor = conn.cursor(cursor_factory=RealDictCursor)
    cursor.execute(
        'SELECT * FROM wperm WHERE username = %s AND password = %s', (username, password))
    user = cursor.fetchone()
    cursor.close()

    if user:
        access_token = create_access_token(
            identity=user['id'], expires_delta=datetime.timedelta(hours=1))
        return jsonify(access_token=access_token), 200
    else:
        return jsonify({"msg": "Bad username or password"}), 401


@app.route('/packs', methods=['POST'])
@jwt_required()
def add_pack():
    user_id = get_jwt_identity()
    data = request.json
    cursor = conn.cursor()
    cursor.execute('''INSERT INTO packs (uuid, name, description, attacks_file, last_modified, category_id, permission_id)
                      VALUES (%s, %s, %s, %s, %s, %s, %s)''',
                   (data['uuid'], data['name'], data['description'], data['attacks_file'], data['last_modified'], data['category_id'], data['permission_id']))
    conn.commit()
    cursor.close()
    return jsonify({'status': 'Pack added successfully'}), 201


@app.route('/packs/<uuid>', methods=['GET'])
@jwt_required()
def get_pack(uuid):
    user_id = get_jwt_identity()
    cursor = conn.cursor(cursor_factory=RealDictCursor)
    cursor.execute('SELECT * FROM packs WHERE uuid = %s', (uuid,))
    pack = cursor.fetchone()
    cursor.close()
    if pack:
        return jsonify(pack)
    else:
        return jsonify({'error': 'Pack not found'}), 404


if __name__ == '__main__':
    app.run(debug=True)

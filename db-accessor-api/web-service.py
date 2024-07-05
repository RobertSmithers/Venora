from flask import Flask, request, jsonify
from flask_jwt_extended import JWTManager, create_access_token, jwt_required, get_jwt_identity
import psycopg2
from psycopg2.extras import RealDictCursor
import datetime
import os

from util import get_conn, hash_password, check_db_connection

app = Flask(__name__)

app.config['JWT_SECRET_KEY'] = os.environ.get('JWT_SECRET_KEY')
jwt = JWTManager(app)


conn = get_conn()


@app.route('/health', methods=['GET'])
def health():
    # Try again if failed on boot
    conn = get_conn()
    if not conn:
        return jsonify()
    db_online = check_db_connection()
    if db_online:
        return jsonify(status="ok"), 200
    else:
        return jsonify(status="error"), 503


@app.route('/login', methods=['POST'])
def login():
    username = request.json.get('username')
    password = hash_password(request.json.get('password'))

    # Fetch user details
    cursor = conn.cursor(cursor_factory=RealDictCursor)
    cursor.execute('SELECT * FROM wperm WHERE username = %s', (username,))
    user = cursor.fetchone()
    cursor.close()  # Should I keep this open?

    if user and hash_password(password, user['salt']) == user['passhash']:
        access_token = create_access_token(
            identity=user['user_id'], expires_delta=datetime.timedelta(hours=1))
        return jsonify(access_token=access_token), 200
    else:
        return jsonify({"msg": "Bad username or password"}), 401


@app.route('/packs', methods=['POST'])
@jwt_required()
def add_pack():
    user_id = get_jwt_identity()
    data = request.json
    cursor = conn.cursor()
    cursor.execute('''
        INSERT INTO strike_packs (category_id, permissions_group_id, strike_name, description)
        VALUES (%s, %s, %s, %s)
    ''', (data['category_id'], data['permissions_group_id'], data['strike_name'], data['description']))
    conn.commit()
    cursor.close()  # Should I keep this open?
    return jsonify({'status': 'Pack added successfully'}), 201


@app.route('/packs/<uuid>', methods=['GET'])
@jwt_required()
def get_pack(uuid):
    user_id = get_jwt_identity()
    cursor = conn.cursor(cursor_factory=RealDictCursor)
    cursor.execute(
        'SELECT * FROM strike_packs WHERE strike_pack_id = %s', (uuid,))
    pack = cursor.fetchone()
    cursor.close()  # Should I keep this open?
    if pack:
        return jsonify(pack)
    else:
        return jsonify({'error': 'Pack not found'}), 404


if __name__ == '__main__':
    app.run(debug=True, port=80)

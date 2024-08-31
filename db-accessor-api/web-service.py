from flask import Flask, request, jsonify
from flask_jwt_extended import JWTManager, create_access_token, jwt_required, get_jwt_identity

from db.accessor import db_get_user, db_insert_user, db_get_pack, db_pack_exists, db_insert_pack, has_pack_permission, check_db_connection
from util import WebApp
import auth
import ssl


web_app = WebApp()
app = web_app.app


@app.route('/health', methods=['GET'])
def health():
    # Try again if failed on boot
    # conn = db_conn.get_conn()
    # if not conn:
    #     return jsonify(status="databaseError"), 501
    db_online = check_db_connection()
    if db_online:
        return jsonify(status="ok"), 200
    else:
        return jsonify(status="error"), 503


@app.route('/login_user', methods=['POST'])
def login_user():
    data = request.json
    username = data.get('username')
    password = data.get('password')
    if not username or not password:
        return jsonify({"msg": "Missing username or password field"}), 400

    # Fetch user details from db
    user = db_get_user(username=username)

    if user and auth.hash_password(password, user['salt']) == user['passhash']:
        token = web_app.generate_jwt(username)
        return jsonify(access_token=token), 200
    else:
        return jsonify({"msg": "Bad username or password"}), 401


@app.route('/login_srv', methods=['POST'])
def login_srv():
    data = request.json
    username = data.get('username')
    password = data.get('password')
    if not username or not password:
        return jsonify({"msg": "Missing username or password field"}), 400

    # Fetch user details from db
    api_key = request.headers.get('x-api-key')
    user = db_get_user()

    if user and auth.hash_password(password, user['salt']) == user['passhash']:
        token = app.generate_jwt(username)
        return jsonify(access_token=token), 200
    else:
        return jsonify({"msg": "Bad username or password"}), 401


@app.route('/user', methods=['GET', 'POST'])
def get_user():
    if request.method == 'GET':
        username = request.args.get('username')
        if not username:
            return jsonify({"status": "error", "msg": "Missing 'username' field"}), 400

        user_data = db_get_user(username)
        if user_data:
            return jsonify(user_data), 200
        else:
            return jsonify({"status": "error", "msg": "User not found"}), 404

    elif request.method == 'POST':
        data = request.json
        required_fields = ['username', 'password', 'token']

        # Check if all required fields are present
        if not all(field in data for field in required_fields):
            return jsonify({"status": "error", "msg": "Missing required fields"}), 400

        username = data['username']
        password = data['password']
        token = data['token']
        email = data.get('email')

        # Insert the new user into the database
        result = db_insert_user(username, password, token, email=email)
        if result:
            return jsonify({"status": "success", "msg": "User created successfully"}), 200
        else:
            return jsonify({"status": "error", "msg": "Failed to create user"}), 500


@app.route('/packs', methods=['POST'])
@jwt_required()
def add_pack():
    data = request.json
    req_keys = ['category_name', 'permissions_name',
                'strike_name', 'description']

    # Check they all required fields exist
    missing_keys = list(key for key in req_keys if key not in data)
    if missing_keys:
        return jsonify({'status': f'Missing keys: {", ".join(req_keys)}'}), 400

    if (db_insert_pack(data)):
        return jsonify({'status': 'Pack added successfully'}), 201
    else:
        return jsonify({'status': 'Failed to insert pack'}), 503


@app.route('/pack/<uuid>', methods=['GET'])
@jwt_required()
def get_pack(uuid):
    username = get_jwt_identity()
    if not db_pack_exists(uuid):
        return jsonify({'error': 'Pack not found'}), 404

    if has_pack_permission(username, uuid):
        pack = db_get_pack(uuid)
        return jsonify(pack)
    else:
        return jsonify({'error': 'User lacks permissions for this pack'}), 404


if __name__ == '__main__':
    # SSL required between VenoraServer and db-accessor-api
    # context = ssl.SSLContext(ssl.PROTOCOL_TLSv1_2)
    # context.load_cert_chain(certfile='SSL/local.crt', keyfile='SSL/local.key')
    app.run(debug=True, host='0.0.0.0', port=443,
            ssl_context=(
                'SSL/server.crt', 'SSL/server.key'))

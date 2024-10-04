"""
Flask REST API for wrapping a client library and providing a frontend interface.

This file contains the implementation of a Flask REST API that serves as a wrapper around a client library. The purpose of this API is to provide a convenient interface for making calls to the client library from a frontend application. By using this API, the frontend application can easily interact with the client library without having to directly call its functions.

The API is built using Flask, a popular web framework for Python. It exposes a set of endpoints that correspond to different operations that can be performed using the client library. These endpoints accept HTTP requests and forward them to the appropriate functions in the client library.

The API can be used to perform various tasks such as retrieving data, creating new resources, updating existing resources, and deleting resources. It follows the principles of REST (Representational State Transfer) architecture, which allows for a stateless and scalable design.

To use this API, you can make HTTP requests to the specified endpoints using a tool like cURL or a web browser. The API will handle the requests and return the appropriate responses based on the actions performed by the client library.

Note: This docstring only provides a high-level overview of the file and its purpose. For detailed documentation of the API endpoints and their functionalities, please refer to the code comments and the API documentation.
"""

from flask import Flask, jsonify, request
from flask_jwt_extended import JWTManager, create_access_token, jwt_required, get_jwt_identity, verify_jwt_in_request
from functools import wraps

import os
import socket

from client.networking.comms import connect_to_server
from client.logic.actions import (
    send_register_request, receive_register_response,
    send_login_request, receive_login_response
)

app = Flask(__name__)
app.config['JWT_SECRET_KEY'] = os.environ.get('JWT_SECRET_KEY')
jwt = JWTManager(app)

# Initialize the connection pool
connections = {}


def get_connection(user_id):
    if user_id not in connections:
        connections[user_id] = connect_to_server()
    return connections[user_id]


def handle_disconnect(user_id):
    if user_id in connections:
        conn = connections[user_id]
        conn.close()
        del connections[user_id]


def is_connected(fn):
    @wraps(fn)
    def wrapper(*args, **kwargs):
        try:
            verify_jwt_in_request()
            user_id = get_jwt_identity()
            if user_id not in connections:
                return jsonify({"status": "failure", "error": "Not connected to Venora server"}), 401
            return fn(*args, **kwargs)
        except Exception as e:
            return jsonify({"status": "failure", "error": str(e)}), 500
    return wrapper


@app.route('/connect', methods=['POST'])
def connect():
    '''This is the very first endpoint that should be called by the frontend to establish a connection with the server.'''
    user_data = request.json
    user_id = user_data['username']

    # Check if user already is connect
    if connections.get(user_id):
        return jsonify({"status": "success", "data": "Already connected"}), 200

    # Connect and store the connection
    get_connection(user_id)
    access_token = create_access_token(identity=user_id)
    return jsonify(access_token=access_token), 200


@app.route('/register', methods=['POST'])
@is_connected
def register():
    user_data = request.json
    user_id = user_data['username']
    conn = get_connection(user_id)

    try:
        # TODO: Bring over logic from cmd.py
        # Send a register request to the C server via the client package
        send_register_request(
            conn, user_data['username'], user_data['password'])
        response = receive_register_response(conn)
        return jsonify({"status": "success", "data": response})
    except (socket.error, Exception) as e:
        handle_disconnect(user_id)
        return jsonify({"status": "failure", "error": str(e)}), 500


@app.route('/login', methods=['POST'])
@is_connected
def login():
    user_data = request.json
    user_id = user_data['username']
    conn = get_connection(user_id)

    try:
        # TODO: Bring over logic from cmd.py
        # Send login request via the client package
        send_login_request(conn, user_data['username'], user_data['password'])
        response = receive_login_response(conn)

        if response['status'] == 'success':
            access_token = create_access_token(identity=user_id)
            return jsonify(access_token=access_token), 200

        return jsonify({"status": "failure", "data": response}), 401
    except (socket.error, Exception) as e:
        handle_disconnect(user_id)
        return jsonify({"status": "failure", "error": str(e)}), 500


if __name__ == '__main__':
    app.run(debug=True)

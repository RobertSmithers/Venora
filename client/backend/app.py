"""
Flask REST API for wrapping a client library and providing a frontend interface.

This file contains the implementation of a Flask REST API that serves as a wrapper around a client library. The purpose of this API is to provide a convenient interface for making calls to the client library from a frontend application. By using this API, the frontend application can easily interact with the client library without having to directly call its functions.

The API is built using Flask, a popular web framework for Python. It exposes a set of endpoints that correspond to different operations that can be performed using the client library. These endpoints accept HTTP requests and forward them to the appropriate functions in the client library.

The API can be used to perform various tasks such as retrieving data, creating new resources, updating existing resources, and deleting resources. It follows the principles of REST (Representational State Transfer) architecture, which allows for a stateless and scalable design.

To use this API, you can make HTTP requests to the specified endpoints using a tool like cURL or a web browser. The API will handle the requests and return the appropriate responses based on the actions performed by the client library.

Note: This docstring only provides a high-level overview of the file and its purpose. For detailed documentation of the API endpoints and their functionalities, please refer to the code comments and the API documentation.
"""

from flask import Flask, jsonify, request
from client.networking.comms import connect_to_server
from client.logic.actions import (
    send_register_request, receive_register_response,
    send_login_request, receive_login_response
)

app = Flask(__name__)


@app.route('/register', methods=['POST'])
def register():
    user_data = request.json
    conn = connect_to_server()  # Connect to the C server

    # Send a register request to the C server via the client package
    send_register_request(conn, user_data['username'], user_data['password'])
    response = receive_register_response(conn)

    return jsonify({"status": "success", "data": response})


@app.route('/login', methods=['POST'])
def login():
    user_data = request.json
    conn = connect_to_server()

    # Send login request via the client package
    send_login_request(conn, user_data['username'], user_data['password'])
    response = receive_login_response(conn)

    return jsonify({"status": "success", "data": response})


if __name__ == '__main__':
    app.run(debug=True)

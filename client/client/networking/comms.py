"""
Contains all networking functionality for the program
"""

import socket
import logging
import ssl
from typing import Optional

from client.config.settings import CHUNK_SIZE

logger = logging.getLogger(__name__)


def connect_to_server(server_ip: str, server_port: int) -> Optional[socket.socket]:
    """
    Connect to Venora server with provided IP address and port.

    Args:
        server_ip (str): The IP address of the server.
        server_port (int): The port number to connect to on the server.

    Returns:
        socket.socket or None:
            - If the connection is successful, returns a connected socket object.
            - If there is an error during connection, returns None.
    """
    try:
        c_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        context = ssl.create_default_context(ssl.Purpose.SERVER_AUTH)

        # Disable older versions of SSL/TLS
        context.options |= ssl.OP_NO_TLSv1 | ssl.OP_NO_TLSv1_1 | ssl.OP_NO_SSLv2 | ssl.OP_NO_SSLv3
        context.minimum_version = ssl.TLSVersion.TLSv1_2

        # Add server's self-signed cert
        context.load_verify_locations('/client/SSL/server/server.crt')

        ssl_sock = context.wrap_socket(
            c_sock, server_hostname='VenoraServer')
        # c_sock.settimeout(5)
        print("Doing a connection")
        ssl_sock.connect((server_ip, server_port))
        logging.info("Connected to Venora Server (%s:%d)",
                     server_ip, server_port)

        return ssl_sock
    except (socket.error, TypeError) as e:
        print("hah!", e)
        logger.debug("Socket error while connecting to the server: %s", e)
        return None
    except ssl.SSLError as e:
        print("hah!", e)
        logger.debug("SSL error while connecting to the server: %s", e)
        return None
    except Exception as e:
        print("hah!", e)
        logger.debug("Other error while connecting to the server: %s", e)
        return None


def recv_from_srv(ssl_sock: ssl.SSLSocket, num_bytes: int = CHUNK_SIZE, verbose: bool = False) -> bytes:
    """
    Receive a message from Venora server.

    Args:
        sock (socket.socket): The connected socket to receive data from.
        verbose (bool, optional): If True, print verbose information. Default is False.

    Returns:
        bytes: The raw response received in bytes.
    """
    try:
        data = ssl_sock.recv(num_bytes)

        if not data:
            raise socket.error("Connection closed by the server.")

        if verbose:
            # Output to logs and console
            logger.debug("Received: %s", data)
        return data

    except ssl.SSLError as e:
        # Handle receiving SSL errors
        logger.debug("Error receiving SSL data from the server: %s", e)
        return
    except socket.error as e:
        # Handle receiving socket errors
        logger.debug("Error receiving data from the server: %s", e)
        return b''


def send_to_srv(ssl_sock: ssl.SSLSocket, data: bytes, verbose: bool = False) -> None:
    """
    Send a message to the server.

    Args:
        sock (socket.socket): The connected socket to send data to.
        message (str): The message to send to the server.
        verbose (bool, optional): If True, print verbose information. Default is False.

    Returns:
        None
    """
    total_sent = 0
    n = len(data)

    try:
        while total_sent < n:
            bytes_sent = ssl_sock.send(data[total_sent:total_sent+CHUNK_SIZE])

            if bytes_sent == 0:
                raise ConnectionError("Connection closed during send")

            total_sent += bytes_sent

        if verbose:
            # Output to logs and console
            logger.info("Sent: %d bytes of data\n%s", n, data)
    except ssl.SSLError as e:
        logger.error("Error sending SSL data to the server: %s", e)
    except socket.error as e:
        logger.error("Error sending data to the server: %s", e)

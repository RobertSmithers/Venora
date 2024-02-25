"""
Contains all networking functionality for the program
"""

import socket
import logging
from typing import Optional

from schema import RequestType
from packer import pack_req

logger = logging.getLogger(__name__)


class Request:
    def __init__(self, type: RequestType, data=None) -> None:
        self.type = type
        self.data = data

    def pack(self) -> Optional[bytes]:
        """
        Packs the Request object into bytes to send to Venora server
        """
        # Note: packer assumes that data is correctly formatted for the given RequestType
        return pack_req(self.type, self.data)


def connect_to_server(server_ip: str, server_port: int) -> socket.socket | None:
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
        c_sock.connect((server_ip, server_port))

        return c_sock
    except (socket.error, TypeError) as e:
        logger.warn(f"Error connecting to the server: {e}")
        return None


# I think I want to specify type enumerations here for responses
def recv_from_srv(sock: socket.socket, verbose: bool = False) -> str:
    """
    Receive a message from Venora server.

    Args:
        sock (socket.socket): The connected socket to receive data from.
        verbose (bool, optional): If True, print verbose information. Default is False.

    Returns:
        str: The received message as a string.
    """
    try:
        data = sock.recv(1024)

        if not data:
            raise socket.error("Connection closed by the server.")

        message = data.decode('utf-8')

        if verbose:
            logger.info(f"Received message from server: {message}")

        return message

    except socket.error as e:
        # Handle receiving errors
        logger.error(f"Error receiving data from the server: {e}")
        return ""


# See above comment (enumerations for requests as well)
def send_to_srv(sock: socket.socket, message: str, verbose: bool = False) -> None:
    """
    Send a message to the server.

    Args:
        sock (socket.socket): The connected socket to send data to.
        message (str): The message to send to the server.
        verbose (bool, optional): If True, print verbose information. Default is False.

    Returns:
        None
    """
    try:
        # Send data to the server
        sock.sendall(message.encode('utf-8'))

        if verbose:
            logger.info(f"Sent message to server: {message}")

    except socket.error as e:
        logger.error(f"Error sending data to the server: {e}")

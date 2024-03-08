"""
Contains all networking functionality for the program
"""

import socket
import logging
from typing import Optional

from client.config.settings import CHUNK_SIZE
from client.networking.schema import RequestType
from client.networking.packer import pack_req

logger = logging.getLogger(__name__)


class Request:
    """A client's request object to build and send to server"""

    def __init__(self, req_type: RequestType, data=None) -> None:
        self.req_type = req_type
        self.data = data

    def pack(self) -> Optional[bytes]:
        """
        Packs the Request object into bytes to send to Venora server
        """
        # Note: packer assumes that data is correctly formatted for the given
        # RequestType
        return pack_req(self.req_type, self.data)


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
        c_sock.settimeout(5)
        c_sock.connect((server_ip, server_port))

        return c_sock
    except (socket.error, TypeError) as e:
        logger.debug("Error connecting to the server: %s", e)
        return None


# I think I want to specify type enumerations here for responses
def recv_from_srv(sock: socket.socket, verbose: bool = False) -> bytes:
    """
    Receive a message from Venora server.

    Args:
        sock (socket.socket): The connected socket to receive data from.
        verbose (bool, optional): If True, print verbose information. Default is False.

    Returns:
        str: The received message as a string.
    """
    print("verbose is", verbose)
    try:
        data = sock.recv(1024)

        if not data:
            raise socket.error("Connection closed by the server.")

        message = data.decode('utf-8')

        if verbose:
            # Output to logs and console
            print("outputting")
            logger.info("Received: %s", message)
        return message

    except socket.error as e:
        # Handle receiving errors
        logger.debug("Error receiving data from the server: %s", e)
        return ""


# See above comment (enumerations for requests as well)
def send_to_srv(sock: socket.socket, data: bytes, verbose: bool = False) -> None:
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
            bytes_sent = sock.send(data[total_sent:total_sent+CHUNK_SIZE])

            if bytes_sent == 0:
                raise ConnectionError("Connection closed during send")

            total_sent += bytes_sent

        if verbose:
            # Output to logs and console
            print("outputting")
            logger.info("Sent: %d bytes of data\n%s", n, data)

    except socket.error as e:
        logger.error("Error sending data to the server: %s", e)

"""Handles the intermediate business logic between client application and networking"""

import socket

from client.networking.packer import (
    pack_type_register,
    pack_type_authenticate,
    pack_type_get_strike_packs
)
from client.networking.comms import send_to_srv


def register(sock: socket.socket, username: str, verbose: bool = False) -> None:
    """
    Creates and sends a register request to the server
    """
    data = {"username": username}
    req = pack_type_register(data)
    send_to_srv(sock, req, verbose=verbose)


def login(sock: socket.socket, username: str, token: str, verbose: bool = False) -> None:
    """
    Creates and sends a login request to the server
    """
    data = {"username": username, "token": token}
    req = pack_type_authenticate(data)
    send_to_srv(sock, req, verbose=verbose)


def get_strike_packs(sock: socket.socket, verbose: bool = False) -> None:
    """
    Creates and sends a strike pack listing to the server
    Expects to receive a listing of capabilities based on permissions of account
    """
    data = {}
    req = pack_type_get_strike_packs(sock, req, verbose=verbose)
    send_to_srv(sock, req, verbose=verbose)

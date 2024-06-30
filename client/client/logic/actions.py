"""Handles the intermediate business logic between client application and networking"""

import socket
import logging
from typing import List

from client.networking.comms import send_to_srv, recv_from_srv
from client.networking.schema import RequestType
from client.networking.packer import pack_req, unpack_response_type, unpack_type

logger = logging.getLogger(__name__)


def send_register_request(sock: socket.socket, username: str, verbose: bool = False) -> None:
    """
    Creates and sends a register request to the server
    """
    data = {"username": username}
    req = pack_req(RequestType.REGISTER, data)
    if req:
        send_to_srv(sock, req, verbose=verbose)
    else:
        logging.error("Failed to send register request")


def receive_register_response(sock: socket.socket, verbose: bool = False) -> List[str]:
    """
    Receives a register response and parses data into a list
    """
    response = recv_from_srv(sock, 2, verbose=verbose)
    if not response:
        logger.warn("Received empty response from server")
        return []
    resp_type = unpack_response_type(response)
    return unpack_type(sock, resp_type, RequestType.REGISTER)


def send_login_request(sock: socket.socket, username: str, token: str, verbose: bool = False) -> None:
    """
    Creates and sends a login request to the server
    """
    data = {"username": username, "token": token}
    req = pack_req(RequestType.AUTHENTICATE, data)
    if req:
        send_to_srv(sock, req, verbose=verbose)
    else:
        logging.error("Failed to send login request")


def receive_login_response(sock: socket.socket, verbose: bool = False) -> List[str]:
    """
    Receives a login response and parses data into a list
    """
    response = recv_from_srv(sock, 2, verbose=verbose)
    if not response:
        logger.warn("Received empty response from server")
        return []
    resp_type = unpack_response_type(response)
    return unpack_type(sock, resp_type, RequestType.AUTHENTICATE)


def send_get_strike_packs_request(sock: socket.socket, verbose: bool = False) -> None:
    """
    Creates and sends a strike pack listing to the server
    Expects to receive a listing of capabilities based on permissions of account
    """
    data = {}
    req = pack_req(RequestType.GET_STRIKE_PACKS, data)
    if req:
        send_to_srv(sock, req, verbose=verbose)
    else:
        logging.error("Failed to send get strike packs request")


def receive_get_strike_packs_response(sock: socket.socket, verbose: bool = False) -> List[str]:
    """
    Receives a strike packs response and parses data into a list
    """
    response = recv_from_srv(sock, 2, verbose=verbose)
    if not response:
        logger.warn("Received empty response from server")
        return []
    resp_type = unpack_response_type(response)
    return unpack_type(sock, resp_type, RequestType.GET_STRIKE_PACKS)

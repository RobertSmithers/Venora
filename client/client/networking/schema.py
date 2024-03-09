"""
Contains the structures used for Venora networking
"""

from enum import Enum

# TODO: maybe use protobuff instead for standardization?


class RequestType(Enum):
    """
    Enumeration class for requests from client to server.
    """
    REGISTER = 1
    AUTHENTICATE = 2
    GET_STRIKE_PACKS = 3


class ResponseType(Enum):
    """
    Enumeration class for requests from server to client.
    """
    SUCCESS = 1
    SUCCESS_DATA = 2
    FAILURE = 3
    INVALID_REQUEST = 4
    SERVER_ERROR = 5

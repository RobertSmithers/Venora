"""
Handles packing and unpacking for client requests and server responses
"""
import struct
import logging
import sys
import socket
from typing import Any, Dict, Optional, List

from client.config.settings import USERNAME_MAX_CHARS
from client.networking.schema import RequestType, ResponseType
from client.networking.comms import recv_from_srv

# Note: I was really close to calling this packman.py

logger = logging.getLogger(__name__)

############################################
#################  REQUESTS  ###############
############################################


def pack_type_register(data: dict) -> Optional[bytes]:
    """
    packs data for the register request

    Args:
        data (dict): Dictionary with 'username'

    Returns:
        None if error, otherwise
        bytes of packed object in the form:
            unsigned short  - RequestType
            unsigned short  - Number of username bytes
            char[]          - username
    """
    username = data.get("username")
    # TODO: Refactor these into a validate_fields func, validate_username_sz
    if not username:
        logger.error(
            "Failed to pack register request - no username provided")
        return None
    user_len = len(username)
    if user_len > USERNAME_MAX_CHARS:
        logger.error(
            "Failed to pack register request - username too long (> %d chars)",
            USERNAME_MAX_CHARS
        )
        return None
    return struct.pack(f"!HH{user_len}s", RequestType.REGISTER.value, user_len, username.encode())


def pack_type_authenticate(data: dict) -> Optional[bytes]:
    """
    packs data for the authenticate request

    Args:
        data (dict): Dictionary with 'username' and 'token'

    Returns:
        None if error, otherwise
        bytes of packed object in the form:
            unsigned short  - RequestType
            unsigned short  - Number of username bytes
            char[]          - username
            unsigned short  - Number of token bytes
            char[]          - token
    """
    username = data.get("username")
    if not username:
        logger.error(
            "Failed to pack an authenticate request - no username provided")
        return None
    user_len = len(username)
    if user_len > USERNAME_MAX_CHARS:
        logger.error(
            "Failed to pack register request - username too long (> %d chars)",
            USERNAME_MAX_CHARS
        )
        return None
    token = data.get("token")
    if not token:
        logger.error(
            "Failed to pack an authenticate request - no token provided")
        return None
    token_len = len(token)
    return struct.pack(f"!HH{user_len}sH{token_len}s",
                       RequestType.AUTHENTICATE.value,
                       user_len,
                       username.encode(),
                       token_len,
                       token.encode()
                       )


def pack_type_get_strike_packs(data: dict) -> bytes:
    """
    packs data for the get strike packs request

    Args:
        data (dict): Empty dictionary

    Returns:
        None if error, otherwise
        bytes of packed object in the form:
            unsigned short  - RequestType
    """
    return struct.pack(f"!H", RequestType.GET_STRIKE_PACKS.value)


def pack_req(req_type: RequestType, data: Dict[str, Any]) -> Optional[bytes]:
    """
    Packs a request message based on the specified request type and data.

    Parameters:
    - req_type (RequestType): The type of the request.
    - data (Dict[str, Any]): The dictionary containing data associated with the request.
      The keys and values depend on the specific request type.

    Returns:
    - Optional[bytes]: The packed binary representation of the request,
      or None if the RequestType is not found/is not implemented.

    Note:
    - This function delegates the packing process to corresponding 'pack_type'
      functions based on the request type. 'pack_type' functions
      must be implemented for each supported request type.
    """
    pack_handlers = {
        RequestType.REGISTER: pack_type_register,
        RequestType.AUTHENTICATE: pack_type_authenticate,
        RequestType.GET_STRIKE_PACKS: pack_type_get_strike_packs,
    }

    handler = pack_handlers.get(req_type)
    if handler is None:
        logging.error(
            "Invalid or unsupported RequestType with value %d", req_type)
        return None

    return handler(data)

############################################
#################  RESPONSE  ###############
############################################


def unpack_response_type(response: bytes) -> ResponseType:
    """retrieves the ResponseType from the response packet
    unpack_type functions must be implemented for each supported response type
    """
    if not response:
        print("Error: response is ", repr(response))
        return None
    return ResponseType(struct.unpack('!H', response[:2])[0])


def unpack_type(sock: socket.socket, resp_type: ResponseType, from_req_type: RequestType) -> List[str]:
    """retrieves data based on the response type"""
    handler = getattr(sys.modules[__name__],
                      f"unpack_type_{resp_type.name.lower()}")

    if not handler:
        raise Exception(
            "Unexpected response from server: ", resp_type)
    return [resp_type] + handler(sock, from_req_type)


def _get_data_blocks(sock: socket.socket, num_blocks: int) -> List[str]:
    ret = []
    for _ in range(num_blocks):
        # TODO: Account for if num_data extends beyond this response/packet sz
        # Get the size of the data
        response = recv_from_srv(sock, 2)
        data_len = int(struct.unpack('!H', response)[0])

        # If data_len is 0, then there is no more data... end here
        if not data_len:
            return ret

        # Read data_len bytes of data
        response = recv_from_srv(sock, int(data_len), verbose=True)
        data = struct.unpack(f'!{data_len}s', response)[0]

        # TODO: Custom class to represent strike pack/other data
        # Probably just some serializable data format will do
        ret.append(data.decode())
    return ret


def unpack_type_success(sock: socket.socket, req_type: RequestType) -> List[str]:
    """unpack a success response (By default there should be no data)"""
    return []


def unpack_type_success_data(sock: socket.socket, req_type: RequestType) -> List[str]:
    """
    unpack a success data response
    Depending on what was requested, the format may slightly differ
    """
    out_data = []
    if req_type == RequestType.GET_STRIKE_PACKS:
        response = recv_from_srv(sock, 2)
        num_blocks = int(struct.unpack('!H', response)[0])
        print(f"There are {num_blocks} data fields")
        out_data = _get_data_blocks(sock, num_blocks)
    elif req_type == RequestType.REGISTER:
        out_data = _get_data_blocks(sock, 1)
    else:
        logger.warning(
            "Unexpected success_data response for Request Type of %s", req_type.name)
    return out_data


def unpack_type_failure(sock: socket.socket, req_type: RequestType) -> List[str]:
    """unpack a failure response and return a list of size 1 of the failure message"""
    return _get_data_blocks(sock, 1)


def unpack_type_invalid_request(sock: socket.socket, req_type: RequestType) -> List[str]:
    """unpack an invalid request response (By default there should be no data)"""
    return []


def unpack_type_server_error(sock: socket.socket, req_type: RequestType) -> List[str]:
    """unpack a server error response (By default there should only be an error code short)"""
    response = recv_from_srv(sock, 2)
    err_code = int(struct.unpack('!H', response)[0])
    return [err_code]

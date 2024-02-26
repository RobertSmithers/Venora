"""
Handles packing and unpacking for client requests and server responses 
"""
import struct
import logging
from typing import Any, Dict, Optional

from client.config import USERNAME_MAX_CHARS
from client.networking.schema import RequestType, ResponseType

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
    return struct.pack(f"!HH{user_len}sH{token_len}s", RequestType.AUTHENTICATE.value, user_len, username.encode(), token_len, token.encode())


def pack_req(type: RequestType, data: Dict[str, Any]) -> Optional[bytes]:
    """
    Packs a request message based on the specified request type and data.

    Parameters:
    - type (RequestType): The type of the request.
    - data (Dict[str, Any]): The dictionary containing data associated with the request.
      The keys and values depend on the specific request type.

    Returns:
    - Optional[bytes]: The packed binary representation of the request,
      or None if the RequestType is not found/is not implemented.

    Note:
    - This function delegates the packing process to corresponding 'pack_type'
      functions based on the request type. Ensure that the 'pack_type' functions
      are implemented for each supported request type.
    """
    pack_handlers = {
        RequestType.REGISTER: pack_type_register,
        RequestType.AUTHENTICATE: pack_type_authenticate
    }

    handler = pack_handlers.get(type)
    if handler is None:
        logging.error("Invalid or unsupported RequestType with value %d", type)
        return None

    return handler(data)

############################################
#################  RESPONSE  ###############
############################################


def unpack_response_type(response: bytes) -> ResponseType:
    """retrieves the ResponseType from the response packet"""
    return struct.unpack('!H', response[:2])[0]

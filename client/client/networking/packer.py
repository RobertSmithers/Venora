"""
Handles packing and unpacking for client requests and server responses 

Note: All packer functions assume that the data is correctly formatted (ie. all necessary fields exist)
"""
import struct
import logging
from typing import Any, Dict, Optional
from client.networking.schema import RequestType, ResponseType

# Note: I was really close to calling this packman.py

logger = logging.getLogger(__name__)

############################################
#################  REQUESTS  ###############
############################################


def pack_type_register(data: dict) -> bytes:
    """
    packs data for the register request

    Args:
        data (dict): Dictionary with 'username'

    Returns:
        bytes of packed object in the form:
            unsigned short  - RequestType
            unsigned short  - Number of username bytes
            char[]          - username
    """
    username = data["username"]
    user_len = len(username)
    return struct.pack("!HH{user_len}s", RequestType.REGISTER, user_len, username)


def pack_type_authenticate(data: dict) -> bytes:
    """
    packs data for the authenticate request

    Args:
        data (dict): Dictionary with 'username' and 'token'

    Returns:
        bytes of packed object in the form:
            unsigned short  - RequestType
            unsigned short  - Number of username bytes
            char[]          - username
            unsigned short  - Number of token bytes
            char[]          - token
    """
    username = data["username"]
    user_len = len(username)
    token = data["token"]
    token_len = len(token)
    return struct.pack(f"!HH{user_len}sH{token_len}s", RequestType.REGISTER, user_len, username, token_len, token)


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

    # # Unpack response type to determine the structure of the packed object
    # if response_type == ResponseType.:  # Adjust this based on your actual response types
    #     # Assuming the response type 1 has a string data following it
    #     data_length = struct.unpack('!H', response[2:4])[0]
    #     data = binary_response[4:4 + data_length].decode('utf-8')

    #     # Now 'data' contains the additional data for response type 1
    #     print(f"Response Type: {response_type}, Data: {data}")
    # else:
    #     print(f"Unknown Response Type: {response_type}")

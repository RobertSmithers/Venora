import logging
from typing import List

from client.networking.schema import ResponseType

logger = logging.getLogger(__name__)


def base_response_handler(resp: List, expected_resp_types: List):
    """
    Parses and logs generic response types (INVALID REQUEST and SERVER ERROR)
    All other responses should be handled by calling function
    """
    assert (len(resp))
    rtype = resp[0]
    if (rtype == ResponseType.INVALID_REQUEST):
        logger.error("SERVER - Invalid Request")
    elif (rtype == ResponseType.SERVER_ERROR):
        # All server error responses should have an error code
        logger.error("SERVER - Internal Server Error #%d", resp[1])
    elif (rtype not in expected_resp_types):
        logger.error("Unexpected response type from server: %d", rtype)

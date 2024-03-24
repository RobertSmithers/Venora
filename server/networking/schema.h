#pragma once

enum RequestType
{
    REQUEST_REGISTER = 1,
    REQUEST_LOGIN,
    REQUEST_GET_STRIKE_PACKS,
};

enum ResponseType
{
    RESPONSE_SUCCESS = 1,
    RESPONSE_SUCCESS_DATA,
    RESPONSE_FAILURE,
    RESPONSE_INVALID_REQUEST,
    RESPONSE_SERVER_ERROR
};

// Number of bytes used to store packet data
#define REQ_RESP_TYPE_SIZE 2
#define REQ_DATA_VARLEN_SIZE 2
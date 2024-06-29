#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>

#include "schema.h"

#pragma once
typedef struct DataBlock_t
{
    void *data;
    size_t size;
} DataBlock;

/*
Clear a socket buffer from any remaining data (and reset back to blocking mode aftewards)
*/
void flush_socket(int sock);

/*
Send a response to the client with the Response Type of Success
*/
bool send_response_success(int sock);

/*
Send a response to the client with the Response Type of Success Data
Data field follows with the pattern of data size | data ...
*/
bool send_response_success_data(int sock, uint16_t num_data_blocks, DataBlock *blocks);
// bool send_response_success_data(int sock, void **data, uint16_t num_data_blocks, uint16_t data_block_size);

/*
Send a response to the client with the Response Type of Failure
Optionally include a message that will be displayed to the user
*/
bool send_response_failure(int sock, char *msg);

/*
Send a response to the client with the Response Type of Invalid Request
*/
bool send_response_invalid(int sock);

/*
Send a response to the client with the Response Type of Server Error
*/
bool send_response_server_error(int sock, uint8_t error_code);
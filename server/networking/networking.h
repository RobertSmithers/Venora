#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "schema.h"

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
bool send_response_success_data(int sock, void *data, uint16_t data_len);

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
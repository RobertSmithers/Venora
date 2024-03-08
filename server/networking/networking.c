#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <networking.h>

bool send_response_failure(int sock, char *msg)
{
    // Get data structs
    uint16_t msg_len = strlen(msg);
    uint8_t *buffer = malloc(REQ_RESP_TYPE_SIZE + REQ_DATA_VARLEN_SIZE + msg_len);
    if (buffer == NULL)
    {
        perror("Failed to malloc send buffer");
    }

    // Construct packet
    uint16_t offset = 0;
    buffer[0] = RESPONSE_FAILURE;
    offset += REQ_RESP_TYPE_SIZE;
    buffer[offset] = msg_len;
    offset += REQ_DATA_VARLEN_SIZE;
    memcpy(&buffer[offset], msg, msg_len);
    offset += msg_len;

    // Send the data
    ssize_t sent = send(sock, buffer, offset, 0);
    if (sent == -1)
    {
        perror("Error sending data");
        free(buffer);
        return false;
    }

    free(buffer);
    return true;
}

bool send_response_success(int sock, void *data, uint16_t data_len)
{
    uint8_t *buffer = malloc(REQ_RESP_TYPE_SIZE + REQ_DATA_VARLEN_SIZE + data_len);
    if (buffer == NULL)
    {
        perror("Failed to malloc send buffer");
    }

    // Construct packet
    uint16_t offset = 0;
    buffer[0] = RESPONSE_SUCCESS;
    offset += REQ_RESP_TYPE_SIZE;
    buffer[offset] = data_len;
    offset += REQ_DATA_VARLEN_SIZE;
    memcpy(&buffer[offset], data, data_len);
    offset += data_len;

    // Send the data
    ssize_t sent = send(sock, buffer, offset, 0);
    if (sent == -1)
    {
        perror("Error sending data");
        free(buffer);
        return false;
    }

    free(buffer);
    return true;
}

bool send_response_invalid(int sock)
{
    uint16_t response = htons(RESPONSE_INVALID_REQUEST);
    ssize_t sent = send(sock, &response, sizeof(response), 0);
    if (sent == -1)
    {
        perror("Error sending data");
        return false;
    }

    return true;
}
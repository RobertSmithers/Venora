#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <networking.h>

void print_buffer(const uint8_t *buffer, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        printf("%02x ", buffer[i]);
    }
    printf("\n");
}

int send_to_client(int sock, const void *buffer, size_t sz, int flags)
{
    int bytes_sent = send(sock, buffer, sz, flags);
    printf("Sent %d bytes of data:\n", bytes_sent);
    print_buffer((uint8_t *)buffer, sz);
    return bytes_sent;
}

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
    // 1) Add response type
    uint16_t response_type = htons(RESPONSE_FAILURE);
    memcpy(buffer, &response_type, sizeof(response_type));
    uint16_t offset = REQ_RESP_TYPE_SIZE;

    // 2) Add msg len
    uint16_t msg_len_net = htons(msg_len);
    memcpy(&buffer[offset], &msg_len_net, sizeof(msg_len_net));
    offset += REQ_DATA_VARLEN_SIZE;

    // 3) Add msg
    memcpy(&buffer[offset], msg, msg_len);
    offset += msg_len;

    // Send the data
    ssize_t sent = send_to_client(sock, buffer, offset, 0);
    if (sent == -1)
    {
        perror("Error sending data");
        free(buffer);
        return false;
    }

    free(buffer);
    return true;
}

bool send_response_success_data(int sock, void *data, uint16_t data_len)
{
    // TODO: Add function for success_data with multiple data blocks
    uint8_t *buffer = (uint8_t *)malloc(REQ_RESP_TYPE_SIZE + REQ_DATA_VARLEN_SIZE + data_len);
    if (buffer == NULL)
    {
        perror("Failed to malloc send buffer");
    }

    // Construct packet
    // 1) Add response type
    uint16_t response_type = htons(RESPONSE_SUCCESS_DATA);
    memcpy(buffer, &response_type, sizeof(response_type));

    // TODO: Define a shared type/size for the packet blocks (ex: REQ_DATA_VARLEN_SIZE = uint16_t type of thing)
    // 2) Add data len
    uint16_t offset = REQ_RESP_TYPE_SIZE;
    uint16_t data_len_net = htons(data_len);
    memcpy(&buffer[offset], &data_len_net, sizeof(data_len_net));
    offset += REQ_DATA_VARLEN_SIZE;

    // 3) Add data
    memcpy(&buffer[offset], data, data_len);
    offset += data_len;

    // Send the data
    ssize_t sent = send_to_client(sock, buffer, offset, 0);
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
    ssize_t sent = send_to_client(sock, &response, sizeof(response), 0);
    if (sent == -1)
    {
        perror("Error sending data");
        return false;
    }

    return true;
}
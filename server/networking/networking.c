#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdint.h>

#include <networking.h>

void print_buffer(const uint8_t *buffer, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        printf("%02x ", buffer[i]);
    }
    printf("\n");
}

void flush_socket(int sock)
{
    // Set client socket to nonblocking
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1)
    {
        perror("Failed to flush socket");
        return;
    }

    flags |= O_NONBLOCK;
    fcntl(sock, F_SETFL, flags);

    // Read until EOF (socket equivalent)
    ssize_t bytes_read;
    uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t) * 1024);
    do
    {
        bytes_read = recv(sock, buffer, 1024, 0);
    } while (bytes_read > 0);
    free(buffer);

    // Set client socket back to blocking
    flags &= ~O_NONBLOCK;
    fcntl(sock, F_SETFL, flags);
}

int send_to_client(int sock, const void *buffer, size_t sz, int flags)
{
    int bytes_sent = send(sock, buffer, sz, flags);
    printf("Sent %d bytes of data:\n", bytes_sent);
    print_buffer((uint8_t *)buffer, sz);
    return bytes_sent;
}

bool send_response_success(int sock)
{
    uint8_t buffer[REQ_RESP_TYPE_SIZE];

    // Construct packet
    // 1) Add response type
    uint16_t response_type = htons(RESPONSE_SUCCESS);
    memcpy(buffer, &response_type, sizeof(response_type));

    // Send the data
    ssize_t sent = send_to_client(sock, buffer, REQ_RESP_TYPE_SIZE, 0);
    if (sent == -1)
    {
        perror("Error sending data");
        return false;
    }

    return true;
}

bool send_response_success_data(int sock, uint16_t num_data_blocks, DataBlock **blocks)
//**data, uint16_t num_data_blocks, uint16_t data_block_size)
// Expects the varargs to be of type DataBlock*
{

    size_t total_size = 0;
    for (int i = 0; i < num_data_blocks; i++)
    {
        total_size += blocks[i]->size;
    }

    uint8_t *buffer = (uint8_t *)malloc(REQ_RESP_TYPE_SIZE + REQ_DATA_VARLEN_SIZE + (total_size));
    if (buffer == NULL)
    {
        perror("Failed to malloc send buffer");
        return NULL;
    }

    // Construct packet
    // 1) Add response type
    uint16_t response_type = htons(RESPONSE_SUCCESS_DATA);
    memcpy(buffer, &response_type, sizeof(response_type));
    uint16_t offset = REQ_RESP_TYPE_SIZE;

    for (int i = 0; i < num_data_blocks; i++)
    {
        // TODO: Define a shared type/size for the packet blocks (ex: REQ_DATA_VARLEN_SIZE = uint16_t type of thing)

        // 2) Add data len
        uint16_t data_len_net = htons(blocks[i]->size);
        memcpy(&buffer[offset], &data_len_net, sizeof(data_len_net));
        offset += REQ_DATA_VARLEN_SIZE; // The uint16_t above

        // 3) Add data
        memcpy(&buffer[offset], (void *)blocks[i]->data, blocks[i]->size);
        offset += blocks[i]->size;

    } // 4) Repeat 2. and 3. for every data block

    // If there is no data to send (such as no strike packs available)
    // This function may still be called over send_response_success
    // In this case, we send a data block size of 0 with no data after it
    if (num_data_blocks == 0)
    {
        uint8_t *new_buffer = (uint8_t *)realloc(buffer,
                                                 REQ_RESP_TYPE_SIZE +
                                                     REQ_DATA_VARLEN_SIZE +
                                                     sizeof(uint16_t)); // Realloc to add in the space for our data block's length identifier of 0
        if (NULL == new_buffer)
        {
            perror("Memory reallocation failed");
            free(buffer);
            return false;
        }
        buffer = new_buffer;
        uint16_t data_len_net = htons(0);
        memcpy(&buffer[offset], &data_len_net, sizeof(data_len_net));
        offset += sizeof(REQ_DATA_VARLEN_SIZE);
    }

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

bool send_response_failure(int sock, char *msg)
{
    // TODO: msg should be a NULLable value (optional)
    // Implementation: malloc w/o msg_len, then when need to add it, check if msg == NULL
    // and if not, calloc() the buffer and then add msg_len and msg

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

bool send_response_server_error(int sock, uint8_t error_code)
{
    uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t) * (REQ_RESP_TYPE_SIZE + 1));

    // Construct packet
    // 1) Add response type
    uint16_t response_type = htons(RESPONSE_SERVER_ERROR);
    memcpy(buffer, &response_type, sizeof(response_type));
    uint16_t offset = REQ_RESP_TYPE_SIZE;

    // 2) Add error code
    memcpy(&buffer[offset], &error_code, sizeof(error_code));
    offset += sizeof(error_code);

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
#include <stdlib.h>
#include <string.h>

#include "networking/web-utils.h"
#include "networking/certificates.h"

#ifndef MEMORY_STRUCT_H
#define MEMORY_STRUCT_H
typedef struct
{
    char *memory;
    size_t size;
} MemoryStruct;
#endif // MEMORY_STRUCT_H

/* CURL callback function to write our response */
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    MemoryStruct *mem = (MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL)
    {
        // Out of memory
        fprintf(stderr, "Not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = '\0';

    return realsize;
}

/* The API-endpoint url to query */
const char *get_base_url()
{
    return "https://db-accessor";
}

/* Function to URL-encode a string */
char *url_encode(const char *str)
{
    return curl_easy_escape(NULL, str, 0);
}

char *json_object_to_url_encoded_str(struct json_object *json_obj)
{
    if (json_obj == NULL)
    {
        return NULL;
    }

    struct json_object_iterator it = json_object_iter_begin(json_obj);
    struct json_object_iterator itEnd = json_object_iter_end(json_obj);

    // Buffer for the string
    size_t buffer_size = 256;
    char *encoded_str = (char *)malloc(buffer_size);
    if (encoded_str == NULL)
    {
        return NULL;
    }
    encoded_str[0] = '\0';

    while (!json_object_iter_equal(&it, &itEnd))
    {
        const char *key = json_object_iter_peek_name(&it);
        struct json_object *val_obj = json_object_iter_peek_value(&it);
        const char *val = json_object_get_string(val_obj);

        char *encoded_key = url_encode(key);
        char *encoded_val = url_encode(val);

        // Get encoded string length and realloc if needed
        size_t new_length = strlen(encoded_str) + strlen(encoded_key) + strlen(encoded_val) + 2; // +2 for '=' and '&'
        if (new_length >= buffer_size)
        {
            buffer_size = new_length + 1;
            char *new_encoded_str = (char *)realloc(encoded_str, buffer_size);
            if (new_encoded_str == NULL)
            {
                free(encoded_str);
                curl_free(encoded_key);
                curl_free(encoded_val);
                return NULL;
            }
            encoded_str = new_encoded_str;
        }

        // Append the encoded key-value pair to string
        if (strlen(encoded_str) > 0)
        {
            strcat(encoded_str, "&");
        }
        strcat(encoded_str, encoded_key);
        strcat(encoded_str, "=");
        strcat(encoded_str, encoded_val);

        curl_free(encoded_key);
        curl_free(encoded_val);

        // Move to the next key-value pair
        json_object_iter_next(&it);
    }

    return encoded_str;
}

struct json_object *http_get(const char *endpoint, json_object *json_data)
{
    CURL *curl;

    // Create our memory block to eventually store json
    MemoryStruct block;
    block.memory = malloc(1); // Will realloc
    block.size = 0;

    curl = curl_easy_init();
    if (curl)
    {
        // SSL required for all comms (CUZ WE LIKE THAT!)
        init_ssl_to_db_api(curl);

        char url[256];
        sprintf(url, "%s%s", get_base_url(), endpoint);

        // Convert JSON data to string
        const char *json_str = "";
        if (NULL != json_data)
        {
            json_str = json_object_to_url_encoded_str(json_data);
        }

        // Set the URL with query parameters
        char *full_url = malloc(strlen(url) + strlen(json_str) + 2);
        sprintf(full_url, "%s?%s", url, json_str);
        curl_easy_setopt(curl, CURLOPT_URL, full_url);
        printf("http_get -- %s\n", full_url);

        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        // curl_easy_setopt(curl, CURLOPT_GETFIELDS, json_str);

        // Response data will get written to block, write_callback will parse it as json
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&block);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            fprintf(stderr, "(HTTP GET) curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            free(block.memory);
            return NULL;
        }

        struct json_object *json_obj = json_tokener_parse(block.memory);

        free(block.memory);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        free(full_url);

        return json_obj;
    }

    curl_global_cleanup();
    return NULL;
}

struct json_object *http_post(const char *endpoint, json_object *json_data)
{
    CURL *curl;

    // Create our memory block to eventually store json
    MemoryStruct block;
    block.memory = malloc(1); // Will realloc
    block.size = 0;

    curl = curl_easy_init();
    if (curl)
    {
        // SSL required for all comms (cuz we like that)
        init_ssl_to_db_api(curl);

        char url[256];
        sprintf(url, "%s%s", get_base_url(), endpoint);

        // Convert JSON data to string
        const char *json_str = "";
        if (NULL != json_data)
        {
            json_str = json_object_to_json_string(json_data);
        }

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_str);

        // Set the content type to application/json
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Response data will get written to block, write_callback will parse it as json
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&block);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            fprintf(stderr, "(HTTP_POST) curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            free(block.memory);
            return NULL;
        }

        struct json_object *json_obj = json_tokener_parse(block.memory);

        free(block.memory);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        curl_global_cleanup();

        return json_obj;
    }

    curl_global_cleanup();
    return NULL;
}

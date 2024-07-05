#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

#include "web-utils.h"

typedef struct
{
    char *memory;
    size_t size;
} MemoryStruct;

/* Utility function to write the response data into a memory structure */
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    MemoryStruct *mem = (MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL)
    {
        printf("Not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

/* The API-endpoint url to query */
const char *get_base_url()
{
    return "http://db-accessor";
}

/* Function to make an HTTP POST request */
char *http_post(const char *endpoint, const char *json_payload)
{
    CURL *curl_handle;
    CURLcode res;
    MemoryStruct chunk;

    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();

    char url[256];
    sprintf(url, "%s%s", get_base_url(), endpoint);

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, json_payload);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    res = curl_easy_perform(curl_handle);
    if (res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        free(chunk.memory);
        chunk.memory = NULL;
    }

    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();

    return chunk.memory;
}

char *http_get(const char *endpoint)
{
    CURL *curl_handle;
    CURLcode res;
    MemoryStruct chunk;

    chunk.memory = malloc(1);
    chunk.size = 0;

    char url[256];
    sprintf(url, "%s%s", get_base_url(), endpoint);

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    res = curl_easy_perform(curl_handle);
    if (res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        free(chunk.memory);
        chunk.memory = NULL;
    }

    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();

    return chunk.memory;
}

/* Helper function to parse JSON response for strike packs */
StrikePackList *parse_strike_packs(const char *json_response)
{
    struct json_object *parsed_json;
    struct json_object *packs_array;
    size_t n_packs;
    size_t i;

    parsed_json = json_tokener_parse(json_response);
    if (json_object_object_get_ex(parsed_json, "strike_packs", &packs_array))
    {
        n_packs = json_object_array_length(packs_array);
        StrikePackList *list = malloc(sizeof(StrikePackList));
        list->strike_packs = malloc(n_packs * sizeof(StrikePack *));
        list->count = n_packs;

        for (i = 0; i < n_packs; i++)
        {
            struct json_object *pack_obj = json_object_array_get_idx(packs_array, i);
            struct json_object *name;
            struct json_object *category;
            struct json_object *description;

            json_object_object_get_ex(pack_obj, "name", &name);
            json_object_object_get_ex(pack_obj, "category", &category);
            json_object_object_get_ex(pack_obj, "description", &description);

            StrikePack *pack = malloc(sizeof(StrikePack));
            pack->name = strdup(json_object_get_string(name));
            pack->category = strdup(json_object_get_string(category));
            pack->description = strdup(json_object_get_string(description));

            list->strike_packs[i] = pack;
        }

        json_object_put(parsed_json); // Free JSON object
        return list;
    }
    else
    {
        fprintf(stderr, "Failed to parse strike packs JSON response\n");
        return NULL;
    }
}
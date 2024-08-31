#include "certificates.h"

void init_ssl(CURL *curl)
{
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_CAINFO, "/usr/local/share/ca-certificates/db-accessor-api.crt");
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    // TODO: Add client cert as well
    // curl_easy_setopt(curl, CURLOPT_SSLCERT, "SSL/client.crt");
    // curl_easy_setopt(curl, CURLOPT_SSLKEY, "SSL/client.key");
}
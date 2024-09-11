#include <openssl/ssl.h>
#include <openssl/err.h>

#include "certificates.h"

void init_ssl_to_db_api(CURL *curl)
{
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_CAINFO, "/srv/SSL/db-accessor-api/db-accessor-api.crt");
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    // TODO: Add client cert as well
    // curl_easy_setopt(curl, CURLOPT_SSLCERT, "SSL/client.crt");
    // curl_easy_setopt(curl, CURLOPT_SSLKEY, "SSL/client.key");
}

void init_ssl_to_clients()
{
    SSL_load_error_strings();
    // SSL_library_init();
    OpenSSL_add_all_algorithms();
    // SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
    // if (ctx == NULL)
    // {
    //     ERR_print_errors_fp(stderr);
    //     exit(EXIT_FAILURE);
    // }

    // if (SSL_CTX_use_certificate_file(ctx, "/usr/local/share/ca-certificates/server.crt", SSL_FILETYPE_PEM) <= 0)
    // {
    //     ERR_print_errors_fp(stderr);
    //     exit(EXIT_FAILURE);
    // }

    // if (SSL_CTX_use_PrivateKey_file(ctx, "/usr/local/share/ca-certificates/server.key", SSL_FILETYPE_PEM) <= 0)
    // {
    //     ERR_print_errors_fp(stderr);
    //     exit(EXIT_FAILURE);
    // }

    // if (!SSL_CTX_check_private_key(ctx))
    // {
    //     fprintf(stderr, "Private key does not match the certificate public key\n");
    //     exit(EXIT_FAILURE);
    // }
}

void cleanup_openssl()
{
    EVP_cleanup();
}

SSL_CTX *create_context()
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    // method = SSLv23_server_method();
    method = TLS_server_method();
    printf("Created context with TLS\n");

    ctx = SSL_CTX_new(method);
    if (!ctx)
    {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);
    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);

    return ctx;
}

void configure_context(SSL_CTX *ctx)
{
    SSL_CTX_use_certificate_file(ctx, "/srv/SSL/server.crt", SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(ctx, "/srv/SSL/server.key", SSL_FILETYPE_PEM);
}
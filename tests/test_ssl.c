#include <stdio.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>

int main(int argc, char **argv)
{
    OpenSSL_add_all_digests();

    EVP_cleanup();

    return 0;
}

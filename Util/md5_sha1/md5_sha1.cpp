#include "md5_sha1.h"

MD5_SHA1::MD5_SHA1()
{

}

std::string MD5_SHA1::get_file_md5(std::string file)
{
    MD5_CTX ctx;
    int len = 0;
    unsigned char buffer[1024] = { 0 };
    unsigned char digest[16] = { 0 };

    FILE* pFile = fopen(file.c_str(), "rb");
    if (!pFile) {
        return "";
    }

    MD5_Init(&ctx);
    while ((len = fread(buffer, 1, 1024, pFile)) > 0) {
        MD5_Update(&ctx, buffer, len);
    }

    MD5_Final(digest, &ctx);

    fclose(pFile);

    char mdString[33] = { 0 };

    for (int i = 0; i < 16; i++) {
        sprintf(&mdString[i * 2], "%02x", (unsigned int)digest[i]);
    }

    return mdString;
}

std::string MD5_SHA1::get_md5(std::string data)
{
    MD5_CTX ctx;
    int len = 0;
    unsigned char buffer[1024] = { 0 };
    unsigned char digest[16] = { 0 };

    MD5_Init(&ctx);
    MD5_Update(&ctx, data.c_str(), data.size());
    MD5_Final(digest, &ctx);

    char mdString[33] = { 0 };

    for (int i = 0; i < 16; i++) {
        sprintf(&mdString[i * 2], "%02x", (unsigned int)digest[i]);
    }

    return mdString;
}

std::string MD5_SHA1::get_sha1(string uuid)
{
    unsigned char digest[SHA_DIGEST_LENGTH];
    char string[100] = { 0 };

    strcpy(string, uuid.c_str());
    SHA1((unsigned char*)&string, strlen(string), (unsigned char*)&digest);

    char mdString[SHA_DIGEST_LENGTH * 2 + 1];
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf(&mdString[i * 2], "%02x", (unsigned int)digest[i]);
    }

    return mdString;
}

char* MD5_SHA1::base64Encode(const char* buffer, int length, bool newLine)
{
    BIO* bmem = NULL;
    BIO* b64 = NULL;
    BUF_MEM* bptr;

    b64 = BIO_new(BIO_f_base64());
    if (!newLine) {
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    }
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, buffer, length);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);
    BIO_set_close(b64, BIO_NOCLOSE);

    char* buff = (char*)malloc(bptr->length + 1);
    memcpy(buff, bptr->data, bptr->length);
    buff[bptr->length] = 0;
    BIO_free_all(b64);

    return buff;
}

char* MD5_SHA1::base64Decode(const char* input, int length, bool newLine)
{
    BIO* b64 = NULL;
    BIO* bmem = NULL;
    char* buffer = (char*)malloc(length);
    memset(buffer, 0, length);
    b64 = BIO_new(BIO_f_base64());
    if (!newLine) {
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    }
    bmem = BIO_new_mem_buf(input, length);
    bmem = BIO_push(b64, bmem);
    BIO_read(bmem, buffer, length);
    BIO_free_all(bmem);

    return buffer;
}

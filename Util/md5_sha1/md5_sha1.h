#ifndef MD5_SHA1_H
#define MD5_SHA1_H

#include <iostream>
using namespace std;

#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/evp.h>  
#include <openssl/bio.h>  
#include <openssl/buffer.h>    

class MD5_SHA1
{
public:
    MD5_SHA1();

    static std::string get_file_md5(std::string file);
    static std::string get_md5(std::string data);
    static std::string get_sha1(string uuid);

    static char* base64Encode(const char* buffer, int length, bool newLine);
    static char* base64Decode(const char* input, int length, bool newLine);

    //static int GetEncoderClsid(const WCHAR* format, GUID* pClsid);
};

#endif // MD5_SHA1_H

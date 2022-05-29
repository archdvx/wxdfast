/***************************************************************
 * Name:      hash.cpp
 * Author:    David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2021
 * License:   GPL2
 **************************************************************/

#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "hash.h"
#include "UtilFunctions.h"

mHash::mHash()
{
}

wxString mHash::MD5(const wxString &path)
{
    if(!wxFileExists(path))
    {
        return "";
    }
    wxBeginBusyCursor();
    std::ifstream fp(path.mb_str(), std::ios::in | std::ios::binary);
    constexpr const std::size_t buffer_size { 1 << 12 };
    char buffer[buffer_size];
    unsigned char *hash;
    unsigned int hash_len = EVP_MD_size(EVP_md5());
    EVP_MD_CTX *ctx;
    ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_md5(), NULL);
    while(fp.good())
    {
        fp.read(buffer, buffer_size);
        EVP_DigestUpdate(ctx, buffer, fp.gcount());
    }
    hash = (unsigned char *)OPENSSL_malloc(hash_len);
    EVP_DigestFinal_ex(ctx, hash, &hash_len);
    EVP_MD_CTX_free(ctx);
    fp.close();
    std::ostringstream os;
    os << std::hex << std::setfill('0');
    for(unsigned int i = 0; i < hash_len; ++i)
    {
        os << std::setw(2) << static_cast<unsigned int>(hash[i]);
    }
    wxEndBusyCursor();
    return MyUtilFunctions::FromStdString(os.str());
}

wxString mHash::SHA1(const wxString &path)
{
    if(!wxFileExists(path))
    {
        return "";
    }
    wxBeginBusyCursor();
    std::ifstream fp(path.mb_str(), std::ios::in | std::ios::binary);
    constexpr const std::size_t buffer_size { 1 << 12 };
    char buffer[buffer_size];
    unsigned char *hash;
    unsigned int hash_len = EVP_MD_size(EVP_sha1());
    EVP_MD_CTX *ctx;
    ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha1(), NULL);
    while(fp.good())
    {
        fp.read(buffer, buffer_size);
        EVP_DigestUpdate(ctx, buffer, fp.gcount());
    }
    hash = (unsigned char *)OPENSSL_malloc(hash_len);
    EVP_DigestFinal_ex(ctx, hash, &hash_len);
    EVP_MD_CTX_free(ctx);
    fp.close();
    std::ostringstream os;
    os << std::hex << std::setfill('0');
    for(unsigned int i = 0; i < hash_len; ++i)
    {
        os << std::setw(2) << static_cast<unsigned int>(hash[i]);
    }
    wxEndBusyCursor();
    return MyUtilFunctions::FromStdString(os.str());
}

wxString mHash::SHA256(const wxString &path)
{
    if(!wxFileExists(path))
    {
        return "";
    }
    wxBeginBusyCursor();
    std::ifstream fp(path.mb_str(), std::ios::in | std::ios::binary);
    constexpr const std::size_t buffer_size { 1 << 12 };
    char buffer[buffer_size];
    unsigned char *hash;
    unsigned int hash_len = EVP_MD_size(EVP_sha256());
    EVP_MD_CTX *ctx;
    ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    while(fp.good())
    {
        fp.read(buffer, buffer_size);
        EVP_DigestUpdate(ctx, buffer, fp.gcount());
    }
    hash = (unsigned char *)OPENSSL_malloc(hash_len);
    EVP_DigestFinal_ex(ctx, hash, &hash_len);
    EVP_MD_CTX_free(ctx);
    fp.close();
    std::ostringstream os;
    os << std::hex << std::setfill('0');
    for(unsigned int i = 0; i < hash_len; ++i)
    {
        os << std::setw(2) << static_cast<unsigned int>(hash[i]);
    }
    wxEndBusyCursor();
    return MyUtilFunctions::FromStdString(os.str());
}

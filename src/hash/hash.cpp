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
    unsigned char hash[MD5_DIGEST_LENGTH] = { 0 };
    MD5_CTX ctx;
    MD5_Init(&ctx);
    while(fp.good())
    {
        fp.read(buffer, buffer_size);
        MD5_Update(&ctx, buffer, fp.gcount());
    }
    MD5_Final(hash, &ctx);
    fp.close();
    std::ostringstream os;
    os << std::hex << std::setfill('0');
    for(int i = 0; i < MD5_DIGEST_LENGTH; ++i)
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
    unsigned char hash[SHA_DIGEST_LENGTH] = { 0 };
    SHA_CTX ctx;
    SHA1_Init(&ctx);
    while(fp.good())
    {
        fp.read(buffer, buffer_size);
        SHA1_Update(&ctx, buffer, fp.gcount());
    }
    SHA1_Final(hash, &ctx);
    fp.close();
    std::ostringstream os;
    os << std::hex << std::setfill('0');
    for(int i = 0; i < SHA_DIGEST_LENGTH; ++i)
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
    unsigned char hash[SHA256_DIGEST_LENGTH] = { 0 };
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    while(fp.good())
    {
        fp.read(buffer, buffer_size);
        SHA256_Update(&ctx, buffer, fp.gcount());
    }
    SHA256_Final(hash, &ctx);
    fp.close();
    std::ostringstream os;
    os << std::hex << std::setfill('0');
    for(int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
    {
        os << std::setw(2) << static_cast<unsigned int>(hash[i]);
    }
    wxEndBusyCursor();
    return MyUtilFunctions::FromStdString(os.str());
}

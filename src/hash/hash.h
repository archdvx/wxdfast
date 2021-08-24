/***************************************************************
 * Name:      hash.h
 * Author:    David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2021
 * License:   GPL2
 **************************************************************/

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <openssl/md5.h>
#include <openssl/sha.h>

#ifndef MHASH_H
#define MHASH_H


class mHash
{
public:
    mHash();

    static wxString MD5(const wxString &path);
    static wxString SHA1(const wxString &path);
    static wxString SHA256(const wxString &path);
};

#endif // MHASH_H

//////////////////////////////////////////////////////////////////////
//    Name:                wxMD5.cpp
//    Purpose:            wxMD5 Class
//    Author:                Casey O'Donnell
//    Creator:            See Internet RFC 1321
//                        Copyright (C) 1991 - 1992
//                        RSA Data Security, Inc.  Created 1991
//    Created:            07/02/2003
//    Last modified:        07/02/2003
//    Licence:            wxWindows license
//////////////////////////////////////////////////////////////////////

// wxMD5.cpp: implementation of the wxMD5 class.
//
//////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "wxMD5.h"
#endif

// for compilers that support precompilation, includes "wx.h"
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others
#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/file.h"
#endif
#include "md5.h"
#include "wxMD5.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

wxMD5::wxMD5()
{
    m_bCalculatedDigest = false;
    m_pszDigestString[32] = '\0';
    m_isfile = FALSE;
}

wxMD5::wxMD5(const wxString& szText)
{
    m_bCalculatedDigest = false;
    m_pszDigestString[32] = '\0';
    m_szText = szText;
    m_isfile = FALSE;
}

wxMD5::wxMD5(const wxFileName& szfile)
{
    m_bCalculatedDigest = false;
    m_pszDigestString[32] = '\0';
    m_file = szfile;
    m_isfile = TRUE;
}

wxMD5::~wxMD5()
{
}

//////////////////////////////////////////////////////////////////////
// Other Methods
//////////////////////////////////////////////////////////////////////

void wxMD5::SetText(const wxString& szText)
{
    m_bCalculatedDigest = false;
    m_szText = szText;
    m_isfile = FALSE;
}

void wxMD5::SetFile(const wxFileName& szfile)
{
    m_bCalculatedDigest = false;
    m_file = szfile;
    m_isfile = TRUE;
}

const wxString wxMD5::GetDigest(bool mainthread)
{
    if (m_isfile)
    {
        if(m_bCalculatedDigest)
        {
            const wxString szRetVal = m_pszDigestString;
            return szRetVal;
        }
        else if(!m_file.FileExists())
        {
            return wxEmptyString;
        }
        else
        {
            MD5_CTX md5Context;
            MD5Init(&md5Context);
            wxFile md5file(m_file.GetFullPath(), wxFile::read);
            unsigned char buffer[16384];
            unsigned int i = 1;
            while (i >0)
            {
                i = md5file.Read(buffer,16384);
                MD5Update(&md5Context, buffer, (unsigned) i);
                if (mainthread)
                    wxYield();
            }
            MD5Final(m_arrDigest, &md5Context);

            wxString szRetVal;
            unsigned int j=0;
            for (i = 0; i < sizeof m_arrDigest; i++)
            {
                szRetVal << wxString::Format(wxT("%02X"),m_arrDigest[i]);
                m_pszDigestString[j] = szRetVal.GetChar(j);
                m_pszDigestString[j+1] = szRetVal.GetChar(j+1);
                j+=2;
            }
            return szRetVal;
        }
    }
    else
    {
        if(m_bCalculatedDigest)
        {
            const wxString szRetVal = m_pszDigestString;
            return szRetVal;
        }
        else if(m_szText.IsEmpty())
        {
            return wxEmptyString;
        }
        else
        {
            MD5_CTX md5Context;
            MD5Init(&md5Context);
            char *text = new char[m_szText.Len()+1];
            unsigned int i;
            for (i=0; i < (m_szText.Len());i++)
                text[i] = m_szText.GetChar(i);
            text[i] = '\0';

            MD5Update(&md5Context, (unsigned char*)(text), strlen(text));
            MD5Final(m_arrDigest, &md5Context);

            wxString szRetVal;
            unsigned int j=0;
            for (i = 0; i < sizeof m_arrDigest; i++)
            {
                szRetVal << wxString::Format(wxT("%02X"),m_arrDigest[i]);
                m_pszDigestString[j] = szRetVal.GetChar(j);
                m_pszDigestString[j+1] = szRetVal.GetChar(j+1);
                j+=2;
            }
            return szRetVal;
        }
    }
}

//////////////////////////////////////////////////////////////////////
// Static Methods
//////////////////////////////////////////////////////////////////////

const wxString wxMD5::GetDigest(const wxString& szText)
{
    wxMD5 md5(szText);

    return md5.GetDigest();
}

const wxString wxMD5::GetDigest(const wxFileName& szfile)
{
    wxMD5 md5(szfile);

    return md5.GetDigest();
}

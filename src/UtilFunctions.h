/***************************************************************
 * Name:      UtilFunctions.h
 * Author:    Max Magalhães Velasques (maxvelasques@gmail.com)
 *            David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2006
 * License:   GPL2
 **************************************************************/

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <vector>

#ifndef MYUTILFUNCTIONS_H
#define MYUTILFUNCTIONS_H

class MyUtilFunctions
{
public:
    static wxString TimespanToWxstr(const uint64_t &span);
    static wxString IntToWxstr(long value,int format = 0);
    /**
     * @brief SpeedText
     * @param speed in bytes
     * @param down, true = download
     * @param showPrefix, true = "D: " or "U: "
     * @return
     */
    static wxString SpeedText(int speed, bool down, bool showPrefix=true);
    static wxString FromStdString(const std::string &str);
    static std::string ToStdString(const wxString &str);
    static wxString SizeText(int64_t size);
    static std::vector<std::string> ArrayStringToVector(const wxArrayString &array);
    static wxString ArrayStringTowxString(const wxArrayString &array);
    static wxString MimeType(const wxString &fname);
    /**
     * @brief BrowserIntegration
     * @param path - where is wxdfast, wdfast-integrator is on same path
     */
    static void BrowserIntegration(const wxString &path);
    static void CheckBrowser(const wxString &browserpath, const wxString &integratorpath, bool firefox, wxArrayString ids);
    static void CreateFirefoxJson(const wxString &jsonpath, const wxString &integratorpath);
    static void CreateChromeJson(const wxString &jsonpath, const wxString &integratorpath, wxArrayString ids);
};
#endif //MYUTILFUNCTIONS_H

/***************************************************************
 * Name:      UtilFunctions.h
 * Author:    Max Magalhães Velasques (maxvelasques@gmail.com)
 *            David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2006
 * License:   GPL2
 **************************************************************/

#include <fstream>
#include <wx/mimetype.h>
#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/stdpaths.h>
#include "wxjson/include/wx/jsonreader.h"
#include "wxjson/include/wx/jsonval.h"
#include "UtilFunctions.h"

wxString MyUtilFunctions::IntToWxstr(long value,int format)
{
    wxString string;
    string << "%0" << format << "ld";
    return wxString::Format(string,value);
}

wxString MyUtilFunctions::SpeedText(int speed, bool down, bool showPrefix)
{
    if(speed >= 1024. * 1024. * 1024.)
    {
        if(showPrefix) return wxString::Format("%s %.2f GB/s", down?"D:":"U:", speed/1024./1024./1024.);
        else return wxString::Format("%.2f GB/s", speed/1024./1024./1024.);
    }
    else if(speed >= 1024. * 1024.)
    {
        if(showPrefix) return wxString::Format("%s %.2f MB/s", down?"D:":"U:", speed/1024./1024.);
        else return wxString::Format("%.2f MB/s", speed/1024./1024.);
    }
    else if(speed >= 1024.)
    {
        if(showPrefix) return wxString::Format("%s %.2f KB/s", down?"D:":"U:", speed/1024.);
        else return wxString::Format("%.2f KB/s", speed/1024.);
    }
    else
    {
        if(showPrefix) return wxString::Format("%s %.2f B/s", down?"D:":"U:", static_cast<double>(speed));
        else return wxString::Format("%.2f B/s", static_cast<double>(speed));
    }
}

wxString MyUtilFunctions::FromStdString(const std::string &str)
{
    if(str.empty()) return "";
    return wxString::FromUTF8(str.c_str());
}

std::string MyUtilFunctions::ToStdString(const wxString &str)
{
    if(str.empty()) return "";
    return str.ToStdString();
}

wxString MyUtilFunctions::SizeText(int64_t size)
{
    if(size >= 1024. * 1024. * 1024.)
    {
        return wxString::Format("%.2f GB", size/1024./1024./1024.);
    }
    else if(size >= 1024. * 1024.)
    {
        return wxString::Format("%.2f MB", size/1024./1024.);
    }
    else if(size >= 1024.)
    {
        return wxString::Format("%.2f KB", size/1024.);
    }
    else
    {
        return wxString::Format("%lld B", size);
    }
}

std::vector<std::string> MyUtilFunctions::ArrayStringToVector(const wxArrayString &array)
{
    std::vector<std::string> vector;
    for(size_t i=0; i < array.GetCount(); i++)
    {
        vector.push_back(ToStdString(array.Item(i)));
    }
    return vector;
}

wxString MyUtilFunctions::ArrayStringTowxString(const wxArrayString &array)
{
    wxString result;
    for(size_t i=0; i < array.GetCount(); i++)
    {
        result << array.Item(i);
        if(i+1 != array.GetCount()) result << wxPATH_SEP;
    }
    return result;
}

wxString MyUtilFunctions::MimeType(const wxString &fname)
{
    wxString type = "";
    if(!wxFileName(fname).GetExt().IsEmpty())
    {
        wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(wxFileName(fname).GetExt());
        if(ft)
        {
            ft->GetMimeType(&type);
            delete ft;
        }
    }
    return type;
}

void MyUtilFunctions::BrowserIntegration(const wxString &path)
{
#ifdef __UNIX__
    wxString integrator;
    if(!wxFileExists(path+wxFILE_SEP_PATH+"wxdfast-integrator")) //wxdfast run from build folder
    {
        integrator = path+wxFILE_SEP_PATH+"integrator/wxdfast-integrator";
    }
    else
    {
        integrator = path+wxFILE_SEP_PATH+"wxdfast-integrator";
    }
    wxArrayString ids;
#ifdef __WXMAC__
    wxString idpath = wxStandardPaths::Get().GetResourcesDir()+"/extensionid";
#else
    wxString idpath = wxString(WXDFAST_DATADIR)+wxFILE_SEP_PATH+"extensionid";
#endif
    if(wxFileExists(idpath))
    {
        wxFile source(idpath, wxFile::read);
        source.Open(idpath);
        wxString text;
        source.ReadAll(&text);
        while(text.Find('\n') != wxNOT_FOUND)
        {
            ids.Add(text.BeforeFirst('\n'));
            text = text.AfterFirst('\n');
        }
        source.Close();
    }
#ifdef __WXMAC__
    //Firefox path
    CheckBrowser(wxGetHomeDir()+"/Library/Application Support/Mozilla", integrator, true, ids);
    //Chrome
    CheckBrowser(wxGetHomeDir()+"/Library/Application Support/Google/Chrome", integrator, false, ids);
    //Chromium
    CheckBrowser(wxGetHomeDir()+"/Library/Application Support/Chromium", integrator, false, ids);
    //Vivaldi
    CheckBrowser(wxGetHomeDir()+"/Library/Application Support/Vivaldi", integrator, false, ids);
    //Opera
    CheckBrowser(wxGetHomeDir()+"/Library/Application Support/Opera", integrator, false, ids);
    //Brave Bowser
    CheckBrowser(wxGetHomeDir()+"/Library/Application Support/BraveSoftware/Brave-Browser", integrator, false, ids);
#else
    //Firefox path
    CheckBrowser(wxGetHomeDir()+"/.mozilla", integrator, true, ids);
    //Chrome
    CheckBrowser(wxGetHomeDir()+"/.config/google-chrome", integrator, false, ids);
    //Chromium
    CheckBrowser(wxGetHomeDir()+"/.config/chromium", integrator, false, ids);
    //Vivaldi
    CheckBrowser(wxGetHomeDir()+"/.config/vivaldi", integrator, false, ids);
    //Opera
    CheckBrowser(wxGetHomeDir()+"/.config/opera", integrator, false, ids);
    //Brave Bowser
    CheckBrowser(wxGetHomeDir()+"/.config/BraveSoftware/Brave-Browser", integrator, false, ids);
#endif
#endif
}

void MyUtilFunctions::CheckBrowser(const wxString &browserpath, const wxString &integratorpath, bool firefox, wxArrayString ids)
{
    if(firefox)
    {
#ifdef __WXMAC__
        wxString host = "/NativeMessagingHosts";
#else
        wxString host = "/native-messaging-hosts";
#endif
        if(wxDirExists(browserpath))
        {
            if(!wxDirExists(browserpath+host))
            {
                wxMkdir(browserpath+host);
            }
            if(wxFileExists(browserpath+host+"/com.wxdfast.firefox.json")) //check path
            {
                wxJSONValue root;
                wxJSONReader reader;
                wxFFileInputStream stream(browserpath+host+"/com.wxdfast.firefox.json");
                reader.Parse(stream, &root);
                if(!root.HasMember("path"))
                {
                    wxRemoveFile(browserpath+host+"/com.wxdfast.firefox.json");
                    CreateFirefoxJson(browserpath+host+"/com.wxdfast.firefox.json", integratorpath);
                }
                else
                {
                    if(root["path"].AsString() != integratorpath)
                    {
                        wxRemoveFile(browserpath+host+"/com.wxdfast.firefox.json");
                        CreateFirefoxJson(browserpath+host+"/com.wxdfast.firefox.json", integratorpath);
                    }
                }
            }
            else //create new json
            {
                CreateFirefoxJson(browserpath+host+"/com.wxdfast.firefox.json", integratorpath);
            }
        }
    }
    else
    {
        if(wxDirExists(browserpath))
        {
            if(!wxDirExists(browserpath+"/NativeMessagingHosts"))
            {
                wxMkdir(browserpath+"/NativeMessagingHosts");
            }
            if(wxFileExists(browserpath+"/NativeMessagingHosts/com.wxdfast.chrome.json")) //check path
            {
                wxJSONValue root;
                wxJSONReader reader;
                wxFFileInputStream stream(browserpath+"/NativeMessagingHosts/com.wxdfast.chrome.json");
                reader.Parse(stream, &root);
                if(!root.HasMember("path"))
                {
                    wxRemoveFile(browserpath+"/NativeMessagingHosts/com.wxdfast.chrome.json");
                    CreateChromeJson(browserpath+"/NativeMessagingHosts/com.wxdfast.chrome.json", integratorpath, ids);
                }
                else
                {
                    if(root["path"].AsString() != integratorpath)
                    {
                        wxRemoveFile(browserpath+"/NativeMessagingHosts/com.wxdfast.chrome.json");
                        CreateChromeJson(browserpath+"/NativeMessagingHosts/com.wxdfast.chrome.json", integratorpath, ids);
                    }
                }
                if(!root.HasMember("allowed_origins"))
                {
                    wxRemoveFile(browserpath+"/NativeMessagingHosts/com.wxdfast.chrome.json");
                    CreateChromeJson(browserpath+"/NativeMessagingHosts/com.wxdfast.chrome.json", integratorpath, ids);
                }
                else
                {
                    wxString exts = root["allowed_origins"].AsString();
                    for(size_t i=0; i < ids.GetCount(); i++)
                    {
                        if(exts.Find(ids[i]) == wxNOT_FOUND)
                        {
                            wxRemoveFile(browserpath+"/NativeMessagingHosts/com.wxdfast.chrome.json");
                            CreateChromeJson(browserpath+"/NativeMessagingHosts/com.wxdfast.chrome.json", integratorpath, ids);
                            break;
                        }
                    }
                }
            }
            else //create new json
            {
                CreateChromeJson(browserpath+"/NativeMessagingHosts/com.wxdfast.chrome.json", integratorpath, ids);
            }
        }
    }
}

void MyUtilFunctions::CreateFirefoxJson(const wxString &jsonpath, const wxString &integratorpath)
{
    std::ofstream *json = new std::ofstream(jsonpath.ToStdString(), std::ios::out|std::ios::app);
    *json << "{\"name\":\"com.wxdfast.firefox\",\"description\":\"Integrate wxDownload Fast with Mozilla Firefox\",\"path\":";
    *json << "\"";
    *json << integratorpath.ToStdString();
    *json << "\",";
    *json << "\"type\":\"stdio\",";
    *json << "\"allowed_extensions\":[\"wxdfast@archdvx\"]}";
    *json << std::endl;
    json->close();
    delete json;
}

void MyUtilFunctions::CreateChromeJson(const wxString &jsonpath, const wxString &integratorpath, wxArrayString ids)
{
    std::ofstream *json = new std::ofstream(jsonpath.ToStdString(), std::ios::out|std::ios::app);
    *json << "{\"name\":\"com.wxdfast.chrome\",\"description\":\"Integrate wxDownload Fast with Google Chrome and similar\",\"path\":";
    *json << "\"";
    *json << integratorpath.ToStdString();
    *json << "\",";
    *json << "\"type\":\"stdio\",\"allowed_origins\":[";
    for(size_t i=0; i < ids.GetCount(); i++)
    {
        *json << "\"chrome-extension://" << ids[i] << "/\"";
        if(ids.GetCount() > 1 && i+1 != ids.GetCount()) *json << ",";
    }
    *json << "]}";
    *json << std::endl;
    json->close();
    delete json;
}

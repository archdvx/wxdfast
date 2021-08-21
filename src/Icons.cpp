/***************************************************************
 * Name:      Icons.cpp
 * Author:    David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2021
 * License:   GPL2
 **************************************************************/

#include <wx/icon.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/mstream.h>
#include <wx/fileconf.h>
#include <wx/stdpaths.h>
#include "Icons.h"

static const char *const  empty[] = {
"16 16 11 1",
"   c None",
".  c #000000",
"+  c #FFFFFF",
"@  c #FDE4E4",
"#  c #F45C5C",
"$  c #EF0B0B",
"%  c #F12727",
"&  c #F67878",
"*  c #FAAEAE",
"=  c #FBC9C9",
"-  c #F89393",
"................",
".              .",
".              .",
".    @#$$%&    .",
".   @%$$$$$&   .",
".   &$$#*%$$   .",
".   =-#+@%$%   .",
".      @%$$*   .",
".      %$%@    .",
".     *$$*     .",
".     @**@     .",
".     *$$*     .",
".     *$$*     .",
".     @**@     .",
".              .",
"................"};

wxIcon ICO_ICON,
 ICO_TRAY,
 ICO_LOGO,
 ICO_ABOUT,
 ICO_COMPLETED,
 ICO_COPYDATA,
 ICO_COPYURL,
 ICO_DETAILS,
 ICO_DOWNLOADING,
 ICO_ERROR,
 ICO_FIND,
 ICO_GRAPH,
 ICO_HELP,
 ICO_NEW,
 ICO_OPTIONS,
 ICO_PASTEURL,
 ICO_PAUSE,
 ICO_PROGRESSBAR,
 ICO_PROPERTIES,
 ICO_QUIT,
 ICO_REMOVE,
 ICO_SCHEDULE,
 ICO_SCHEDULED,
 ICO_START,
 ICO_STARTALL,
 ICO_STOP,
 ICO_STOPALL,
 ICO_DOWNLOAD_INFO,
 ICO_DOWNLOAD_MOVE_DOWN,
 ICO_DOWNLOAD_MOVE_UP,
 ICO_DOWNLOAD_NEW,
 ICO_DOWNLOAD_REMOVE,
 ICO_DOWNLOAD_SCHEDULE,
 ICO_DOWNLOAD_START,
 ICO_DOWNLOAD_START_ALL,
 ICO_DOWNLOAD_STOP,
 ICO_DOWNLOAD_STOP_ALL,
 ICO_GDOWNLOAD_INFO,
 ICO_GDOWNLOAD_MOVE_DOWN,
 ICO_GDOWNLOAD_MOVE_UP,
 ICO_GDOWNLOAD_NEW,
 ICO_GDOWNLOAD_REMOVE,
 ICO_GDOWNLOAD_SCHEDULE,
 ICO_GDOWNLOAD_START,
 ICO_GDOWNLOAD_START_ALL,
 ICO_GDOWNLOAD_STOP,
 ICO_GDOWNLOAD_STOP_ALL;

wxIcon makeIcon(const wxString &path, const wxString &name)
{
    if(wxFileExists(path+wxFILE_SEP_PATH+name))
    {
        return wxIcon(path+wxFILE_SEP_PATH+name, wxBITMAP_TYPE_PNG);
    }
    else
    {
        wxIcon icon(empty);
        return icon;
    }
}

wxIcon makeGreyedIcon(const wxString &path, const wxString &name)
{
    if(wxFileExists(path+wxFILE_SEP_PATH+name))
    {
        wxImage img(path+wxFILE_SEP_PATH+name, wxBITMAP_TYPE_PNG);
        wxIcon icon;
        icon.CopyFromBitmap(wxBitmap(img.ConvertToGreyscale()));
        return icon;
    }
    wxImage img(empty);
    wxIcon icon;
    icon.CopyFromBitmap(wxBitmap(img.ConvertToGreyscale()));
    return icon;
}

wxIcon makeScaledIcon(const wxString &path, int max)
{
    if(wxFileExists(path))
    {
        wxImage img(path);
        int w = img.GetWidth();
        int h = img.GetHeight();
        if(w>max || h>max)
        {
            if(w>h)
                img = img.Scale(max, (max*h)/w);
            else
                img = img.Scale((max*w)/h, max);
        }
        wxIcon icon;
        icon.CopyFromBitmap(wxBitmap(img));
        return icon;
    }
    return wxIcon(empty);
}

void makeAllIcons(const wxString &path)
{
    wxString themepath;
    if(path == "default")
    {
#if defined (__WXMAC__)
        themepath = wxStandardPaths::Get().GetResourcesDir()+"/icons";
#else
        themepath = wxString(WXDFAST_DATADIR)+wxFILE_SEP_PATH+"icons";
#endif
    }
    else
    {
        themepath = path;
    }
    ICO_ICON = makeIcon(themepath, "wxdfast.png");
    ICO_TRAY = makeIcon(themepath, "wxdfast.png");
    ICO_LOGO = makeIcon(themepath+wxFILE_SEP_PATH+"logo", "about.png");
    ICO_ABOUT = makeIcon(themepath+wxFILE_SEP_PATH+"menubar", "about.png");
    ICO_COMPLETED = makeIcon(themepath+wxFILE_SEP_PATH+"menubar", "completed.png");
    ICO_COPYDATA = makeIcon(themepath+wxFILE_SEP_PATH+"menubar", "copydata.png");
    ICO_COPYURL = makeIcon(themepath+wxFILE_SEP_PATH+"menubar", "copyurl.png");
    ICO_DETAILS = makeIcon(themepath+wxFILE_SEP_PATH+"menubar", "details.png");
    ICO_DOWNLOADING = makeIcon(themepath+wxFILE_SEP_PATH+"menubar", "downloading.png");
    ICO_ERROR = makeIcon(themepath+wxFILE_SEP_PATH+"menubar", "error.png");
    ICO_FIND = makeIcon(themepath+wxFILE_SEP_PATH+"menubar", "find.png");
    ICO_GRAPH = makeIcon(themepath+wxFILE_SEP_PATH+"menubar", "graph.png");
    ICO_HELP = makeIcon(themepath+wxFILE_SEP_PATH+"menubar", "help.png");
    ICO_NEW = makeIcon(themepath+wxFILE_SEP_PATH+"menubar", "new.png");
    ICO_OPTIONS = makeIcon(themepath+wxFILE_SEP_PATH+"menubar", "options.png");
    ICO_PASTEURL = makeIcon(themepath+wxFILE_SEP_PATH+"menubar", "pasteurl.png");
    ICO_PAUSE = makeIcon(themepath+wxFILE_SEP_PATH+"menubar", "pause.png");
    ICO_PROGRESSBAR = makeIcon(themepath+wxFILE_SEP_PATH+"menubar", "progressbar.png");
    ICO_PROPERTIES = makeIcon(themepath+wxFILE_SEP_PATH+"menubar", "properties.png");
    ICO_QUIT = makeIcon(themepath+wxFILE_SEP_PATH+"menubar", "quit.png");
    ICO_REMOVE = makeIcon(themepath+wxFILE_SEP_PATH+"menubar", "remove.png");
    ICO_SCHEDULE = makeIcon(themepath+wxFILE_SEP_PATH+"menubar", "schedule.png");
    ICO_SCHEDULED = makeIcon(themepath+wxFILE_SEP_PATH+"menubar", "scheduled.png");
    ICO_START = makeIcon(themepath+wxFILE_SEP_PATH+"menubar", "start.png");
    ICO_STARTALL = makeIcon(themepath+wxFILE_SEP_PATH+"menubar", "startall.png");
    ICO_STOP = makeIcon(themepath+wxFILE_SEP_PATH+"menubar", "stop.png");
    ICO_STOPALL = makeIcon(themepath+wxFILE_SEP_PATH+"menubar", "stopall.png");
    ICO_DOWNLOAD_INFO = makeIcon(themepath+wxFILE_SEP_PATH+"toolbar", "download_info.png");
    ICO_DOWNLOAD_MOVE_DOWN = makeIcon(themepath+wxFILE_SEP_PATH+"toolbar", "download_move_down.png");
    ICO_DOWNLOAD_MOVE_UP = makeIcon(themepath+wxFILE_SEP_PATH+"toolbar", "download_move_up.png");
    ICO_DOWNLOAD_NEW = makeIcon(themepath+wxFILE_SEP_PATH+"toolbar", "download_new.png");
    ICO_DOWNLOAD_REMOVE = makeIcon(themepath+wxFILE_SEP_PATH+"toolbar", "download_remove.png");
    ICO_DOWNLOAD_SCHEDULE = makeIcon(themepath+wxFILE_SEP_PATH+"toolbar", "download_schedule.png");
    ICO_DOWNLOAD_START = makeIcon(themepath+wxFILE_SEP_PATH+"toolbar", "download_start.png");
    ICO_DOWNLOAD_START_ALL = makeIcon(themepath+wxFILE_SEP_PATH+"toolbar", "download_start_all.png");
    ICO_DOWNLOAD_STOP = makeIcon(themepath+wxFILE_SEP_PATH+"toolbar", "download_stop.png");
    ICO_DOWNLOAD_STOP_ALL = makeIcon(themepath+wxFILE_SEP_PATH+"toolbar", "download_stop_all.png");
    ICO_GDOWNLOAD_INFO = makeGreyedIcon(themepath+wxFILE_SEP_PATH+"toolbar", "download_info.png");
    ICO_GDOWNLOAD_MOVE_DOWN = makeGreyedIcon(themepath+wxFILE_SEP_PATH+"toolbar", "download_move_down.png");
    ICO_GDOWNLOAD_MOVE_UP = makeGreyedIcon(themepath+wxFILE_SEP_PATH+"toolbar", "download_move_up.png");
    ICO_GDOWNLOAD_NEW = makeGreyedIcon(themepath+wxFILE_SEP_PATH+"toolbar", "download_new.png");
    ICO_GDOWNLOAD_REMOVE = makeGreyedIcon(themepath+wxFILE_SEP_PATH+"toolbar", "download_remove.png");
    ICO_GDOWNLOAD_SCHEDULE = makeGreyedIcon(themepath+wxFILE_SEP_PATH+"toolbar", "download_schedule.png");
    ICO_GDOWNLOAD_START = makeGreyedIcon(themepath+wxFILE_SEP_PATH+"toolbar", "download_start.png");
    ICO_GDOWNLOAD_START_ALL = makeGreyedIcon(themepath+wxFILE_SEP_PATH+"toolbar", "download_start_all.png");
    ICO_GDOWNLOAD_STOP = makeGreyedIcon(themepath+wxFILE_SEP_PATH+"toolbar", "download_stop.png");
    ICO_GDOWNLOAD_STOP_ALL = makeGreyedIcon(themepath+wxFILE_SEP_PATH+"toolbar", "download_stop_all.png");
}

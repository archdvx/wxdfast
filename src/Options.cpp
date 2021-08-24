/***************************************************************
 * Name:      Options.cpp
 * Author:    Max Magalhães Velasques (maxvelasques@gmail.com)
 *            David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2006
 * License:   GPL2
 **************************************************************/

#include <wx/stdpaths.h>
#include <wx/fileconf.h>
#include "Options.h"

mOptions::mOptions()
{
    wxString iniPath = wxStandardPaths::Get().GetUserDataDir();
    if(!wxDirExists(iniPath))
    {
        wxMkdir(iniPath);
    }
    iniPath << wxFILE_SEP_PATH << "wxdfast.ini";
    wxFileConfig *conf = new wxFileConfig("APPNAME", "COMPANYNAME", iniPath);
    wxConfigBase::Set(conf);

    m_x = -1;
    m_y = -1;
    m_w = 700;
    m_h = 500;
    m_separator1 = 100;
    m_separator2 = 100;

    m_lastnewreleasecheck = wxDateTime::Now();
    m_attempts = 5;
    m_closedialog = false;
    m_simultaneous = 5;
    m_attemptstime = 5;
    m_shutdown = false;
    m_disconnect = false;
    m_alwaysshutdown = false;
    m_alwaysdisconnect = false;
    m_timerupdateinterval = 500;
    m_restoremainframe = true;
    m_hidemainframe = false;
    m_checkforupdates = false;
    m_rememberboxnewoptions = false;
    m_shownotify = true;
    m_md5 = true;
    m_sha1 = false;
    m_sha256 = false;

    m_graphshow = true;
    m_graphrefreshtime = 500;
    m_graphscale = 1024;
    m_graphtextarea = 110;
    m_graphheight = 65;
    m_graphspeedfontsize = 10;
    m_graphbackcolor = *wxBLACK;
    m_graphgridcolor = *wxGREEN;
    m_graphlinecolor = *wxRED;
    m_graphuploadcolor = *wxYELLOW;
    m_graphlinewidth = 1;
    m_graphfontcolor = *wxBLUE;

#ifdef __WXMSW__
    m_shutdowncmd = wxGetOSDirectory() + "\\system32\\shutdown.exe -s -t 0";
    m_disconnectcmd = wxGetOSDirectory() + "\\system32\\rasdial.exe /disconnect";
    m_destination = wxGetHomeDir();
    m_filemanagerpath = wxGetOSDirectory() + "\\explorer.exe";
#else
    m_shutdowncmd = "shutdown -h now";
    m_disconnectcmd = "poff";
    m_destination = wxGetHomeDir();
    m_filemanagerpath = "xfe";
#endif

    m_activatescheduling = false;
    m_startdatetime = wxDateTime::Now();
    m_startdatetime.SetSecond(0);
    m_finishdatetime = wxDateTime::Now()+wxTimeSpan::Hours(4);
    for(size_t i=0; i < MAX_SCHEDULE_EXCEPTIONS; i++)
    {
        m_scheduleexceptions[i].isactive = false;
    }

    m_bandwidthoption = 0;
    m_livebandwidthoption = 0;
    m_bandwidth = 100;

    m_proxy = false;
    m_proxyServer = "";
    m_proxyPort = "";
    m_proxyUsername = "";
    m_proxyAuthstring = "";

    m_finishedpiece = *wxGREEN;
    m_unfinishedpiece = *wxLIGHT_GREY;

    m_lastx = -1;
    m_lasty = -1;
    m_lastnumberofconnections = 5;
    m_laststartoption = 1;
    m_lastdestination = wxGetHomeDir();
    m_lastcommand = "";
    m_lastontopoption = false;
    m_lastbandwidth = 0;
}

mOptions::~mOptions()
{
}

mOptions &mOptions::instance()
{
    static mOptions s;
    return s;
}

void mOptions::load(const wxString &path)
{
    if(!path.IsEmpty())
    {
        wxFileConfig *conf = new wxFileConfig("APPNAME", "COMPANYNAME", path);
        wxConfigBase::Set(conf);
    }
    //GUI
    wxConfigBase::Get()->Read("GUI/x", &m_x, m_x);
    wxConfigBase::Get()->Read("GUI/y", &m_y, m_y);
    wxConfigBase::Get()->Read("GUI/w", &m_w, m_w);
    wxConfigBase::Get()->Read("GUI/h", &m_h, m_h);
    wxConfigBase::Get()->Read("GUI/separator1", &m_separator1, m_separator1);
    if(m_separator1 < 20)
        m_separator1 = 100;
    wxConfigBase::Get()->Read("GUI/separator2", &m_separator2, m_separator2);
    if(m_separator2 < 20)
        m_separator2 = 100;
    //General
    wxString value = wxEmptyString;
    wxConfigBase::Get()->Read("General/lastnewreleasecheck", &value, m_lastnewreleasecheck.FormatISOCombined());
    m_lastnewreleasecheck.ParseISOCombined(value);
    wxConfigBase::Get()->Read("General/attempts", &m_attempts, m_attempts);
    wxConfigBase::Get()->Read("General/closedialog", &m_closedialog, m_closedialog);
    wxConfigBase::Get()->Read("General/simultaneous", &m_simultaneous, m_simultaneous);
    if(m_simultaneous < 1)
        m_simultaneous = 1;
    wxConfigBase::Get()->Read("General/attemptstime", &m_attemptstime, m_attemptstime);
    wxConfigBase::Get()->Read("General/alwaysshutdown", &m_alwaysshutdown, m_alwaysshutdown);
    wxConfigBase::Get()->Read("General/alwaysdisconnect", &m_alwaysdisconnect, m_alwaysdisconnect);
    wxConfigBase::Get()->Read("General/timerupdateinterval", &m_timerupdateinterval, m_timerupdateinterval);
    if(m_timerupdateinterval < 100 || m_timerupdateinterval > 1000)
        m_timerupdateinterval = 100;
    wxConfigBase::Get()->Read("General/restoremainframe", &m_restoremainframe, m_restoremainframe);
    wxConfigBase::Get()->Read("General/hidemainframe", &m_hidemainframe, m_hidemainframe);
    wxConfigBase::Get()->Read("General/checkforupdates", &m_checkforupdates, m_checkforupdates);
    wxConfigBase::Get()->Read("General/rememberboxnewoptions", &m_rememberboxnewoptions, m_rememberboxnewoptions);
    wxConfigBase::Get()->Read("General/shownotify", &m_shownotify, m_shownotify);
    wxConfigBase::Get()->Read("General/md5", &m_md5, m_md5);
    wxConfigBase::Get()->Read("General/sha1", &m_sha1, m_sha1);
    wxConfigBase::Get()->Read("General/sha256", &m_sha256, m_sha256);
    //Graph
    wxConfigBase::Get()->Read("Graph/graphshow", &m_graphshow, m_graphshow);
    wxConfigBase::Get()->Read("Graph/graphrefreshtime", &m_graphrefreshtime, m_graphrefreshtime);
    if(m_graphrefreshtime < 50 || m_graphrefreshtime > 1000)
        m_graphrefreshtime = 50;
    wxConfigBase::Get()->Read("Graph/graphscale", &m_graphscale, m_graphscale);
    wxConfigBase::Get()->Read("Graph/graphtextarea", &m_graphtextarea, m_graphtextarea);
    wxConfigBase::Get()->Read("Graph/graphheight", &m_graphheight, m_graphheight);
    wxConfigBase::Get()->Read("Graph/graphspeedfontsize", &m_graphspeedfontsize, m_graphspeedfontsize);
    m_graphbackcolor.Set(wxConfigBase::Get()->Read("Graph/graphbackcolor", m_graphbackcolor.GetAsString()));
    m_graphgridcolor.Set(wxConfigBase::Get()->Read("Graph/graphgridcolor", m_graphgridcolor.GetAsString()));
    m_graphlinecolor.Set(wxConfigBase::Get()->Read("Graph/graphlinecolor", m_graphlinecolor.GetAsString()));
    m_graphuploadcolor.Set(wxConfigBase::Get()->Read("Graph/graphuploadcolor", m_graphuploadcolor.GetAsString()));
    wxConfigBase::Get()->Read("Graph/graphlinewidth", &m_graphlinewidth, m_graphlinewidth);
    m_graphfontcolor.Set(wxConfigBase::Get()->Read("Graph/graphfontcolor", m_graphfontcolor.GetAsString()));
    //Path
    wxConfigBase::Get()->Read("Path/shutdowncmd", &m_shutdowncmd, m_shutdowncmd);
    wxConfigBase::Get()->Read("Path/disconnectcmd", &m_disconnectcmd, m_disconnectcmd);
    wxConfigBase::Get()->Read("Path/destination", &m_destination, m_destination);
    wxConfigBase::Get()->Read("Path/filemanagerpath", &m_filemanagerpath, m_filemanagerpath);
    //Schedule
    wxConfigBase::Get()->Read("Schedule/activatescheduling", &m_activatescheduling, m_activatescheduling);
    wxConfigBase::Get()->Read("Schedule/startdatetime", &value, m_startdatetime.FormatISOCombined());
    m_startdatetime.ParseISOCombined(value);
    wxConfigBase::Get()->Read("Schedule/finishdatetime", &value, m_finishdatetime.FormatISOCombined());
    m_finishdatetime.ParseISOCombined(value);
    if(m_startdatetime.IsLaterThan(m_finishdatetime) || m_startdatetime.IsEqualTo(m_finishdatetime))
    {
        m_startdatetime = m_finishdatetime-wxTimeSpan::Hours(4);
    }
    for(size_t i=0; i < MAX_SCHEDULE_EXCEPTIONS; i++)
    {
        int val = 0;
        wxConfigBase::Get()->Read(wxString::Format("ScheduleException%zu/isactive",i), &m_scheduleexceptions[i].isactive, m_scheduleexceptions[i].isactive);
        wxConfigBase::Get()->Read(wxString::Format("ScheduleException%zu/day",i), &val, 0);
        m_scheduleexceptions[i].day = static_cast<wxUint16>(val);
        wxConfigBase::Get()->Read(wxString::Format("ScheduleException%zu/startHour",i), &val, 0);
        m_scheduleexceptions[i].startHour = static_cast<wxUint16>(val);
        wxConfigBase::Get()->Read(wxString::Format("ScheduleException%zu/startMinute",i), &val, 0);
        m_scheduleexceptions[i].startMinute = static_cast<wxUint16>(val);
        wxConfigBase::Get()->Read(wxString::Format("ScheduleException%zu/finishHour",i), &val, 0);
        m_scheduleexceptions[i].finishHour = static_cast<wxUint16>(val);
        wxConfigBase::Get()->Read(wxString::Format("ScheduleException%zu/finishMinute",i), &val, 0);
        m_scheduleexceptions[i].finishMinute = static_cast<wxUint16>(val);
    }
    //Bandwidth
    wxConfigBase::Get()->Read("Bandwidth/bandwidthoption", &m_bandwidthoption, m_bandwidthoption);
    m_livebandwidthoption = m_bandwidthoption;
    wxConfigBase::Get()->Read("Bandwidth/bandwidth", &m_bandwidth, m_bandwidth);
    //Proxy
    wxConfigBase::Get()->Read("Proxy/proxy", &m_proxy, m_proxy);
    wxConfigBase::Get()->Read("Proxy/proxy_server", &m_proxyServer, m_proxyServer);
    wxConfigBase::Get()->Read("Proxy/proxy_port", &m_proxyPort, m_proxyPort);
    wxConfigBase::Get()->Read("Proxy/proxy_username", &m_proxyUsername, m_proxyUsername);
    wxConfigBase::Get()->Read("Proxy/proxy_authstring", &m_proxyAuthstring, m_proxyAuthstring);
    //Progress
    m_finishedpiece.Set(wxConfigBase::Get()->Read("Progress/finishedpiece", m_finishedpiece.GetAsString()));
    m_unfinishedpiece.Set(wxConfigBase::Get()->Read("Progress/unfinishedpiece", m_unfinishedpiece.GetAsString()));
    //NewDialog
    wxConfigBase::Get()->Read("Newdialog/lastx", &m_lastx, m_lastx);
    wxConfigBase::Get()->Read("Newdialog/lasty", &m_lasty, m_lasty);
    wxConfigBase::Get()->Read("Newdialog/lastnumberofconnections", &m_lastnumberofconnections, m_lastnumberofconnections);
    wxConfigBase::Get()->Read("Newdialog/laststartoption", &m_laststartoption, m_laststartoption);
    wxConfigBase::Get()->Read("Newdialog/lastdestination", &m_lastdestination, m_lastdestination);
    wxConfigBase::Get()->Read("Newdialog/lastcommand", &m_lastcommand, m_lastcommand);
    wxConfigBase::Get()->Read("Newdialog/lastontopoption", &m_lastontopoption, m_lastontopoption);
    wxConfigBase::Get()->Read("Newdialog/lastbandwidth", &m_lastbandwidth, m_lastbandwidth);
}

void mOptions::save()
{
    //GUI
    wxConfigBase::Get()->Write("GUI/x", m_x);
    wxConfigBase::Get()->Write("GUI/y", m_y);
    wxConfigBase::Get()->Write("GUI/w", m_w);
    wxConfigBase::Get()->Write("GUI/h", m_h);
    wxConfigBase::Get()->Write("GUI/separator1", m_separator1);
    wxConfigBase::Get()->Write("GUI/separator2", m_separator2);
    //General
    wxConfigBase::Get()->Write("General/lastnewreleasecheck", m_lastnewreleasecheck.FormatISOCombined());
    wxConfigBase::Get()->Write("General/attempts", m_attempts);
    wxConfigBase::Get()->Write("General/closedialog", m_closedialog);
    wxConfigBase::Get()->Write("General/simultaneous", m_simultaneous);
    wxConfigBase::Get()->Write("General/attemptstime", m_attemptstime);
    wxConfigBase::Get()->Write("General/alwaysshutdown", m_alwaysshutdown);
    wxConfigBase::Get()->Write("General/alwaysdisconnect", m_alwaysdisconnect);
    wxConfigBase::Get()->Write("General/timerupdateinterval", m_timerupdateinterval);
    wxConfigBase::Get()->Write("General/restoremainframe", m_restoremainframe);
    wxConfigBase::Get()->Write("General/hidemainframe", m_hidemainframe);
    wxConfigBase::Get()->Write("General/checkforupdates", m_checkforupdates);
    wxConfigBase::Get()->Write("General/rememberboxnewoptions", m_rememberboxnewoptions);
    wxConfigBase::Get()->Write("General/shownotify", m_shownotify);
    wxConfigBase::Get()->Write("General/md5", m_md5);
    wxConfigBase::Get()->Write("General/sha1", m_sha1);
    wxConfigBase::Get()->Write("General/sha256", m_sha256);
    //Graph
    wxConfigBase::Get()->Write("Graph/graphshow", m_graphshow);
    wxConfigBase::Get()->Write("Graph/graphrefreshtime", m_graphrefreshtime);
    wxConfigBase::Get()->Write("Graph/graphscale", m_graphscale);
    wxConfigBase::Get()->Write("Graph/graphtextarea", m_graphtextarea);
    wxConfigBase::Get()->Write("Graph/graphheight", m_graphheight);
    wxConfigBase::Get()->Write("Graph/graphspeedfontsize", m_graphspeedfontsize);
    wxConfigBase::Get()->Write("Graph/graphbackcolor", m_graphbackcolor.GetAsString());
    wxConfigBase::Get()->Write("Graph/graphgridcolor", m_graphgridcolor.GetAsString());
    wxConfigBase::Get()->Write("Graph/graphlinecolor", m_graphlinecolor.GetAsString());
    wxConfigBase::Get()->Write("Graph/graphuploadcolor", m_graphuploadcolor.GetAsString());
    wxConfigBase::Get()->Write("Graph/graphlinewidth", m_graphlinewidth);
    wxConfigBase::Get()->Write("Graph/graphfontcolor", m_graphfontcolor.GetAsString());
    //Path
    wxConfigBase::Get()->Write("Path/shutdowncmd", m_shutdowncmd);
    wxConfigBase::Get()->Write("Path/disconnectcmd", m_disconnectcmd);
    wxConfigBase::Get()->Write("Path/destination", m_destination);
    wxConfigBase::Get()->Write("Path/filemanagerpath", m_filemanagerpath);
    //Schedule
    wxConfigBase::Get()->Write("Schedule/activatescheduling", m_activatescheduling);
    wxConfigBase::Get()->Write("Schedule/startdatetime", m_startdatetime.FormatISOCombined());
    wxConfigBase::Get()->Write("Schedule/finishdatetime", m_finishdatetime.FormatISOCombined());
    for(size_t i=0; i < MAX_SCHEDULE_EXCEPTIONS; i++)
    {
        wxConfigBase::Get()->Write(wxString::Format("ScheduleException%zu/isactive",i), m_scheduleexceptions[i].isactive);
        wxConfigBase::Get()->Write(wxString::Format("ScheduleException%zu/day",i), m_scheduleexceptions[i].day);
        wxConfigBase::Get()->Write(wxString::Format("ScheduleException%zu/startHour",i), m_scheduleexceptions[i].startHour);
        wxConfigBase::Get()->Write(wxString::Format("ScheduleException%zu/startMinute",i), m_scheduleexceptions[i].startMinute);
        wxConfigBase::Get()->Write(wxString::Format("ScheduleException%zu/finishHour",i), m_scheduleexceptions[i].finishHour);
        wxConfigBase::Get()->Write(wxString::Format("ScheduleException%zu/finishMinute",i), m_scheduleexceptions[i].finishMinute);
    }
    //Bandwidth
    wxConfigBase::Get()->Write("Bandwidth/bandwidthoption", m_bandwidthoption);
    wxConfigBase::Get()->Write("Bandwidth/bandwidth", m_bandwidth);
    //Proxy
    wxConfigBase::Get()->Write("Proxy/proxy", m_proxy);
    wxConfigBase::Get()->Write("Proxy/proxy_server", m_proxyServer);
    wxConfigBase::Get()->Write("Proxy/proxy_port", m_proxyPort);
    wxConfigBase::Get()->Write("Proxy/proxy_username", m_proxyUsername);
    wxConfigBase::Get()->Write("Proxy/proxy_authstring", m_proxyAuthstring);
    //Progress
    wxConfigBase::Get()->Write("Progress/finishedpiece", m_finishedpiece.GetAsString());
    wxConfigBase::Get()->Write("Progress/unfinishedpiece", m_unfinishedpiece.GetAsString());
    //NewDialog
    wxConfigBase::Get()->Write("Newdialog/lastx", m_lastx);
    wxConfigBase::Get()->Write("Newdialog/lasty", m_lasty);
    wxConfigBase::Get()->Write("Newdialog/lastnumberofconnections", m_lastnumberofconnections);
    wxConfigBase::Get()->Write("Newdialog/laststartoption", m_laststartoption);
    wxConfigBase::Get()->Write("Newdialog/lastdestination", m_lastdestination);
    wxConfigBase::Get()->Write("Newdialog/lastcommand", m_lastcommand);
    wxConfigBase::Get()->Write("Newdialog/lastontopoption", m_lastontopoption);
    wxConfigBase::Get()->Write("Newdialog/lastbandwidth", m_lastbandwidth);
    wxConfigBase::Get()->Flush();
}

wxDateTime mOptions::lastnewreleasecheck() const
{
    return m_lastnewreleasecheck;
}

void mOptions::setLastnewreleasecheck(const wxDateTime &lastnewreleasecheck)
{
    m_lastnewreleasecheck = lastnewreleasecheck;
}

int mOptions::attempts() const
{
    return m_attempts;
}

void mOptions::setAttempts(int attempts)
{
    m_attempts = attempts;
}

bool mOptions::closedialog() const
{
    return m_closedialog;
}

void mOptions::setClosedialog(bool closedialog)
{
    m_closedialog = closedialog;
}

int mOptions::simultaneous() const
{
    return m_simultaneous;
}

void mOptions::setSimultaneous(int simultaneous)
{
    m_simultaneous = simultaneous;
}

int mOptions::attemptstime() const
{
    return m_attemptstime;
}

void mOptions::setAttemptstime(int attemptstime)
{
    m_attemptstime = attemptstime;
}

bool mOptions::shutdown() const
{
    return m_shutdown;
}

void mOptions::setShutdown(bool shutdown)
{
    m_shutdown = shutdown;
}

bool mOptions::disconnect() const
{
    return m_disconnect;
}

void mOptions::setDisconnect(bool disconnect)
{
    m_disconnect = disconnect;
}

bool mOptions::alwaysshutdown() const
{
    return m_alwaysshutdown;
}

void mOptions::setAlwaysshutdown(bool alwaysshutdown)
{
    m_alwaysshutdown = alwaysshutdown;
}

bool mOptions::alwaysdisconnect() const
{
    return m_alwaysdisconnect;
}

void mOptions::setAlwaysdisconnect(bool alwaysdisconnect)
{
    m_alwaysdisconnect = alwaysdisconnect;
}

int mOptions::timerupdateinterval() const
{
    return m_timerupdateinterval;
}

void mOptions::setTimerupdateinterval(int timerupdateinterval)
{
    m_timerupdateinterval = timerupdateinterval;
}

bool mOptions::restoremainframe() const
{
    return m_restoremainframe;
}

void mOptions::setRestoremainframe(bool restoremainframe)
{
    m_restoremainframe = restoremainframe;
}

bool mOptions::hidemainframe() const
{
    return m_hidemainframe;
}

void mOptions::setHidemainframe(bool hidemainframe)
{
    m_hidemainframe = hidemainframe;
}

bool mOptions::checkforupdates() const
{
    return m_checkforupdates;
}

void mOptions::setCheckforupdates(bool checkforupdates)
{
    m_checkforupdates = checkforupdates;
}

bool mOptions::graphshow() const
{
    return m_graphshow;
}

void mOptions::setGraphshow(bool graphshow)
{
    m_graphshow = graphshow;
}

int mOptions::graphrefreshtime() const
{
    return m_graphrefreshtime;
}

void mOptions::setGraphrefreshtime(int graphrefreshtime)
{
    m_graphrefreshtime = graphrefreshtime;
}

int mOptions::graphscale() const
{
    return m_graphscale;
}

void mOptions::setGraphscale(int graphscale)
{
    m_graphscale = graphscale;
}

int mOptions::graphtextarea() const
{
    return m_graphtextarea;
}

void mOptions::setGraphtextarea(int graphtextarea)
{
    m_graphtextarea = graphtextarea;
}

int mOptions::graphheight() const
{
    return m_graphheight;
}

void mOptions::setGraphheight(int graphheight)
{
    m_graphheight = graphheight;
}

int mOptions::graphspeedfontsize() const
{
    return m_graphspeedfontsize;
}

void mOptions::setGraphspeedfontsize(int graphspeedfontsize)
{
    m_graphspeedfontsize = graphspeedfontsize;
}

wxColour mOptions::graphbackcolor() const
{
    return m_graphbackcolor;
}

void mOptions::setGraphbackcolor(const wxColour &graphbackcolor)
{
    m_graphbackcolor = graphbackcolor;
}

wxColour mOptions::graphgridcolor() const
{
    return m_graphgridcolor;
}

void mOptions::setGraphgridcolor(const wxColour &graphgridcolor)
{
    m_graphgridcolor = graphgridcolor;
}

wxColour mOptions::graphlinecolor() const
{
    return m_graphlinecolor;
}

void mOptions::setGraphlinecolor(const wxColour &graphlinecolor)
{
    m_graphlinecolor = graphlinecolor;
}

wxColour mOptions::graphuploadcolor() const
{
    return m_graphuploadcolor;
}

void mOptions::setGraphuploadcolor(const wxColour &graphuploadcolor)
{
    m_graphuploadcolor = graphuploadcolor;
}

int mOptions::graphlinewidth() const
{
    return m_graphlinewidth;
}

void mOptions::setGraphlinewidth(int graphlinewidth)
{
    m_graphlinewidth = graphlinewidth;
}

wxColour mOptions::graphfontcolor() const
{
    return m_graphfontcolor;
}

void mOptions::setGraphfontcolor(const wxColour &graphfontcolor)
{
    m_graphfontcolor = graphfontcolor;
}

wxString mOptions::shutdowncmd() const
{
    return m_shutdowncmd;
}

void mOptions::setShutdowncmd(const wxString &shutdowncmd)
{
    m_shutdowncmd = shutdowncmd;
}

wxString mOptions::disconnectcmd() const
{
    return m_disconnectcmd;
}

void mOptions::setDisconnectcmd(const wxString &disconnectcmd)
{
    m_disconnectcmd = disconnectcmd;
}

wxString mOptions::destination() const
{
    return m_destination;
}

void mOptions::setDestination(const wxString &destination)
{
    m_destination = destination;
}

wxString mOptions::filemanagerpath() const
{
    return m_filemanagerpath;
}

void mOptions::setFilemanagerpath(const wxString &filemanagerpath)
{
    m_filemanagerpath = filemanagerpath;
}

bool mOptions::activatescheduling() const
{
    return m_activatescheduling;
}

void mOptions::setActivatescheduling(bool activatescheduling)
{
    m_activatescheduling = activatescheduling;
}

wxDateTime mOptions::startdatetime() const
{
    return m_startdatetime;
}

void mOptions::setStartdatetime(const wxDateTime &startdatetime)
{
    m_startdatetime = startdatetime;
}

wxDateTime mOptions::finishdatetime() const
{
    return m_finishdatetime;
}

void mOptions::setFinishdatetime(const wxDateTime &finishdatetime)
{
    m_finishdatetime = finishdatetime;
}

int mOptions::bandwidthoption() const
{
    return m_bandwidthoption;
}

void mOptions::setBandwidthoption(int bandwidthoption)
{
    m_bandwidthoption = bandwidthoption;
}

long mOptions::bandwidth() const
{
    return m_bandwidth;
}

void mOptions::setBandwidth(long bandwidth)
{
    m_bandwidth = bandwidth;
}

bool mOptions::proxy() const
{
    return m_proxy;
}

void mOptions::setProxy(bool proxy)
{
    m_proxy = proxy;
}

wxString mOptions::proxyServer() const
{
    return m_proxyServer;
}

void mOptions::setProxyServer(const wxString &proxyServer)
{
    m_proxyServer = proxyServer;
}

wxString mOptions::proxyPort() const
{
    return m_proxyPort;
}

void mOptions::setProxyPort(const wxString &proxyPort)
{
    m_proxyPort = proxyPort;
}

wxString mOptions::proxyUsername() const
{
    return m_proxyUsername;
}

void mOptions::setProxyUsername(const wxString &proxyUsername)
{
    m_proxyUsername = proxyUsername;
}

wxString mOptions::proxyAuthstring() const
{
    return m_proxyAuthstring;
}

void mOptions::setProxyAuthstring(const wxString &proxyAuthstring)
{
    m_proxyAuthstring = proxyAuthstring;
}

bool mOptions::rememberboxnewoptions() const
{
    return m_rememberboxnewoptions;
}

void mOptions::setRememberboxnewoptions(bool rememberboxnewoptions)
{
    m_rememberboxnewoptions = rememberboxnewoptions;
}

int mOptions::x() const
{
    return m_x;
}

void mOptions::setX(int x)
{
    m_x = x;
}

int mOptions::y() const
{
    return m_y;
}

void mOptions::setY(int y)
{
    m_y = y;
}

int mOptions::w() const
{
    return m_w;
}

void mOptions::setW(int w)
{
    m_w = w;
}

int mOptions::h() const
{
    return m_h;
}

void mOptions::setH(int h)
{
    m_h = h;
}

mScheduleException mOptions::scheduleException(size_t index) const
{
    for(size_t i=0; i < MAX_SCHEDULE_EXCEPTIONS; i++)
    {
        if(i == index)
        {
            return m_scheduleexceptions[i];
        }
    }
    mScheduleException ex;
    ex.isactive = false;
    return ex;
}

void mOptions::setScheduleException(size_t index, const mScheduleException &exception)
{
    for(size_t i=0; i < MAX_SCHEDULE_EXCEPTIONS; i++)
    {
        if(i == index)
        {
            m_scheduleexceptions[i].day = exception.day;
            m_scheduleexceptions[i].finishHour = exception.finishHour;
            m_scheduleexceptions[i].finishMinute = exception.finishMinute;
            m_scheduleexceptions[i].isactive = exception.isactive;
            m_scheduleexceptions[i].startHour = exception.startHour;
            m_scheduleexceptions[i].startMinute = exception.startMinute;
        }
    }
}

void mOptions::clearScheduleExceptions()
{
    for(size_t i=0; i < MAX_SCHEDULE_EXCEPTIONS; i++)
    {
        m_scheduleexceptions[i].isactive = false;
    }
}

bool mOptions::IsTimeForSchedule()
{
    bool time = false;
    if(m_activatescheduling)
    {
        wxDateTime now = wxDateTime::Now();
        if(now > m_startdatetime && now < moptions.finishdatetime())
        {
            time = true;
            for(size_t j=0; j < MAX_SCHEDULE_EXCEPTIONS; j++)
            {
                if(m_scheduleexceptions[j].isactive && now.GetWeekDay() == m_scheduleexceptions[j].day)
                {
                    wxDateTime exceptionstart(m_scheduleexceptions[j].startHour, m_scheduleexceptions[j].startMinute);
                    wxDateTime exceptionend(m_scheduleexceptions[j].finishHour, m_scheduleexceptions[j].finishMinute);
                    if(now > exceptionstart && now < exceptionend)
                    {
                        time = false;
                        break;
                    }
                }
            }
        }
    }
    return time;
}

wxColour mOptions::finishedpiece() const
{
    return m_finishedpiece;
}

void mOptions::setFinishedpiece(const wxColour &finishedpiece)
{
    m_finishedpiece = finishedpiece;
}

wxColour mOptions::unfinishedpiece() const
{
    return m_unfinishedpiece;
}

void mOptions::setUnfinishedpiece(const wxColour &unfinishedpiece)
{
    m_unfinishedpiece = unfinishedpiece;
}

int mOptions::lastx() const
{
    return m_lastx;
}

void mOptions::setLastx(int lastx)
{
    m_lastx = lastx;
}

int mOptions::lasty() const
{
    return m_lasty;
}

void mOptions::setLasty(int lasty)
{
    m_lasty = lasty;
}

int mOptions::lastnumberofconnections() const
{
    return m_lastnumberofconnections;
}

void mOptions::setLastnumberofconnections(int lastnumberofconnections)
{
    m_lastnumberofconnections = lastnumberofconnections;
}

int mOptions::laststartoption() const
{
    return m_laststartoption;
}

void mOptions::setLaststartoption(int laststartoption)
{
    m_laststartoption = laststartoption;
}

wxString mOptions::lastdestination() const
{
    return m_lastdestination;
}

void mOptions::setLastdestination(const wxString &lastdestination)
{
    m_lastdestination = lastdestination;
}

wxString mOptions::lastcommand() const
{
    return m_lastcommand;
}

void mOptions::setLastcommand(const wxString &lastcommand)
{
    m_lastcommand = lastcommand;
}

bool mOptions::lastontopoption() const
{
    return m_lastontopoption;
}

void mOptions::setLastontopoption(bool lastontopoption)
{
    m_lastontopoption = lastontopoption;
}

int mOptions::lastbandwidth() const
{
    return m_lastbandwidth;
}

void mOptions::setLastbandwidth(int lastbandwidth)
{
    m_lastbandwidth = lastbandwidth;
}

bool mOptions::shownotify() const
{
    return m_shownotify;
}

void mOptions::setShownotify(bool newShownotify)
{
    m_shownotify = newShownotify;
}

int mOptions::livebandwidthoption() const
{
    return m_livebandwidthoption;
}

void mOptions::setLivebandwidthoption(int newLivebandwidthoption)
{
    m_livebandwidthoption = newLivebandwidthoption;
}

int mOptions::separator1() const
{
    return m_separator1;
}

void mOptions::setSeparator1(int newSeparator1)
{
    m_separator1 = newSeparator1;
}

int mOptions::separator2() const
{
    return m_separator2;
}

void mOptions::setSeparator2(int newSeparator2)
{
    m_separator2 = newSeparator2;
}

bool mOptions::md5() const
{
    return m_md5;
}

void mOptions::setMd5(bool newMd5)
{
    m_md5 = newMd5;
}

bool mOptions::sha1() const
{
    return m_sha1;
}

void mOptions::setSha1(bool newSha1)
{
    m_sha1 = newSha1;
}

bool mOptions::sha256() const
{
    return m_sha256;
}

void mOptions::setSha256(bool newSha256)
{
    m_sha256 = newSha256;
}

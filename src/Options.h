/***************************************************************
 * Name:      Options.h
 * Author:    Max Magalhães Velasques (maxvelasques@gmail.com)
 *            David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2006
 * License:   GPL2
 **************************************************************/

#ifndef MOPTIONS_H
#define MOPTIONS_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include "Defs.h"
#include "ScheduleException.h"

#define moptions mOptions::instance()

class mOptions
{
public:
    mOptions();
    ~mOptions();

    static mOptions &instance();
    void load(const wxString &path="");
    void save();
    wxDateTime lastnewreleasecheck() const;
    void setLastnewreleasecheck(const wxDateTime &lastnewreleasecheck);
    int attempts() const;
    void setAttempts(int attempts);
    bool closedialog() const;
    void setClosedialog(bool closedialog);
    int simultaneous() const;
    void setSimultaneous(int simultaneous);
    int attemptstime() const;
    void setAttemptstime(int attemptstime);
    bool shutdown() const;
    void setShutdown(bool shutdown);
    bool disconnect() const;
    void setDisconnect(bool disconnect);
    bool alwaysshutdown() const;
    void setAlwaysshutdown(bool alwaysshutdown);
    bool alwaysdisconnect() const;
    void setAlwaysdisconnect(bool alwaysdisconnect);
    int timerupdateinterval() const;
    void setTimerupdateinterval(int timerupdateinterval);
    bool restoremainframe() const;
    void setRestoremainframe(bool restoremainframe);
    bool hidemainframe() const;
    void setHidemainframe(bool hidemainframe);
    bool checkforupdates() const;
    void setCheckforupdates(bool checkforupdates);
    bool graphshow() const;
    void setGraphshow(bool graphshow);
    int graphhowmanyvalues() const;
    void setGraphhowmanyvalues(int graphhowmanyvalues);
    int graphrefreshtime() const;
    void setGraphrefreshtime(int graphrefreshtime);
    int graphscale() const;
    void setGraphscale(int graphscale);
    int graphtextarea() const;
    void setGraphtextarea(int graphtextarea);
    int graphheight() const;
    void setGraphheight(int graphheight);
    int graphspeedfontsize() const;
    void setGraphspeedfontsize(int graphspeedfontsize);
    wxColour graphbackcolor() const;
    void setGraphbackcolor(const wxColour &graphbackcolor);
    wxColour graphgridcolor() const;
    void setGraphgridcolor(const wxColour &graphgridcolor);
    wxColour graphlinecolor() const;
    void setGraphlinecolor(const wxColour &graphlinecolor);
    wxColour graphuploadcolor() const;
    void setGraphuploadcolor(const wxColour &graphuploadcolor);
    int graphlinewidth() const;
    void setGraphlinewidth(int graphlinewidth);
    wxColour graphfontcolor() const;
    void setGraphfontcolor(const wxColour &graphfontcolor);
    wxString shutdowncmd() const;
    void setShutdowncmd(const wxString &shutdowncmd);
    wxString disconnectcmd() const;
    void setDisconnectcmd(const wxString &disconnectcmd);
    wxString destination() const;
    void setDestination(const wxString &destination);
    wxString filemanagerpath() const;
    void setFilemanagerpath(const wxString &filemanagerpath);
    bool activatescheduling() const;
    void setActivatescheduling(bool activatescheduling);
    wxDateTime startdatetime() const;
    void setStartdatetime(const wxDateTime &startdatetime);
    wxDateTime finishdatetime() const;
    void setFinishdatetime(const wxDateTime &finishdatetime);
    int bandwidthoption() const;
    void setBandwidthoption(int bandwidthoption);
    long bandwidth() const;
    void setBandwidth(long bandwidth);
    bool proxy() const;
    void setProxy(bool proxy);
    wxString proxyServer() const;
    void setProxyServer(const wxString &proxyServer);
    wxString proxyPort() const;
    void setProxyPort(const wxString &proxyPort);
    wxString proxyUsername() const;
    void setProxyUsername(const wxString &proxyUsername);
    wxString proxyAuthstring() const;
    void setProxyAuthstring(const wxString &proxyAuthstring);
    bool rememberboxnewoptions() const;
    void setRememberboxnewoptions(bool rememberboxnewoptions);
    int x() const;
    void setX(int x);
    int y() const;
    void setY(int y);
    int w() const;
    void setW(int w);
    int h() const;
    void setH(int h);
    mScheduleException scheduleException(size_t index) const;
    void setScheduleException(size_t index, const mScheduleException &exception);
    void clearScheduleExceptions();
    bool IsTimeForSchedule();
    wxColour finishedpiece() const;
    void setFinishedpiece(const wxColour &finishedpiece);
    wxColour unfinishedpiece() const;
    void setUnfinishedpiece(const wxColour &unfinishedpiece);
    int lastx() const;
    void setLastx(int lastx);
    int lasty() const;
    void setLasty(int lasty);
    int lastnumberofconnections() const;
    void setLastnumberofconnections(int lastnumberofconnections);
    int laststartoption() const;
    void setLaststartoption(int laststartoption);
    wxString lastdestination() const;
    void setLastdestination(const wxString &lastdestination);
    wxString lastcommand() const;
    void setLastcommand(const wxString &lastcommand);
    bool lastontopoption() const;
    void setLastontopoption(bool lastontopoption);
    int lastbandwidth() const;
    void setLastbandwidth(int lastbandwidth);
    bool shownotify() const;
    void setShownotify(bool newShownotify);
    int livebandwidthoption() const;
    void setLivebandwidthoption(int newLivebandwidthoption);
    int separator1() const;
    void setSeparator1(int newSeparator1);
    int separator2() const;
    void setSeparator2(int newSeparator2);

private:
    //GUI
    int m_x;
    int m_y;
    int m_w;
    int m_h;
    int m_separator1;
    int m_separator2;
    //General
    wxDateTime m_lastnewreleasecheck;
    int m_attempts;             //number of attempts
    bool m_closedialog;         //show the close dialog
    int m_simultaneous;         //number of simultaneous downloads
    int m_attemptstime;         //time between the attempts in seconds
    bool m_shutdown;
    bool m_disconnect;
    bool m_alwaysshutdown;
    bool m_alwaysdisconnect;
    int m_timerupdateinterval; //time between the timer refreshs in milliseconds
    bool m_restoremainframe;   //Restore the mainframe when all downloads are finished
    bool m_hidemainframe;      //Hide the mainframe when the user start a download
    bool m_checkforupdates;
    bool m_rememberboxnewoptions;
    bool m_shownotify;
    //Graph
    bool m_graphshow;
    int m_graphrefreshtime;    //time between the graph refreshs in milliseconds
    int m_graphscale;          //max value showed in the graph
    int m_graphtextarea;       //size the area reserved for the speed value
    int m_graphheight;
    int m_graphspeedfontsize;
    wxColour m_graphbackcolor;
    wxColour m_graphgridcolor;
    wxColour m_graphlinecolor;
    wxColour m_graphuploadcolor;
    int m_graphlinewidth;
    wxColour m_graphfontcolor;
    //Path
    wxString m_shutdowncmd;
    wxString m_disconnectcmd;
    wxString m_destination;
    wxString m_filemanagerpath;
    //Schedule
    bool m_activatescheduling;
    wxDateTime m_startdatetime;
    wxDateTime m_finishdatetime;
    mScheduleException m_scheduleexceptions[MAX_SCHEDULE_EXCEPTIONS];
    //Bandwidth
    int m_bandwidthoption;
    int m_livebandwidthoption;
    long m_bandwidth;
    //Proxy
    bool m_proxy;
    wxString m_proxyServer;
    wxString m_proxyPort;
    wxString m_proxyUsername;
    wxString m_proxyAuthstring;
    //Progress
    wxColour m_finishedpiece;
    wxColour m_unfinishedpiece;
    //NewDialog
    int m_lastx;
    int m_lasty;
    int m_lastnumberofconnections;
    int m_laststartoption;
    wxString m_lastdestination;
    wxString m_lastcommand;
    bool m_lastontopoption;
    int m_lastbandwidth;
};

#endif // MOPTIONS_H

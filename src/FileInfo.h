/***************************************************************
 * Name:      FileInfo.h
 * Author:    Max Magalhães Velasques (maxvelasques@gmail.com)
 *            David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2006
 * License:   GPL2
 **************************************************************/

#ifndef MFILEINFO_H
#define MFILEINFO_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/xml/xml.h>
#include <aria2/aria2.h>

class mFileInfo
{
public:
    mFileInfo();

    wxXmlNode *createXmlNode();
    bool fromXmlNode(const wxXmlNode* node);
    int index() const;
    void setIndex(int index);
    aria2::A2Gid gid() const;
    void setGid(const aria2::A2Gid &gid);
    int status() const;
    void setStatus(int status);
    void setStatusFromAriastatus(aria2::DownloadStatus status);
    wxString name() const;
    void setName(const wxString &name);
    wxString destination() const;
    void setDestination(const wxString &destination);
    wxArrayString urls() const;
    void setUrls(const wxArrayString &urls);
    void addUrl(const wxString &url);
    wxString user() const;
    void setUser(const wxString &user);
    wxString password() const;
    void setPassword(const wxString &password);
    wxString MD5() const;
    void setMD5(const wxString &MD5);
    int connections() const;
    void setConnections(int connections);
    int bandwidth() const;
    void setBandwidth(int bandwidth);
    wxString comment() const;
    void setComment(const wxString &comment);
    wxString link() const;
    void setLink(const wxString &link);
    wxString command() const;
    void setCommand(const wxString &command);
    int64_t totalLength() const;
    void setTotalLength(const int64_t &totalLength);
    int64_t downloadLength() const;
    void setDownloadLength(const int64_t &downloadLength);
    int64_t uploadLength() const;
    void setUploadLength(const int64_t &uploadLength);
    std::string bitfield() const;
    void setBitfield(const std::string &bitfield);
    int downloadSpeed() const;
    void setDownloadSpeed(int downloadSpeed);
    int uploadSpeed() const;
    void setUploadSpeed(int uploadSpeed);
    aria2::A2Gid parent() const;
    void setParent(const aria2::A2Gid &parent);
    int numPieces() const;
    void setNumPieces(int numPieces);
    int numConnections() const;
    void setNumConnections(int numConnections);
    wxDateTime startTime() const;
    void setStartTime(const wxDateTime &startTime);
    wxDateTime endTime() const;
    void setEndTime(const wxDateTime &endTime);
    uint64_t timepassed() const;
    void addTimepassed(int milliseconds);
    void clearTimepassed();
    wxString log() const;
    void appendLogLine(const wxString &line);
    void clearLog();
    int attempts() const;
    void setAttempts(int newAttempts);
    void addAttempt();
    uint64_t laststat() const;
    void setLaststat(const uint64_t &newLaststat);

private:
    int m_index;
    aria2::A2Gid m_gid;
    int m_status;
    wxString m_name;
    wxString m_destination;
    wxArrayString m_urls;
    wxString m_user;
    wxString m_password;
    wxString m_MD5;
    int m_connections;
    int m_bandwidth;
    wxString m_comment;
    wxString m_link;
    wxString m_command;
    int64_t m_totalLength;
    int64_t m_downloadLength;
    int64_t m_uploadLength;
    std::string m_bitfield;
    int m_downloadSpeed;
    int m_uploadSpeed;
    aria2::A2Gid m_parent;
    int m_numPieces;
    int m_numConnections;
    wxDateTime m_startTime;
    wxDateTime m_endTime;
    uint64_t m_timepassed;
    wxString m_log;
    int m_attempts;
    uint64_t m_laststat;
    //NOTE torrent values - next release
};

#endif // MFILEINFO_H

/***************************************************************
 * Name:      FileInfo.cpp
 * Author:    Max Magalhães Velasques (maxvelasques@gmail.com)
 *            David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2006
 * License:   GPL2
 **************************************************************/

#include "FileInfo.h"
#include "UtilFunctions.h"
#include "Options.h"

mFileInfo::mFileInfo()
{
    m_gid = 0;
    m_name = wxEmptyString;
    m_totalLength = 0;
    m_downloadLength = 0;
    m_uploadLength = 0;
    m_downloadSpeed = 0;
    m_uploadSpeed = 0;
    m_numPieces = 0;
    m_timepassed = 0;
    m_status = STATUS_QUEUE;
    m_destination = moptions.destination();
    m_index = -1;
    m_user = wxEmptyString;
    m_password = wxEmptyString;
    m_MD5 = wxEmptyString;
    m_SHA1 = wxEmptyString;
    m_SHA256 = wxEmptyString;
    m_connections = 0;
    m_bandwidth = 0;
    m_comment = wxEmptyString;
    m_link = wxEmptyString;
    m_command = wxEmptyString;
    m_bitfield = "";
    m_parent = 0;
    m_numConnections = 0;
    m_startTime = wxDateTime::Now();
    m_endTime = wxDateTime::Now();
    m_attempts = 0;
    m_laststat = 0;
}

wxXmlNode *mFileInfo::createXmlNode()
{
    wxXmlNode *node = new wxXmlNode(wxXML_ELEMENT_NODE, "File");
    node->AddAttribute("index", wxString::Format("%d",m_index));
    node->AddAttribute("gid", MyUtilFunctions::FromStdString(aria2::gidToHex(m_gid)));
    node->AddAttribute("status", wxString::Format("%d",m_status));
    node->AddAttribute("name", m_name);
    node->AddAttribute("destination", m_destination);
    wxXmlNode *urls = new wxXmlNode(wxXML_ELEMENT_NODE, "URLs");
    for(size_t i=0; i < m_urls.GetCount(); i++)
    {
        wxXmlNode *url = new wxXmlNode(wxXML_ELEMENT_NODE, "URL");
        url->AddAttribute("value", m_urls[i]);
        urls->AddChild(url);
    }
    node->AddChild(urls);
    node->AddAttribute("user", m_user);
    node->AddAttribute("password", m_password);
    node->AddAttribute("md5", m_MD5);
    node->AddAttribute("sha1", m_SHA1);
    node->AddAttribute("sha256", m_SHA256);
    node->AddAttribute("connections", wxString::Format("%d",m_connections));
    node->AddAttribute("bandwith", wxString::Format("%d",m_bandwidth));
    node->AddAttribute("comment", m_comment);
    node->AddAttribute("link", m_link);
    node->AddAttribute("command", m_command);
    node->AddAttribute("totalLength", wxString::Format("%lld",m_totalLength));
    node->AddAttribute("startTime", m_startTime.FormatISOCombined());
    node->AddAttribute("endTime", m_endTime.FormatISOCombined());
    node->AddAttribute("timepassed", wxString::Format("%llu",m_timepassed));
    node->AddAttribute("log", m_log);
    node->AddAttribute("attempts", wxString::Format("%d",m_attempts));
    return node;
}

bool mFileInfo::fromXmlNode(const wxXmlNode *node)
{
    if(!node) return false;
    if(node->GetName() != "File") return false;
    m_index = wxAtoi(node->GetAttribute("index","-1"));
    m_gid = aria2::hexToGid(MyUtilFunctions::ToStdString(node->GetAttribute("gid","0000000000000000")));
    m_status = wxAtoi(node->GetAttribute("status",wxString::Format("%d",STATUS_ERROR)));
    if(m_status < STATUS_STOPED || m_status > STATUS_SCHEDULE_ACTIVE)
    {
        m_status = STATUS_ERROR;
    }
    m_name = node->GetAttribute("name");
    m_destination = node->GetAttribute("destination",moptions.destination());
    m_urls.clear();
    wxXmlNode *urlsNode = node->GetChildren();
    wxXmlNode *urlNode = urlsNode->GetChildren();
    while(urlNode)
    {
        m_urls.push_back(urlNode->GetAttribute("value"));
        urlNode = urlNode->GetNext();
    }
    m_user = node->GetAttribute("user");
    m_password = node->GetAttribute("password");
    m_MD5 = node->GetAttribute("md5");
    m_SHA1 = node->GetAttribute("sha1");
    m_SHA256 = node->GetAttribute("sha256");
    m_connections = wxAtoi(node->GetAttribute("connections", "1"));
    if(m_connections < 1 || m_connections > 16)
    {
        m_connections = 1;
    }
    m_bandwidth = wxAtoi(node->GetAttribute("bandwith", "0"));
    m_comment = node->GetAttribute("comment");
    m_link = node->GetAttribute("link");
    m_command = node->GetAttribute("command");
    m_totalLength = static_cast<int64_t>(wxAtof(node->GetAttribute("totalLength", "0.0")));
    m_startTime.ParseISOCombined(node->GetAttribute("startTime", wxDateTime::Now().FormatISOCombined()));
    m_endTime.ParseISOCombined(node->GetAttribute("endTime", wxDateTime::Now().FormatISOCombined()));
    m_timepassed = static_cast<uint64_t>(wxAtof(node->GetAttribute("timepassed", "0.0")));
    m_log = node->GetAttribute("log");
    m_attempts = wxAtoi(node->GetAttribute("attempts","0"));
    return m_index != -1;
}

int mFileInfo::index() const
{
    return m_index;
}

void mFileInfo::setIndex(int index)
{
    m_index = index;
}

aria2::A2Gid mFileInfo::gid() const
{
    return m_gid;
}

void mFileInfo::setGid(const aria2::A2Gid &gid)
{
    m_gid = gid;
}

int mFileInfo::status() const
{
    return m_status;
}

void mFileInfo::setStatus(int status)
{
    m_status = status;
}

void mFileInfo::setStatusFromAriastatus(aria2::DownloadStatus status)
{
    if(m_status == STATUS_SCHEDULE_ACTIVE || m_status == STATUS_SCHEDULE_QUEUE)
    {
        if(status == aria2::DOWNLOAD_ACTIVE) m_status = STATUS_SCHEDULE_ACTIVE;
        if(status == aria2::DOWNLOAD_COMPLETE) m_status = STATUS_FINISHED;
        if(status == aria2::DOWNLOAD_ERROR) m_status = STATUS_ERROR;
        return;
    }
    switch(status) {
    case aria2::DOWNLOAD_ACTIVE: m_status = STATUS_ACTIVE; break;
    case aria2::DOWNLOAD_WAITING: m_status = STATUS_STOPED; break;
    case aria2::DOWNLOAD_PAUSED: m_status = STATUS_QUEUE; break;
    case aria2::DOWNLOAD_COMPLETE: m_status = STATUS_FINISHED; break;
    case aria2::DOWNLOAD_ERROR: m_status = STATUS_ERROR; break;
    default: m_status = STATUS_STOPED;
    }
}

wxString mFileInfo::name() const
{
    return m_name;
}

void mFileInfo::setName(const wxString &name)
{
    m_name = name;
}

wxString mFileInfo::destination() const
{
    return m_destination;
}

void mFileInfo::setDestination(const wxString &destination)
{
    m_destination = destination;
}

wxArrayString mFileInfo::urls() const
{
    return m_urls;
}

void mFileInfo::setUrls(const wxArrayString &urls)
{
    m_urls = urls;
}

void mFileInfo::addUrl(const wxString &url)
{
    m_urls.Add(url);
}

wxString mFileInfo::user() const
{
    return m_user;
}

void mFileInfo::setUser(const wxString &user)
{
    m_user = user;
}

wxString mFileInfo::password() const
{
    return m_password;
}

void mFileInfo::setPassword(const wxString &password)
{
    m_password = password;
}

wxString mFileInfo::MD5() const
{
    return m_MD5;
}

void mFileInfo::setMD5(const wxString &MD5)
{
    m_MD5 = MD5;
}

int mFileInfo::connections() const
{
    return m_connections;
}

void mFileInfo::setConnections(int connections)
{
    m_connections = connections;
}

int mFileInfo::bandwidth() const
{
    return m_bandwidth;
}

void mFileInfo::setBandwidth(int bandwidth)
{
    m_bandwidth = bandwidth;
}

wxString mFileInfo::comment() const
{
    return m_comment;
}

void mFileInfo::setComment(const wxString &comment)
{
    m_comment = comment;
}

wxString mFileInfo::link() const
{
    return m_link;
}

void mFileInfo::setLink(const wxString &link)
{
    m_link = link;
}

wxString mFileInfo::command() const
{
    return m_command;
}

void mFileInfo::setCommand(const wxString &command)
{
    m_command = command;
}

int64_t mFileInfo::totalLength() const
{
    return m_totalLength;
}

void mFileInfo::setTotalLength(const int64_t &totalLength)
{
    m_totalLength = totalLength;
}

int64_t mFileInfo::downloadLength() const
{
    return m_downloadLength;
}

void mFileInfo::setDownloadLength(const int64_t &downloadLength)
{
    m_downloadLength = downloadLength;
}

int64_t mFileInfo::uploadLength() const
{
    return m_uploadLength;
}

void mFileInfo::setUploadLength(const int64_t &uploadLength)
{
    m_uploadLength = uploadLength;
}

std::string mFileInfo::bitfield() const
{
    return m_bitfield;
}

void mFileInfo::setBitfield(const std::string &bitfield)
{
    m_bitfield = bitfield;
}

int mFileInfo::downloadSpeed() const
{
    return m_downloadSpeed;
}

void mFileInfo::setDownloadSpeed(int downloadSpeed)
{
    m_downloadSpeed = downloadSpeed;
}

int mFileInfo::uploadSpeed() const
{
    return m_uploadSpeed;
}

void mFileInfo::setUploadSpeed(int uploadSpeed)
{
    m_uploadSpeed = uploadSpeed;
}

aria2::A2Gid mFileInfo::parent() const
{
    return m_parent;
}

void mFileInfo::setParent(const aria2::A2Gid &parent)
{
    m_parent = parent;
}

int mFileInfo::numPieces() const
{
    if(!totalLength()) return 0;
    return m_numPieces;
}

void mFileInfo::setNumPieces(int numPieces)
{
    m_numPieces = numPieces;
}

int mFileInfo::numConnections() const
{
    return m_numConnections;
}

void mFileInfo::setNumConnections(int numConnections)
{
    m_numConnections = numConnections;
}

wxDateTime mFileInfo::startTime() const
{
    return m_startTime;
}

void mFileInfo::setStartTime(const wxDateTime &startTime)
{
    m_startTime = startTime;
}

wxDateTime mFileInfo::endTime() const
{
    return m_endTime;
}

void mFileInfo::setEndTime(const wxDateTime &endTime)
{
    m_endTime = endTime;
}

uint64_t mFileInfo::timepassed() const
{
    return m_timepassed;
}

void mFileInfo::addTimepassed(int milliseconds)
{
    m_timepassed += milliseconds;
}

void mFileInfo::clearTimepassed()
{
    m_timepassed = 0;
}

wxString mFileInfo::log() const
{
    return m_log;
}

void mFileInfo::appendLogLine(const wxString &line)
{
    m_log << line << "\n";
}

void mFileInfo::clearLog()
{
    m_log = "";
}

int mFileInfo::attempts() const
{
    return m_attempts;
}

void mFileInfo::setAttempts(int newAttempts)
{
    m_attempts = newAttempts;
}

void mFileInfo::addAttempt()
{
    m_attempts++;
}

uint64_t mFileInfo::laststat() const
{
    return m_laststat;
}

void mFileInfo::setLaststat(const uint64_t &newLaststat)
{
    m_laststat = newLaststat;
}

const wxString &mFileInfo::SHA1() const
{
    return m_SHA1;
}

void mFileInfo::setSHA1(const wxString &newSHA1)
{
    m_SHA1 = newSHA1;
}

const wxString &mFileInfo::SHA256() const
{
    return m_SHA256;
}

void mFileInfo::setSHA256(const wxString &newSHA256)
{
    m_SHA256 = newSHA256;
}

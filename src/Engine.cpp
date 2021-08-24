/***************************************************************
 * Name:      Engine.cpp
 * Author:    David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2021
 * License:   GPL2
 **************************************************************/

#include <wx/stdpaths.h>
#include <chrono>
#include <algorithm>
#include "Engine.h"
#include "wxDFast.h"
#include "Options.h"
#include "Defs.h"
#include "UtilFunctions.h"

mEngine::mEngine(mMainFrame *frame)
    : wxThread(wxTHREAD_DETACHED)
{
    m_frame = frame;
    m_versionGid = 0;
    m_checkversion = false;
    m_started = false;
}

mEngine::~mEngine()
{
    wxCriticalSectionLocker enter(m_frame->m_engineCS);
    m_frame->m_engine = nullptr;
}

void *mEngine::Entry()
{
    aria2::SessionConfig config;
    config.keepRunning = true;
    aria2::KeyVals options;
    //NOTE 0.70.x will not work as BT downloader/uploader
    options.push_back(std::make_pair("follow-torrent","false"));
    options.push_back(std::make_pair("max-overall-download-limit",moptions.livebandwidthoption()==2?std::to_string(moptions.bandwidth())+"K":"0"));
    if(moptions.proxy())
    {
        if(moptions.proxyPort().IsEmpty()) options.push_back(std::make_pair("all-proxy",MyUtilFunctions::ToStdString(moptions.proxyServer())));
        else options.push_back(std::make_pair("all-proxy",MyUtilFunctions::ToStdString(moptions.proxyServer()+":"+moptions.proxyPort())));
        options.push_back(std::make_pair("all-proxy-passwd",MyUtilFunctions::ToStdString(moptions.proxyAuthstring())));
        options.push_back(std::make_pair("all-proxy-user",MyUtilFunctions::ToStdString(moptions.proxyUsername())));
    }
#ifdef __WXMSW__
    options.push_back(std::make_pair("ca-certificate",MyUtilFunctions::ToStdString(wxString(WXDFAST_DATADIR)+wxFILE_SEP_PATH+"ca-certs"+wxFILE_SEP_PATH+"mozilla.pem")));
#endif
    m_session = aria2::sessionNew(options, config);
    auto graph = std::chrono::steady_clock::now();
    auto update = graph;
    while(!TestDestroy())
    {
        int rv = aria2::run(m_session, aria2::RUN_ONCE);
        if(rv != 1)
        {
            break;
        }
        if(!m_started)
        {
            m_started = true;
            wxThreadEvent event(wxEVT_THREAD, ID_ENGINE_STARTED);
            wxQueueEvent(m_frame, event.Clone());
        }
        auto now = std::chrono::steady_clock::now();
        auto count = std::chrono::duration_cast<std::chrono::milliseconds>(now - graph).count();
        if(count >= moptions.graphrefreshtime())
        {
            graph = now;
            aria2::GlobalStat st = aria2::getGlobalStat(m_session);
            GlobalEvent event(ID_ENGINE_GLOBAL, st.downloadSpeed, st.uploadSpeed, st.numActive, st.numWaiting, st.numStopped);
            wxQueueEvent(m_frame, event.Clone());
        }
        count = std::chrono::duration_cast<std::chrono::milliseconds>(now - update).count();
        if(count >= moptions.timerupdateinterval())
        {
            update = now;
            std::vector<aria2::A2Gid> gids = aria2::getActiveDownload(m_session);
            for(const auto &gid : gids)
            {
                if(gid != 0 && gid != m_versionGid && std::find(m_gids.begin(), m_gids.end(), gid) == m_gids.end())
                    m_gids.push_back(gid);
            }
            for(auto it = m_gids.begin(); it != m_gids.end();)
            {
                aria2::DownloadHandle* dh = aria2::getDownloadHandle(m_session, (*it));
                if(dh)
                {
                    if(dh->getStatus() == aria2::DOWNLOAD_REMOVED)
                    {
                        DownloadEvent event(ID_ENGINE_REMOVED, (*it), dh->getStatus(), 0, 0, 0, "", 0, 0, 0, 0);
                        wxQueueEvent(m_frame, event.Clone());
                        aria2::deleteDownloadHandle(dh);
                        it = m_gids.erase(it);
                        continue;
                    }
                    if(dh->getStatus() == aria2::DOWNLOAD_COMPLETE || dh->getStatus() == aria2::DOWNLOAD_ERROR)
                    {
                        DownloadEvent event(ID_ENGINE_DOWNLOAD, (*it), dh->getStatus(), dh->getTotalLength(), dh->getCompletedLength(), dh->getUploadLength(),
                                            dh->getBitfield(), dh->getDownloadSpeed(), dh->getUploadSpeed(), dh->getPieceLength(), dh->getNumPieces());
                        wxQueueEvent(m_frame, event.Clone());
                        aria2::deleteDownloadHandle(dh);
                        it = m_gids.erase(it);
                        continue;
                    }
                    DownloadEvent event(ID_ENGINE_DOWNLOAD, (*it), dh->getStatus(), dh->getTotalLength(), dh->getCompletedLength(), dh->getUploadLength(),
                                        dh->getBitfield(), dh->getDownloadSpeed(), dh->getUploadSpeed(), dh->getPieceLength(), dh->getNumPieces());
                    wxQueueEvent(m_frame, event.Clone());
                    aria2::deleteDownloadHandle(dh);
                }
                ++it;
            }
            if(m_checkversion && m_versionGid)
            {
                aria2::DownloadHandle* dh = aria2::getDownloadHandle(m_session, m_versionGid);
                if(dh && (dh->getStatus() == aria2::DOWNLOAD_COMPLETE || dh->getStatus() == aria2::DOWNLOAD_ERROR))
                {
                    m_versionGid = 0;
                    m_checkversion = false;
                    wxThreadEvent event(wxEVT_THREAD, ID_ENGINE_VERSION);
                    wxQueueEvent(m_frame, event.Clone());
                    aria2::deleteDownloadHandle(dh);
                }
            }
        }
    }
    aria2::shutdown(m_session, true);
    return (wxThread::ExitCode)0;
}

void mEngine::OnExit()
{
}

aria2::A2Gid mEngine::CheckVersion()
{
    m_versionGid = 0;
    std::vector<std::string> uris;
    uris.push_back("https://dxsolutions.org/wxdfast-version");
    aria2::KeyVals options;
    options.push_back(std::make_pair("dir",MyUtilFunctions::ToStdString(wxStandardPaths::Get().GetUserDataDir())));
    int rv = aria2::addUri(m_session, &m_versionGid, uris, options);
    if(rv < 0)
    {
        std::cerr << "Failed to check version" << std::endl;
    }
    else
    {
        m_checkversion = true;
    }
    return m_versionGid;
}

mFileInfo mEngine::FillInfo(aria2::A2Gid gid)
{
    mFileInfo info;
    aria2::DownloadHandle* dh = aria2::getDownloadHandle(m_session, gid);
    if(dh)
    {
        info.setGid(gid);
        info.setStatus(dh->getStatus());
        info.setName(wxFileNameFromPath(MyUtilFunctions::FromStdString(dh->getFile(1).path)));
        info.setDestination(MyUtilFunctions::FromStdString(dh->getDir()));
        info.setTotalLength(dh->getTotalLength());
        info.setParent(dh->getBelongsTo());
        std::vector<aria2::UriData> uris = dh->getFile(1).uris;
        wxArrayString urls;
        for(const auto &uri : uris)
        {
            urls.push_back(MyUtilFunctions::FromStdString(uri.uri));
        }
        info.setUrls(urls);
        info.setNumPieces(dh->getNumPieces());
        aria2::deleteDownloadHandle(dh);
    }
    return info;
}

void mEngine::AddFileInfo(const mFileInfo &info)
{
    AddGid(info.gid());
    aria2::KeyVals options;
    options.push_back(std::make_pair("dir",MyUtilFunctions::ToStdString(info.destination())));
    options.push_back(std::make_pair("split",std::to_string(info.connections())));
    options.push_back(std::make_pair("max-connection-per-server",std::to_string(info.connections())));
    options.push_back(std::make_pair("max-download-limit",moptions.livebandwidthoption()==1?std::to_string(info.bandwidth())+"K":"0"));
    options.push_back(std::make_pair("max-tries",std::to_string(moptions.attempts())));
    options.push_back(std::make_pair("retry-wait",std::to_string(moptions.attemptstime())));
    options.push_back(std::make_pair("gid",aria2::gidToHex(info.gid())));
    if(!info.user().IsEmpty())
    {
        options.push_back(std::make_pair("http-user",MyUtilFunctions::ToStdString(info.user())));
        options.push_back(std::make_pair("ftp-user",MyUtilFunctions::ToStdString(info.user())));
    }
    if(!info.password().IsEmpty())
    {
        options.push_back(std::make_pair("http-passwd",MyUtilFunctions::ToStdString(info.password())));
        options.push_back(std::make_pair("ftp-passwd",MyUtilFunctions::ToStdString(info.password())));
    }
    aria2::addUri(m_session, nullptr, MyUtilFunctions::ArrayStringToVector(info.urls()), options);
}

void mEngine::AddGid(aria2::A2Gid gid)
{
    if(std::find(m_gids.begin(), m_gids.end(), gid) != m_gids.end())
        return;
    m_gids.push_back(gid);
}

bool mEngine::HasGid(aria2::A2Gid gid)
{
    if(std::find(m_gids.begin(), m_gids.end(), gid) != m_gids.end())
        return true;
    return false;
}

bool mEngine::IsActive(aria2::A2Gid gid)
{
    std::vector<aria2::A2Gid> gids = aria2::getActiveDownload(m_session);
    if(std::find(gids.begin(), gids.end(), gid) != gids.end())
        return true;
    return false;
}

void mEngine::PauseDownload(aria2::A2Gid gid)
{
    if(aria2::isNull(gid))
        return;
    aria2::pauseDownload(m_session, gid);
}

void mEngine::ResumeDownload(aria2::A2Gid gid, const wxString &destination, int connections, int bandwidth)
{
    if(aria2::isNull(gid))
        return;
    aria2::KeyVals options;
    options.push_back(std::make_pair("dir",MyUtilFunctions::ToStdString(destination)));
    options.push_back(std::make_pair("split",std::to_string(connections)));
    options.push_back(std::make_pair("max-connection-per-server",std::to_string(connections)));
    options.push_back(std::make_pair("max-download-limit",moptions.livebandwidthoption()==1?std::to_string(bandwidth)+"K":"0"));
    options.push_back(std::make_pair("max-tries",std::to_string(moptions.attempts())));
    options.push_back(std::make_pair("retry-wait",std::to_string(moptions.attemptstime())));
    aria2::changeOption(m_session, gid, options);
    aria2::unpauseDownload(m_session, gid);
}

bool mEngine::RemoveDownload(aria2::A2Gid gid)
{
    if(aria2::isNull(gid))
        return false;
    if(aria2::removeDownload(m_session, gid) < 0)
        return false;
    return true;
}

wxString mEngine::RealFileName(aria2::A2Gid gid)
{
    wxString name = "";
    if(aria2::isNull(gid))
        return name;
    aria2::DownloadHandle* dh = aria2::getDownloadHandle(m_session, gid);
    if(dh)
    {
        name = wxFileNameFromPath(MyUtilFunctions::FromStdString(dh->getFile(1).path));
        aria2::deleteDownloadHandle(dh);
    }
    return name;
}

void mEngine::SetGlobalBandwith()
{
    if(moptions.livebandwidthoption() == 2)
    {
        aria2::KeyVals options;
        options.push_back(std::make_pair("max-overall-download-limit",std::to_string(moptions.bandwidth())+"K"));
        aria2::changeGlobalOption(m_session, options);
    }
    else
    {
        aria2::KeyVals options;
        options.push_back(std::make_pair("max-overall-download-limit","0"));
        aria2::changeGlobalOption(m_session, options);
    }
}

void mEngine::SetDownloadBandwith(aria2::A2Gid gid, int bandwidth)
{
    if(aria2::isNull(gid))
        return;
    aria2::KeyVals options;
    options.push_back(std::make_pair("max-download-limit",moptions.livebandwidthoption()==1?std::to_string(bandwidth)+"K":"0"));
    aria2::changeOption(m_session, gid, options);
}

void mEngine::UpdateOptions()
{
    if(moptions.proxy())
    {
        aria2::KeyVals options;
        if(moptions.proxyPort().IsEmpty()) options.push_back(std::make_pair("all-proxy",MyUtilFunctions::ToStdString(moptions.proxyServer())));
        else options.push_back(std::make_pair("all-proxy",MyUtilFunctions::ToStdString(moptions.proxyServer()+":"+moptions.proxyPort())));
        options.push_back(std::make_pair("all-proxy-passwd",MyUtilFunctions::ToStdString(moptions.proxyAuthstring())));
        options.push_back(std::make_pair("all-proxy-user",MyUtilFunctions::ToStdString(moptions.proxyUsername())));
        aria2::changeGlobalOption(m_session, options);
    }
    else
    {
        aria2::KeyVals options;
        options.push_back(std::make_pair("all-proxy",""));
        aria2::changeGlobalOption(m_session, options);
    }
    for(const auto &gid : m_gids)
    {
        if(!aria2::isNull(gid))
        {
            aria2::KeyVals options;
            options.push_back(std::make_pair("max-tries",std::to_string(moptions.attempts())));
            options.push_back(std::make_pair("retry-wait",std::to_string(moptions.attemptstime())));
            aria2::changeOption(m_session, gid, options);
        }
    }
}

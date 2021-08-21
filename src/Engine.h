/***************************************************************
 * Name:      Engine.h
 * Author:    David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2021
 * License:   GPL2
 **************************************************************/

#ifndef ENGINE_H
#define ENGINE_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/thread.h>
#include <wx/event.h>
#include <aria2/aria2.h>
#include "FileInfo.h"

class mMainFrame;

class GlobalEvent : public wxThreadEvent
{
public:
    GlobalEvent(int winid, int downloadSpeed, int uploadSpeed, int numActive, int numWaiting, int numStopped)
        : wxThreadEvent(wxEVT_THREAD, winid),
          m_downloadSpeed(downloadSpeed), m_uploadSpeed(uploadSpeed), m_numActive(numActive), m_numWaiting(numWaiting),
          m_numStopped(numStopped)
    {}
    int GetDownloadSpeed() const { return m_downloadSpeed; }
    int GetUploadSpeed() const { return m_uploadSpeed; }
    int GetNumActive() const { return m_numActive; }
    int GetNumWaiting() const { return m_numWaiting; }
    int GetNumStopped() const { return m_numStopped; }
    // implement the base class pure virtual
    virtual wxEvent *Clone() const { return new GlobalEvent(*this); }
private:
    //Overall download speed (byte/sec)
    int m_downloadSpeed;
    //Overall upload speed(byte/sec)
    int m_uploadSpeed;
    //The number of active downloads
    int m_numActive;
    //The number of waiting downloads
    int m_numWaiting;
    //The number of stopped downloads
    int m_numStopped;
};

class DownloadEvent : public wxThreadEvent
{
public:
    DownloadEvent(int winid, aria2::A2Gid gid, aria2::DownloadStatus status, int64_t totalLength, int64_t completedLength,
                  int64_t uploadLength, std::string bitfield, int downloadSpeed, int uploadSpeed, size_t pieceLength,
                  int numPieces)
        : wxThreadEvent(wxEVT_THREAD, winid),
          m_gid(gid), m_status(status), m_totalLength(totalLength), m_completedLength(completedLength), m_uploadLength(uploadLength),
          m_bitfield(bitfield), m_downloadSpeed(downloadSpeed), m_uploadSpeed(uploadSpeed), m_pieceLength(pieceLength), m_numPieces(numPieces)
    {}
    aria2::A2Gid GetGid() const { return m_gid; }
    aria2::DownloadStatus GetStatus() const { return m_status; }
    int64_t GetTotalLegth() const { return m_totalLength; }
    int64_t GetCompletedLegth() const { return m_completedLength; }
    int64_t GetUploadLegth() const { return m_uploadLength; }
    std::string GetBitfield() const { return m_bitfield; }
    int GetDownloadSpeed() const { return m_downloadSpeed; }
    int GetUploadSpeed() const { return m_uploadSpeed; }
    int GetPieceLength() const { return m_pieceLength; }
    int GetNumPieces() const { return m_numPieces; }
    // implement the base class pure virtual
    virtual wxEvent *Clone() const { return new DownloadEvent(*this); }
private:
    //download GID
    aria2::A2Gid m_gid;
    //status of this download
    aria2::DownloadStatus m_status;
    //the total length of this download in bytes
    int64_t m_totalLength;
    //the completed length of this download in bytes
    int64_t m_completedLength;
    //the uploaded length of this download in bytes
    int64_t m_uploadLength;
    //the download progress in byte-string. The highest bit corresponds to piece index 0. The set bits indicate the piece is available and unset bits indicate the piece is missing.
    //The spare bits at the end are set to zero.
    //When download has not started yet, returns empty string.
    std::string m_bitfield;
    //download speed of this download measured in bytes/sec
    int m_downloadSpeed;
    //upload speed of this download measured in bytes/sec
    int m_uploadSpeed;
    //piece length in bytes
    size_t m_pieceLength;
    //the number of pieces
    int m_numPieces;
};

class mEngine : public wxThread
{
public:
    mEngine(mMainFrame *frame);
    ~mEngine();

    virtual void *Entry();
    virtual void OnExit();
    aria2::A2Gid CheckVersion();
    mFileInfo FillInfo(aria2::A2Gid gid);
    void AddFileInfo(const mFileInfo &info);
    void AddGid(aria2::A2Gid gid);
    bool HasGid(aria2::A2Gid gid);
    bool IsActive(aria2::A2Gid gid);
    void PauseDownload(aria2::A2Gid gid);
    void ResumeDownload(aria2::A2Gid gid, const wxString &destination, int connections, int bandwidth);
    bool RemoveDownload(aria2::A2Gid gid);
    wxString RealFileName(aria2::A2Gid gid);
    void SetGlobalBandwith();
    void SetDownloadBandwith(aria2::A2Gid gid, int bandwidth);
    void UpdateOptions();
protected:
    mMainFrame *m_frame;
private:
    aria2::Session *m_session;
    std::vector<aria2::A2Gid> m_gids;
    aria2::A2Gid m_versionGid;
    bool m_checkversion;
    bool m_started;
};

#endif // ENGINE_H

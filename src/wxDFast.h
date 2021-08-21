/***************************************************************
 * Name:      wxDFast.h
 * Author:    Max Magalhães Velasques (maxvelasques@gmail.com)
 *            David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2006
 * License:   GPL2
 **************************************************************/

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/snglinst.h>
#include <wx/taskbar.h>
#include <wx/splitter.h>
#include <wx/notebook.h>
#include <wx/listbook.h>
#include <wx/socket.h>
#if wxUSE_FINDREPLDLG
  #include <wx/fdrepdlg.h>
#endif // wxUSE_FINDREPLDLG
#include "widgets/dxtoolbar.h"
#include "widgets/dxlistctrl.h"
#include "Graph.h"
#include "Progress.h"
#include "FileInfo.h"

class mApp : public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();
protected:
    wxSingleInstanceChecker *m_checker;
    wxLocale *m_Locale;
};

class mTaskBarIcon: public wxTaskBarIcon
{
    DECLARE_EVENT_TABLE()
public:
#if defined(__WXCOCOA__)
    mTaskBarIcon(wxTaskBarIconType iconType = DEFAULT_TYPE) : wxTaskBarIcon(iconType)
#else
    mTaskBarIcon()
#endif
    {}
    ~mTaskBarIcon() {}

    virtual wxMenu *CreatePopupMenu();
private:
    void OnLeftButtonDClick(wxTaskBarIconEvent& event);
    void OnClose(wxCommandEvent& event);
    void OnHide(wxCommandEvent& event);
    void OnNew(wxCommandEvent& event);
    void OnBandUnlimited(wxCommandEvent& event);
    void OnBandControlOn(wxCommandEvent& event);
    void OnBandControlPerDownload(wxCommandEvent& event);
};

class mMainFrame;

class ProgressCtrl: public dxListCtrl
{
public:
    ProgressCtrl(mMainFrame *frame, wxWindow *parent, const wxWindowID id)
        : dxListCtrl(parent, id), m_frame(frame) {}

    void CreateColumns();
private:
    mMainFrame *m_frame;

    virtual wxString OnGetItemText(long item, long column) const;
    virtual int OnGetItemColumnImage(long item, long column) const;
    virtual wxListItemAttr *OnGetItemAttr(long item) const;
};

class FinishedCtrl: public dxListCtrl
{
public:
    FinishedCtrl(mMainFrame *frame, wxWindow *parent, const wxWindowID id)
        : dxListCtrl(parent, id), m_frame(frame) {}

    void CreateColumns();
private:
    mMainFrame *m_frame;

    virtual wxString OnGetItemText(long item, long column) const;
    virtual int OnGetItemColumnImage(long item, long column) const;
    virtual wxListItemAttr *OnGetItemAttr(long item) const;
};

class FinishedInfoCtrl: public dxListCtrl
{
public:
    FinishedInfoCtrl(mMainFrame *frame, wxWindow *parent, const wxWindowID id)
        : dxListCtrl(parent, id), m_frame(frame) {}

    void CreateColumns();
private:
    mMainFrame *m_frame;

    virtual wxString OnGetItemText(long item, long column) const;
    virtual int OnGetItemColumnImage(long item, long column) const;
    virtual wxListItemAttr *OnGetItemAttr(long item) const;
};

class mEngine;

class mMainFrame: public wxFrame
{
    DECLARE_EVENT_TABLE()
    friend class mEngine;
public:
    mMainFrame();
    ~mMainFrame();

    wxString GetDownloadItemText(long item, long column);
    int GetDownloadItemColumnImage(long item, long column);
    wxString GetFinishItemText(long item, long column);
    int GetFinishItemColumnImage(long item, long column);
    wxString GetFinishInfoItemText(long item, long column);
    wxString GetTaskTooltip();
    void AddDownload(const wxString &destination, const wxString &comment, const wxString &reference, wxArrayString urls);
protected:
    mEngine *m_engine;
    wxCriticalSection m_engineCS;
private:
    wxBoxSizer *m_sizer;
    dxToolBar *m_bar;
    mTaskBarIcon *m_taskbaricon;
    wxMenuItem *m_menunew;
    wxMenuItem *m_menuremove;
    wxMenuItem *m_menuschedule;
    wxMenuItem *m_menustart;
    wxMenuItem *m_menustop;
    wxMenuItem *m_menustartall;
    wxMenuItem *m_menustopall;
    wxMenuItem *m_menupaste;
    wxMenuItem *m_menucopyurl;
    wxMenuItem *m_menucopydownloaddata;
    wxMenuItem *m_menuremoveall;
    wxMenuItem *m_menufind;
    wxMenuItem *m_menushowgraph;
    wxMenuItem *m_menudetails;
    wxMenuItem *m_menusite;
    wxMenuItem *m_menuproperties;
    wxMenuItem *m_menumove;
    wxMenuItem *m_menumd5;
    wxMenuItem *m_menuopendestination;
    wxMenuItem *m_menuagain;
    wxMenuItem *m_menuexport;
    wxMenuItem *m_menuimport;
    wxMenuItem *m_menushutdown;
    wxMenuItem *m_menudisconnect;
    wxNotebook *m_book;
    mGraph *m_graph;
    ProgressCtrl *m_progresslistctrl;
    FinishedCtrl *m_finishedlistctrl;
    FinishedInfoCtrl *m_finishedinfoctrl;
    wxSplitterWindow *m_progresssplitter;
    wxSplitterWindow *m_finishedsplitter;
    wxListbook *m_lbook;
    mProgress *m_progress;
    wxTextCtrl *m_detail;
    std::vector<mFileInfo> m_progressList;
    std::vector<mFileInfo> m_finishedList;
    wxScopedPtr<wxImageList> m_imagelist;
    long m_selectedProgress;
    long m_selectedFinished;
    wxTimer m_timer;
    bool m_havenotify;
    wxSocketServer *m_server;
    wxString m_ipcString;
#if wxUSE_FINDREPLDLG
    wxFindReplaceDialog *m_dlgFind;
#endif // wxUSE_FINDREPLDLG

    void OnClose(wxCloseEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnSite(wxCommandEvent& event);
    void OnNew(wxCommandEvent& event);
    void OnRemove(wxCommandEvent& event);
    void OnSchedule(wxCommandEvent& event);
    void OnStart(wxCommandEvent& event);
    void OnStop(wxCommandEvent& event);
    void OnStartAll(wxCommandEvent& event);
    void OnStopAll(wxCommandEvent& event);
    void OnBug(wxCommandEvent& event);
    void OnDonate(wxCommandEvent& event);
    void OnShowGraph(wxCommandEvent& event);
    void OnDetails(wxCommandEvent& event);
    void OnPasteURL(wxCommandEvent& event);
    void OnOptions(wxCommandEvent& event);
    void OnProperties(wxCommandEvent& event);
    void OnRemoveAll(wxCommandEvent& event);
    void OnFind(wxCommandEvent& event);
    void OnDownloadAgain(wxCommandEvent& event);
    void OnMove(wxCommandEvent& event);
    void OnCheckMD5(wxCommandEvent& event);
    void OnOpenDestination(wxCommandEvent& event);
    void OnCopyURL(wxCommandEvent& event);
    void OnCopyDownloadData(wxCommandEvent& event);
    void OnExportConf(wxCommandEvent& event);
    void OnImportConf(wxCommandEvent& event);
    void OnShutdown(wxCommandEvent& event);
    void OnDisconnect(wxCommandEvent& event);
    void OnUp(wxCommandEvent& event);
    void OnDown(wxCommandEvent& event);
    void OnProgressRightClick(wxListEvent& event);
    void OnProgressActivated(wxListEvent& event);
    void OnProgressSelected(wxListEvent& event);
    void OnProgressDeselected(wxListEvent& event);
    void OnFinishedRightClick(wxListEvent& event);
    void OnFinishedActivated(wxListEvent& event);
    void OnFinishedSelected(wxListEvent& event);
    void OnFinishedDeselected(wxListEvent& event);
    void OnPageChanged(wxBookCtrlEvent& event);
    void OnGlobalStat(wxThreadEvent& event);
    void OnDownloadStat(wxThreadEvent& event);
    void OnCheckVersion(wxThreadEvent& event);
    void OnDownloadRemoved(wxThreadEvent& event);
    void OnEngineStarted(wxThreadEvent& event);
    void OnTimer(wxTimerEvent& event);
    void OnBandUnlimited(wxCommandEvent& event);
    void OnBandControlOn(wxCommandEvent& event);
    void OnBandControlPerDownload(wxCommandEvent& event);
    void OnServerEvent(wxSocketEvent& event);
    void OnSocketEvent(wxSocketEvent& event);
#if wxUSE_FINDREPLDLG
    void OnFindDialog(wxFindDialogEvent& event);
#endif // wxUSE_FINDREPLDLG

    void SetBandwidth();
    void SetStatusBandwidth();
    void SetStatusSpeed(int down, int up);
    void ShowHideResizeGraph();
    void UpdateProgressList();
    void UpdateFinishedList();
    void EnableTools();
    bool EnableStartAll();
    bool SomeProcessActive();
    bool SomeProcessOnDownload();
    int LastIndex();
    void StartDownload(size_t item);
    void PauseDownload(size_t item);
    bool ReadProgressList(wxString dir="");
    bool ReadFinishedList(wxString dir="");
    bool SaveProgressList();
    bool SaveFinishedList();
    std::string CreateGID();
    bool HasGID(aria2::A2Gid gid);
    aria2::A2Gid GenerateGID();
    bool ExportConf(wxString dir);
    void LogAfterFirstStart(size_t item);
    void UpdateDetail();
    void ShowNotify(const wxString &title, const wxString &message);
    void ShutDownOrDisconnect();
    void ParseReceived(const wxString &text);
    int CurrentSimultaneous();
    void RemoveFromProgressList(size_t item);
    void RemoveFromFinishedList(size_t item, bool removefile);
    void SelectProgressByIndex(int index);
};

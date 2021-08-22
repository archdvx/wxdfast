/***************************************************************
 * Name:      wxDFast.cpp
 * Author:    Max Magalhães Velasques (maxvelasques@gmail.com)
 *            David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2006
 * License:   GPL2
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include <aria2/aria2.h>
#include <wx/aboutdlg.h>
#include <wx/sysopt.h>
#include <wx/file.h>
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>
#include <wx/mimetype.h>
#include <wx/filename.h>
#include <wx/clipbrd.h>
#include <wx/progdlg.h>
#include <wx/notifmsg.h>
#include <algorithm>
#include <random>
#include <cinttypes>
#include "wxDFast.h"
#include "Icons.h"
#include "Defs.h"
#include "Options.h"
#include "UtilFunctions.h"
#include "OptionsDialog.h"
#include "Engine.h"
#include "wxMD5/wxMD5.h"
#include "NewDialog.h"
#include "BatchDialog.h"
#include "wxjson/include/wx/jsonreader.h"
#include "wxjson/include/wx/jsonval.h"
#include "IpcDefs.h"
#include "config.h"

mMainFrame* frame = nullptr;
bool frameShown = true;

const wxString usage_msg = _(""
"Usage: wxdfast [-h] [-v] [-i] [-l <str>] [-d <str>] [-c <str>] [-r <str>] [URL of the file(s) to be downloaded...]\n"
"  -h, --help                   Print (this) help screen and exit\n"
"  -v, --version                Print version information and exit\n"
"  -i, --hide                   Start with the the main frame hide\n"
"  -d, --destination=<str>      Destination directory\n"
"  -c, --comments=<str>         Add comments to download\n"
"  -r, --reference=<str>        Set a reference URL"
"");


IMPLEMENT_APP(mApp)

bool SortFileInfo(mFileInfo f1, mFileInfo f2)
{
    return f1.index() < f2.index();
}

bool mApp::OnInit()
{
    bool tray = false;
    wxString destination = "", comment = "", reference = "";
    wxArrayString urls;
    for(int i=0; i<this->argc; ++i)
    {
        if(this->argv[i].CmpNoCase("-v")==0 || this->argv[i].CmpNoCase("--version")==0)
        {
            fprintf(stdout, "%s %s\n", PROGRAM_NAME, VERSION);
            exit(0);
        }
        if(this->argv[i].CmpNoCase("-h")==0 || this->argv[i].CmpNoCase("--help")==0)
        {
            fprintf(stdout, "%s\n", usage_msg.c_str().AsChar());
            exit(0);
        }
        if(this->argv[i].CmpNoCase("-i")==0 || this->argv[i].CmpNoCase("--hide")==0)
        {
            tray = true;
            continue;
        }
        if(this->argv[i].CmpNoCase("-d")==0)
        {
            if(i+1 < this->argc)
            {
                destination = this->argv[i];
            }
            continue;
        }
        if(this->argv[i].StartsWith("--destination="))
        {
            destination = this->argv[i].AfterFirst('=');
            continue;
        }
        if(this->argv[i].CmpNoCase("-c")==0)
        {
            if(i+1 < this->argc)
            {
                comment = this->argv[i];
            }
            continue;
        }
        if(this->argv[i].StartsWith("--comments="))
        {
            comment = this->argv[i].AfterFirst('=');
            continue;
        }
        if(this->argv[i].CmpNoCase("-r")==0)
        {
            if(i+1 < this->argc)
            {
                reference = this->argv[i];
            }
            continue;
        }
        if(this->argv[i].StartsWith("--reference="))
        {
            reference = this->argv[i].AfterFirst('=');
            continue;
        }
        if(i)
        {
            urls.Add(this->argv[i]);
        }
    }
    m_checker = new wxSingleInstanceChecker;
    if(m_checker->IsAnotherRunning())
    {
        if(!tray) wxLogError(_("wxDownload Fast already running"));
        delete m_checker; // OnExit() won't be called if we return false
        m_checker = nullptr;
        return false;
    }
    wxInitAllImageHandlers();
    moptions.load();
    makeAllIcons("default");
#if defined (__WXMAC__)
    wxSystemOptions::SetOption(_T("mac.listctrl.always_use_generic"),1);
#endif
    wxLocale::AddCatalogLookupPathPrefix(LOCALEDIR);
    m_Locale = new wxLocale();
    m_Locale->Init(wxLANGUAGE_DEFAULT);
    m_Locale->AddCatalog("wxstd");
    m_Locale->AddCatalog("wxdfast");

    aria2::libraryInit();

    frame = new mMainFrame();
    frame->SetIcon(ICO_ICON); // To Set App Icon
    frame->Show();

    if(tray)
    {
        frame->Hide();
        frameShown = false;
    }

    MyUtilFunctions::BrowserIntegration(wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath());

    if(urls.GetCount())
    {
        frame->AddDownload(destination, comment, reference, urls);
    }

    return true;
}

int mApp::OnExit()
{
    aria2::libraryDeinit();
    delete m_checker;
    return 0;
}

BEGIN_EVENT_TABLE(mTaskBarIcon, wxTaskBarIcon)
    EVT_MENU(ID_MENU_HIDE, mTaskBarIcon::OnHide)
    EVT_MENU(ID_MENU_NEW, mTaskBarIcon::OnNew)
    EVT_MENU(wxID_EXIT, mTaskBarIcon::OnClose)
    EVT_MENU(ID_MENU_OFF, mTaskBarIcon::OnBandUnlimited)
    EVT_MENU(ID_MENU_ON, mTaskBarIcon::OnBandControlOn)
    EVT_MENU(ID_MENU_PERDOWNLOAD,  mTaskBarIcon::OnBandControlPerDownload)
    EVT_TASKBAR_LEFT_DCLICK(mTaskBarIcon::OnLeftButtonDClick)
END_EVENT_TABLE()

wxMenu *mTaskBarIcon::CreatePopupMenu()
{
    wxMenu *menu = new wxMenu;
    if(frameShown)
        menu->Append(ID_MENU_HIDE, _("Hide the main window"));
    else
        menu->Append(ID_MENU_HIDE, _("Show the main window"));
    menu->AppendSeparator();
    menu->Append(ID_MENU_NEW, _("New download"));
    wxMenu *subband = new wxMenu;
    wxMenuItem *off = subband->Append(ID_MENU_OFF, _("Unlimited"), _("Just for this session. To change definitely go to \"Preferences\""), wxITEM_RADIO);
    wxMenuItem *on = subband->Append(ID_MENU_ON, _("Active")+" ("+MyUtilFunctions::SpeedText(moptions.bandwidth()*1024, true, false)+")", _("Just for this session. To change definitely go to \"Preferences\""), wxITEM_RADIO);
    wxMenuItem *per = subband->Append(ID_MENU_PERDOWNLOAD, _("Per Download"), _("Just for this session. To change definitely go to \"Preferences\""), wxITEM_RADIO);
    if(moptions.livebandwidthoption() == 1)
        per->Check();
    else if(moptions.livebandwidthoption() == 2)
        on->Check();
    else
        off->Check();
    menu->Append(wxID_ANY, _("Band control"), subband);
    menu->AppendSeparator();
    menu->Append(wxID_EXIT, _("Quit"));
    return menu;
}

void mTaskBarIcon::OnLeftButtonDClick(wxTaskBarIconEvent &/*event*/)
{
    if(frameShown)
    {
        frame->Show(false);
    }
    else
    {
        frame->Iconize(false);
        frame->SetFocus();
        frame->Raise();
        frame->Show(true);
    }
    frameShown = !frameShown;
}

void mTaskBarIcon::OnClose(wxCommandEvent &event)
{
    wxPostEvent(frame, event);
}

void mTaskBarIcon::OnHide(wxCommandEvent &/*event*/)
{
    if(frameShown)
    {
        frame->Show(false);
    }
    else
    {
        frame->Iconize(false);
        frame->SetFocus();
        frame->Raise();
        frame->Show(true);
    }
    frameShown = !frameShown;
}

void mTaskBarIcon::OnNew(wxCommandEvent &event)
{
    wxPostEvent(frame, event);
}

void mTaskBarIcon::OnBandUnlimited(wxCommandEvent &event)
{
    wxPostEvent(frame, event);
}

void mTaskBarIcon::OnBandControlOn(wxCommandEvent &event)
{
    wxPostEvent(frame, event);
}

void mTaskBarIcon::OnBandControlPerDownload(wxCommandEvent &event)
{
    wxPostEvent(frame, event);
}

BEGIN_EVENT_TABLE(mMainFrame,wxFrame)
    EVT_CLOSE(mMainFrame::OnClose)
    EVT_MENU(wxID_EXIT, mMainFrame::OnQuit)
    EVT_MENU(wxID_ABOUT, mMainFrame::OnAbout)
    EVT_MENU(ID_MENU_NEW, mMainFrame::OnNew)
    EVT_MENU(ID_MENU_REMOVE, mMainFrame::OnRemove)
    EVT_MENU(ID_MENU_SCHEDULE, mMainFrame::OnSchedule)
    EVT_MENU(ID_MENU_START, mMainFrame::OnStart)
    EVT_MENU(ID_MENU_STOP, mMainFrame::OnStop)
    EVT_MENU(ID_MENU_STARTALL,  mMainFrame::OnStartAll)
    EVT_MENU(ID_MENU_STOPALL,  mMainFrame::OnStopAll)
    EVT_MENU(ID_MENU_SITE, mMainFrame::OnSite)
    EVT_MENU(ID_MENU_BUG, mMainFrame::OnBug)
    EVT_MENU(ID_MENU_DONATE,  mMainFrame::OnDonate)
    EVT_MENU(ID_MENU_SHOWGRAPH, mMainFrame::OnShowGraph)
    EVT_MENU(ID_MENU_DETAILS, mMainFrame::OnDetails)
    EVT_MENU(ID_MENU_PASTEURL, mMainFrame::OnPasteURL)
    EVT_MENU(wxID_PREFERENCES, mMainFrame::OnOptions)
    EVT_MENU(ID_MENU_INFO, mMainFrame::OnProperties)
    EVT_MENU(ID_MENU_REMOVEALL, mMainFrame::OnRemoveAll)
    EVT_MENU(ID_MENU_FIND, mMainFrame::OnFind)
    EVT_MENU(ID_MENU_AGAIN, mMainFrame::OnDownloadAgain)
    EVT_MENU(ID_MENU_MOVE, mMainFrame::OnMove)
    EVT_MENU(ID_MENU_MD5, mMainFrame::OnCheckMD5)
    EVT_MENU(ID_MENU_OPENDESTINATION, mMainFrame::OnOpenDestination)
    EVT_MENU(ID_MENU_COPYURL, mMainFrame::OnCopyURL)
    EVT_MENU(ID_MENU_COPYDOWNLOADDATA, mMainFrame::OnCopyDownloadData)
    EVT_MENU(ID_MENU_EXPORT, mMainFrame::OnExportConf)
    EVT_MENU(ID_MENU_IMPORT, mMainFrame::OnImportConf)
    EVT_MENU(ID_MENU_SHUTDOWN, mMainFrame::OnShutdown)
    EVT_MENU(ID_MENU_DISCONNECT, mMainFrame::OnDisconnect)
    EVT_MENU(ID_MENU_UP, mMainFrame::OnUp)
    EVT_MENU(ID_MENU_DOWN, mMainFrame::OnDown)
    EVT_MENU(ID_MENU_OFF, mMainFrame::OnBandUnlimited)
    EVT_MENU(ID_MENU_ON, mMainFrame::OnBandControlOn)
    EVT_MENU(ID_MENU_PERDOWNLOAD,  mMainFrame::OnBandControlPerDownload)
    EVT_LIST_ITEM_RIGHT_CLICK(ID_LIST_PROGRESS, mMainFrame::OnProgressRightClick)
    EVT_LIST_ITEM_ACTIVATED(ID_LIST_PROGRESS, mMainFrame::OnProgressActivated)
    EVT_LIST_ITEM_SELECTED(ID_LIST_PROGRESS, mMainFrame::OnProgressSelected)
    EVT_LIST_ITEM_DESELECTED(ID_LIST_PROGRESS, mMainFrame::OnProgressDeselected)
    EVT_LIST_ITEM_RIGHT_CLICK(ID_LIST_FINISHED, mMainFrame::OnFinishedRightClick)
    EVT_LIST_ITEM_ACTIVATED(ID_LIST_FINISHED, mMainFrame::OnFinishedActivated)
    EVT_LIST_ITEM_SELECTED(ID_LIST_FINISHED, mMainFrame::OnFinishedSelected)
    EVT_LIST_ITEM_DESELECTED(ID_LIST_FINISHED, mMainFrame::OnFinishedDeselected)
    EVT_NOTEBOOK_PAGE_CHANGED(ID_NOTEBOOK, mMainFrame::OnPageChanged)
    EVT_TIMER(wxID_ANY, mMainFrame::OnTimer)
    EVT_THREAD(ID_ENGINE_GLOBAL, mMainFrame::OnGlobalStat)
    EVT_THREAD(ID_ENGINE_DOWNLOAD, mMainFrame::OnDownloadStat)
    EVT_THREAD(ID_ENGINE_VERSION, mMainFrame::OnCheckVersion)
    EVT_THREAD(ID_ENGINE_REMOVED, mMainFrame::OnDownloadRemoved)
    EVT_THREAD(ID_ENGINE_STARTED, mMainFrame::OnEngineStarted)
    EVT_SOCKET(ID_IPC, mMainFrame::OnServerEvent)
    EVT_SOCKET(ID_IPC_SOCKET, mMainFrame::OnSocketEvent)
#if wxUSE_FINDREPLDLG
    EVT_FIND(wxID_ANY, mMainFrame::OnFindDialog)
    EVT_FIND_NEXT(wxID_ANY, mMainFrame::OnFindDialog)
    EVT_FIND_CLOSE(wxID_ANY, mMainFrame::OnFindDialog)
#endif // wxUSE_FINDREPLDLG
END_EVENT_TABLE()

mMainFrame::mMainFrame()
    : wxFrame(nullptr, -1, PROGRAM_NAME, wxDefaultPosition, wxSize(600,400)), m_selectedProgress(-1), m_selectedFinished(-1), m_timer(this), m_ipcString("")
{
    m_havenotify = false;
#ifdef __UNIX__
    wxString contents;
    if(wxGetEnv("PATH", &contents))
    {
        wxArrayString path = wxStringTokenize(contents, wxPATH_SEP);
        for(size_t i=0; i<path.GetCount(); i++)
        {
            if(wxFileExists(path[i]+wxFILE_SEP_PATH+"notify-send"))
            {
                m_havenotify = true;
                break;
            }
        }
    }
#endif // __UNIX__

#if wxUSE_MENUS
    // create a menu bar
    wxMenuBar* mbar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu();
#if defined (__WXMAC__)
    fileMenu->Append(wxID_ABOUT, _("About"));
    fileMenu->Append(wxID_PREFERENCES, _("Preferences"));
#endif
    m_menunew = new wxMenuItem(nullptr, ID_MENU_NEW, _("New Download")+"\tCtrl-N");
    m_menunew->SetBitmap(ICO_NEW);
    fileMenu->Append(m_menunew);
    m_menuremove = new wxMenuItem(nullptr, ID_MENU_REMOVE, _("Remove Download")+"\tCtrl-R");
    m_menuremove->SetBitmap(ICO_REMOVE);
    fileMenu->Append(m_menuremove);
    fileMenu->AppendSeparator();
    m_menuschedule = new wxMenuItem(nullptr, ID_MENU_SCHEDULE, _("Schedule Download")+"\tCtrl-H");
    m_menuschedule->SetBitmap(ICO_SCHEDULE);
    fileMenu->Append(m_menuschedule);
    m_menustart = new wxMenuItem(nullptr, ID_MENU_START, _("Start Download")+"\tCtrl-S");
    m_menustart->SetBitmap(ICO_START);
    fileMenu->Append(m_menustart);
    m_menustop = new wxMenuItem(nullptr, ID_MENU_STOP, _("Stop Download")+"\tCtrl-T");
    m_menustop->SetBitmap(ICO_STOP);
    fileMenu->Append(m_menustop);
    m_menustartall = new wxMenuItem(nullptr, ID_MENU_STARTALL, _("Start all downloads"));
    m_menustartall->SetBitmap(ICO_STARTALL);
    fileMenu->Append(m_menustartall);
    m_menustopall = new wxMenuItem(nullptr, ID_MENU_STOPALL, _("Stop all downloads"));
    m_menustopall->SetBitmap(ICO_STOPALL);
    fileMenu->Append(m_menustopall);
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, _("Quit"));
    mbar->Append(fileMenu, _("&File"));
    wxMenu *editMenu = new wxMenu();
    m_menupaste = new wxMenuItem(nullptr, ID_MENU_PASTEURL, _("Paste URL")+"\tCtrl-V");
    m_menupaste->SetBitmap(ICO_PASTEURL);
    editMenu->Append(m_menupaste);
    m_menucopyurl = new wxMenuItem(nullptr, ID_MENU_COPYURL, _("Copy URL"));
    m_menucopyurl->SetBitmap(ICO_COPYURL);
    editMenu->Append(m_menucopyurl);
    m_menucopydownloaddata = new wxMenuItem(nullptr, ID_MENU_COPYDOWNLOADDATA, _("Copy Download Data"));
    m_menucopydownloaddata->SetBitmap(ICO_COPYDATA);
    editMenu->Append(m_menucopydownloaddata);
    editMenu->AppendSeparator();
    m_menuremoveall = new wxMenuItem(nullptr, ID_MENU_REMOVEALL, _("Remove All")+"\tCtrl-A");
    editMenu->Append(m_menuremoveall);
    editMenu->AppendSeparator();
    m_menufind = new wxMenuItem(nullptr, ID_MENU_FIND, _("Find Download")+"\tCtrl-F");
    m_menufind->SetBitmap(ICO_FIND);
    editMenu->Append(m_menufind);
    mbar->Append(editMenu, _("&Edit"));
    wxMenu *viewMenu = new wxMenu();
    m_menushowgraph = new wxMenuItem(nullptr, ID_MENU_SHOWGRAPH, _("Show/Hide Graph")+"\tCtrl-G");
    m_menushowgraph->SetBitmap(ICO_GRAPH);
    viewMenu->Append(m_menushowgraph);
    m_menudetails = new wxMenuItem(nullptr, ID_MENU_DETAILS, _("Show/Hide Details")+"\tCtrl-D");
    m_menudetails->SetBitmap(ICO_DETAILS);
    viewMenu->Append(m_menudetails);
    mbar->Append(viewMenu, _("&View"));
    wxMenu* toolsMenu = new wxMenu();
    m_menuproperties = new wxMenuItem(nullptr, ID_MENU_INFO, _("Download Properties")+"\tCtrl-P");
    m_menuproperties->SetBitmap(ICO_PROPERTIES);
    toolsMenu->Append(m_menuproperties);
    m_menumove = new wxMenuItem(nullptr, ID_MENU_MOVE, _("Move File"));
    toolsMenu->Append(m_menumove);
    toolsMenu->AppendSeparator();
#if !(defined (__WXMAC__)) //TODO fix on next release
    m_menumd5 = new wxMenuItem(nullptr, ID_MENU_MD5, _("Check Integrity"));
    toolsMenu->Append(m_menumd5);
#endif
    m_menuopendestination = new wxMenuItem(nullptr, ID_MENU_OPENDESTINATION, _("Open Destination Directory"));
    toolsMenu->Append(m_menuopendestination);
    m_menuagain = new wxMenuItem(nullptr, ID_MENU_AGAIN, _("Download File Again"));
    toolsMenu->Append(m_menuagain);
    toolsMenu->AppendSeparator();
    m_menuexport = new wxMenuItem(nullptr, ID_MENU_EXPORT, _("Export Configuration"));
    toolsMenu->Append(m_menuexport);
    m_menuimport = new wxMenuItem(nullptr, ID_MENU_IMPORT, _("Import Configuration"));
    toolsMenu->Append(m_menuimport);
    toolsMenu->AppendSeparator();
    m_menushutdown = toolsMenu->AppendCheckItem(ID_MENU_SHUTDOWN, _("Shutdown when finished"), _("This option is only valid in this session of the program"));
    m_menudisconnect = toolsMenu->AppendCheckItem(ID_MENU_DISCONNECT, _("Disconnect when finished"), _("This option is only valid in this session of the program"));
#if !(defined (__WXMAC__))
    toolsMenu->AppendSeparator();
    toolsMenu->Append(wxID_PREFERENCES, _("Preferences"));
#endif
    mbar->Append(toolsMenu, _("&Tools"));
    wxMenu* helpMenu = new wxMenu();
    m_menusite = new wxMenuItem(nullptr, ID_MENU_SITE, _("wxDownload Fast Site"));
    m_menusite->SetBitmap(ICO_HELP);
    helpMenu->Append(m_menusite);
    helpMenu->Append(ID_MENU_BUG, _("Report Bugs..."));
    helpMenu->Append(ID_MENU_DONATE, _("Donate..."));
#if !(defined (__WXMAC__))
    helpMenu->AppendSeparator();
    helpMenu->Append(wxID_ABOUT, _("About"));
#endif
    mbar->Append(helpMenu, _("&Help"));
    SetMenuBar(mbar);
#endif // wxUSE_MENUS

#if defined(__WXCOCOA__)
        m_taskbaricon = new mTaskBarIcon(wxTaskBarIcon::DOCK);
#else
        m_taskbaricon = new mTaskBarIcon();
        m_taskbaricon->SetIcon(ICO_TRAY, PROGRAM_NAME);
#endif

    wxPanel* panel = new wxPanel(this);

    m_sizer = new wxBoxSizer(wxVERTICAL);

    m_bar = new dxToolBar(panel);
    m_bar->AddTool(ID_MENU_NEW, _("New"), ICO_DOWNLOAD_NEW, _("Add new download"));
    m_bar->AddTool(ID_MENU_REMOVE, _("Remove"), ICO_DOWNLOAD_REMOVE, ICO_GDOWNLOAD_REMOVE, wxITEM_NORMAL, _("Remove the selected download"));
    m_bar->AddSeparator();
    m_bar->AddTool(ID_MENU_SCHEDULE, _("Schedule"), ICO_DOWNLOAD_SCHEDULE, ICO_GDOWNLOAD_SCHEDULE, wxITEM_NORMAL, _("Mark the selected download as scheduled"));
    m_bar->AddTool(ID_MENU_START, _("Start"), ICO_DOWNLOAD_START, ICO_GDOWNLOAD_START, wxITEM_NORMAL, _("Start the selected download"));
    m_bar->AddTool(ID_MENU_STOP, _("Stop"), ICO_DOWNLOAD_STOP, ICO_GDOWNLOAD_STOP, wxITEM_NORMAL, _("Stop the selected download"));
    m_bar->AddTool(ID_MENU_STARTALL, _("Start All"), ICO_DOWNLOAD_START_ALL, ICO_GDOWNLOAD_START_ALL, wxITEM_NORMAL, _("Start all downloads"));
    m_bar->AddTool(ID_MENU_STOPALL, _("Stop All"), ICO_DOWNLOAD_STOP_ALL, ICO_GDOWNLOAD_STOP_ALL, wxITEM_NORMAL, _("Stop all downloads"));
    m_bar->AddSeparator();
    m_bar->AddTool(ID_MENU_UP, _("Up"), ICO_DOWNLOAD_MOVE_UP, ICO_GDOWNLOAD_MOVE_UP, wxITEM_NORMAL, _("Push download higher up the queue"));
    m_bar->AddTool(ID_MENU_DOWN, _("Down"), ICO_DOWNLOAD_MOVE_DOWN, ICO_GDOWNLOAD_MOVE_DOWN, wxITEM_NORMAL, _("Push download lower down the queue"));
    m_bar->AddSeparator();
    m_bar->AddTool(ID_MENU_INFO, _("Properties"), ICO_DOWNLOAD_INFO, ICO_GDOWNLOAD_INFO, wxITEM_NORMAL, _("Show the properties of the selected download"));
    m_bar->Realize();
    m_sizer->Add(m_bar, 0, wxALL|wxEXPAND, 5);

    m_graph = new mGraph(panel);
    m_graph->SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    m_graph->SetSize(wxSize(-1,moptions.graphheight()));
    m_sizer->Add(m_graph, 0, wxEXPAND|wxFIXED_MINSIZE, 5);
    m_graph->Show(moptions.graphshow());

    m_imagelist.reset(new wxImageList(16, 16));
    m_imagelist->Add(ICO_DOWNLOADING);
    m_imagelist->Add(ICO_PAUSE);
    m_imagelist->Add(ICO_SCHEDULED);
    m_imagelist->Add(ICO_COMPLETED);
    m_imagelist->Add(ICO_ERROR);
    m_book = new wxNotebook(panel, ID_NOTEBOOK);
    wxPanel *page1 = new wxPanel(m_book);
    wxBoxSizer *psizer = new wxBoxSizer(wxVERTICAL);
    m_progresssplitter = new wxSplitterWindow(page1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE);
    m_progresslistctrl = new ProgressCtrl(this, m_progresssplitter, ID_LIST_PROGRESS);
    m_progresslistctrl->SetImageList(m_imagelist.get(), wxIMAGE_LIST_SMALL);
    m_progresslistctrl->CreateColumns();
    m_progresslistctrl->ToggleWindowStyle(wxLC_SINGLE_SEL);
    m_lbook = new wxListbook(m_progresssplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLB_LEFT);
    m_progress = new mProgress(m_lbook);
    m_lbook->AddPage(m_progress, _("Progress"));
    m_detail = new wxTextCtrl(m_lbook, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_MULTILINE);
    m_lbook->AddPage(m_detail, _("Detail"));
    psizer->Add(m_progresssplitter, 1, wxALL|wxEXPAND, 5);
    page1->SetSizer(psizer);
    psizer->Fit(page1);
    psizer->SetSizeHints(page1);
    m_book->AddPage(page1, _("Downloads in Progress"));
    wxPanel *page2 = new wxPanel(m_book);
    wxBoxSizer *fsizer = new wxBoxSizer(wxVERTICAL);
    m_finishedsplitter = new wxSplitterWindow(page2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE);
    m_finishedlistctrl = new FinishedCtrl(this, m_finishedsplitter, ID_LIST_FINISHED);
    m_finishedlistctrl->SetImageList(m_imagelist.get(), wxIMAGE_LIST_SMALL);
    m_finishedlistctrl->CreateColumns();
    m_finishedlistctrl->ToggleWindowStyle(wxLC_SINGLE_SEL);
    m_finishedinfoctrl = new FinishedInfoCtrl(this, m_finishedsplitter, wxID_ANY);
    m_finishedinfoctrl->CreateColumns();
    m_finishedinfoctrl->SetItemCount(11);
    m_finishedinfoctrl->ToggleWindowStyle(wxLC_NO_HEADER);
    fsizer->Add(m_finishedsplitter, 1, wxALL|wxEXPAND, 5);
    page2->SetSizer(fsizer);
    fsizer->Fit(page2);
    fsizer->SetSizeHints(page2);
    m_book->AddPage(page2, _("Finished Downloads"));
    m_book->SetSelection(0);
    m_sizer->Add(m_book, 1, wxALL|wxEXPAND, 5);

#if wxUSE_STATUSBAR
    CreateStatusBar(3);
    SetStatusBandwidth();
#endif // wxUSE_STATUSBAR

#if wxUSE_FINDREPLDLG
    m_dlgFind = nullptr;
#endif // wxUSE_FINDREPLDLG

    panel->SetSizer(m_sizer);
    m_sizer->Fit(this);
    m_sizer->SetSizeHints(this);

    if(moptions.x() == -1 || moptions.y() == -1)
    {
        if(wxSystemSettings::GetMetric(wxSYS_SCREEN_X)<moptions.w()) moptions.setW(750);
        if(wxSystemSettings::GetMetric(wxSYS_SCREEN_Y)<moptions.h()) moptions.setH(450);
        SetSize(moptions.w(), moptions.h());
    }
    else
    {
        if(wxSystemSettings::GetMetric(wxSYS_SCREEN_X)<moptions.x()+moptions.w())
        {
            moptions.setX(10);
            moptions.setW(750);
        }
        if(wxSystemSettings::GetMetric(wxSYS_SCREEN_Y)<moptions.y()+moptions.h())
        {
            moptions.setY(10);
            moptions.setH(450);
        }
        SetSize(moptions.x(), moptions.y(), moptions.w(), moptions.h());
    }

    m_progresssplitter->SplitHorizontally(m_progresslistctrl, m_lbook, moptions.separator1());
    m_finishedsplitter->SplitHorizontally(m_finishedlistctrl, m_finishedinfoctrl, moptions.separator2());

    m_engine = new mEngine(this);
    m_engine->Create();
    m_engine->Run();

    if(1)
    {
#ifdef wxHAS_UNIX_DOMAIN_SOCKETS
        wxUNIXaddress addr;
        wxString filename = wxGetHomeDir();
        if(filename.Last() != wxT('/'))
        {
            filename += wxT('/');
        }
        filename << ".wxdfast.socket";
        remove(filename.fn_str());
        addr.Filename(filename);
#else
        wxIPV4address addr;
        addr.Service(IPC_SERVICE);
#endif
        m_server = new wxSocketServer(addr);
        if(!m_server->IsOk())
        {
#ifdef wxHAS_UNIX_DOMAIN_SOCKETS
            wxLogMessage(wxString::Format(_("Could not listen at the specified socket %s!"), addr.Filename()));
#else
            wxLogMessage(wxString::Format(_("Could not listen at the specified port %d!"), IPC_SERVICE));
#endif
            return;
        }
#ifndef wxHAS_UNIX_DOMAIN_SOCKETS
        wxIPV4address addrReal;
        if(!m_server->GetLocal(addrReal))
        {
            wxLogDebug("ERROR: couldn't get the address we bound to");
        }
        else
        {
            wxLogDebug("Server listening at %s:%u", addrReal.IPAddress(), addrReal.Service());
        }
#endif
        // Setup the event handler and subscribe to connection events
        m_server->SetEventHandler(*this, ID_IPC);
        m_server->SetNotify(wxSOCKET_CONNECTION_FLAG);
        m_server->Notify(true);
    }

    m_timer.Start(moptions.timerupdateinterval());
}

mMainFrame::~mMainFrame()
{
    m_server->Close();
#ifdef wxHAS_UNIX_DOMAIN_SOCKETS
    wxString filename = wxGetHomeDir();
    if(filename.Last() != wxT('/'))
    {
        filename += wxT('/');
    }
    filename << ".wxdfast.socket";
    remove(filename.fn_str());
#endif

    SaveProgressList();
    SaveFinishedList();

    moptions.setX(GetPosition().x);
    moptions.setY(GetPosition().y);
    moptions.setW(GetSize().x);
    moptions.setH(GetSize().y);
    if(m_progresssplitter->IsSplit())
    {
        moptions.setSeparator1(m_progresssplitter->GetSashPosition());
    }
    if(m_finishedsplitter->IsSplit())
    {
        moptions.setSeparator2(m_finishedsplitter->GetSashPosition());
    }
    moptions.save();

    if(m_taskbaricon) delete m_taskbaricon;

    if(m_timer.IsRunning())
            m_timer.Stop();
}

wxString mMainFrame::GetDownloadItemText(long item, long column)
{
    if(item < 0 || item >= static_cast<long>(m_progressList.size()))
        return "";
    switch(column) {
    case 0: return "";
    case 1:
    {
        if(m_progressList[static_cast<size_t>(item)].status()==STATUS_ACTIVE)
        {
            if(m_progressList[static_cast<size_t>(item)].totalLength()) return _("   [ Yes ]");
            else _("   [ No  ]");
        }
        return "   [     ]";
    }
    case 2: return m_progressList[static_cast<size_t>(item)].name();
    case 3: return MyUtilFunctions::SizeText(m_progressList[static_cast<size_t>(item)].totalLength());
    case 4: return MyUtilFunctions::SizeText(m_progressList[static_cast<size_t>(item)].downloadLength());
    case 5: return m_progressList[static_cast<size_t>(item)].totalLength()?
                wxString::Format("%.1f%%", (100.*m_progressList[static_cast<size_t>(item)].downloadLength())/(m_progressList[static_cast<size_t>(item)].totalLength()/1.)):
                "0%";
    case 6: return MyUtilFunctions::TimespanToWxstr(m_progressList[static_cast<size_t>(item)].timepassed());
    case 7:
    {
        if(m_progressList[static_cast<size_t>(item)].status()==STATUS_FINISHED || m_progressList[static_cast<size_t>(item)].status()==STATUS_ERROR)
        {
            return MyUtilFunctions::TimespanToWxstr(0);
        }
        if(m_progressList[static_cast<size_t>(item)].downloadSpeed())
        {
            return MyUtilFunctions::TimespanToWxstr(1000*(m_progressList[static_cast<size_t>(item)].totalLength()
                                                    -m_progressList[static_cast<size_t>(item)].downloadLength())/
                                                    m_progressList[static_cast<size_t>(item)].downloadSpeed());
        }
        else
        {
            return wxString::FromUTF8("∞");
        }
    }
    case 8: return MyUtilFunctions::SpeedText(m_progressList[static_cast<size_t>(item)].downloadSpeed(),true,false);
    case 9: return wxString::Format("%d", m_progressList[static_cast<size_t>(item)].attempts());
    default: return MyUtilFunctions::ArrayStringTowxString(m_progressList[static_cast<size_t>(item)].urls());
    }
}

int mMainFrame::GetDownloadItemColumnImage(long item, long column)
{
    if(item < 0 || item >= static_cast<long>(m_progressList.size()))
        return -1;
    if(column != 0)
        return -1;
    switch(m_progressList[static_cast<size_t>(item)].status()) {
    case STATUS_ACTIVE: return 0;
    case STATUS_ERROR: return 4;
    case STATUS_FINISHED: return 3;
    case STATUS_QUEUE: return 1;
    case STATUS_SCHEDULE_QUEUE: return 2;
    case STATUS_SCHEDULE_ACTIVE: return 0;
    case STATUS_STOPED: return 1;
    default: return -1;
    }
}

wxString mMainFrame::GetFinishItemText(long item, long column)
{
    if(item < 0 || item >= static_cast<long>(m_finishedList.size()))
        return "";
    switch(column) {
    case 1: return m_finishedList[static_cast<size_t>(item)].name();
    case 2: return MyUtilFunctions::SizeText(m_finishedList[static_cast<size_t>(item)].totalLength());
    case 3: return m_finishedList[static_cast<size_t>(item)].endTime().Format("%x %X");
    default: return "";
    }
}

int mMainFrame::GetFinishItemColumnImage(long item, long column)
{
    if(item < 0 || item >= static_cast<long>(m_finishedList.size()))
        return -1;
    if(column != 0)
        return -1;
    if(m_finishedList[static_cast<size_t>(item)].status() == STATUS_FINISHED)
        return 3;
    if(m_finishedList[static_cast<size_t>(item)].status() == STATUS_ERROR)
        return 4;
    return -1;
}

wxString mMainFrame::GetFinishInfoItemText(long item, long column)
{
    if(column == 0)
    {
        switch(item) {
        case 0: return _("Name");
        case 1: return _("File type");
        case 2: return _("Size");
        case 3: return _("Time");
        case 4: return _("Destination");
        case 5: return _("Started");
        case 6: return _("Finished");
        case 7: return _("MD5");
        case 8: return _("Reference URL");
        case 9: return _("Comments");
        case 10: return _("URLs");
        default: return "";
        }
    }
    else
    {
        if(m_selectedFinished == -1)
            return "";
        switch(item) {
        case 0: return m_finishedList[static_cast<size_t>(m_selectedFinished)].name();
        case 1:
        {
            return MyUtilFunctions::MimeType(m_finishedList[static_cast<size_t>(m_selectedFinished)].name());
        }
        case 2: return MyUtilFunctions::SizeText(m_finishedList[static_cast<size_t>(m_selectedFinished)].totalLength());
        case 3: return MyUtilFunctions::TimespanToWxstr(m_finishedList[static_cast<size_t>(m_selectedFinished)].timepassed());
        case 4: return m_finishedList[static_cast<size_t>(m_selectedFinished)].destination();
        case 5: return m_finishedList[static_cast<size_t>(m_selectedFinished)].startTime().Format("%x %X");
        case 6: return m_finishedList[static_cast<size_t>(m_selectedFinished)].endTime().Format("%x %X");
        case 7: return m_finishedList[static_cast<size_t>(m_selectedFinished)].MD5();
        case 8: return m_finishedList[static_cast<size_t>(m_selectedFinished)].link();
        case 9: return m_finishedList[static_cast<size_t>(m_selectedFinished)].comment();
        case 10: return MyUtilFunctions::ArrayStringTowxString(m_finishedList[static_cast<size_t>(m_selectedFinished)].urls());
        default: return "";
        }
    }
}

wxString mMainFrame::GetTaskTooltip()
{
    wxString taskTip = PROGRAM_NAME;
    int64_t totalpercentual = 0;
    int totalspeed = 0, count = 0;
    if(m_progressList.size())
    {
        for(auto it=m_progressList.begin(); it!=m_progressList.end(); ++it)
        {
            if((*it).status() == STATUS_ACTIVE)
            {
                totalspeed += (*it).downloadSpeed();
                totalpercentual += (*it).totalLength()?(100*(*it).downloadLength())/((*it).totalLength()):0;
                count++;
            }
        }
        if(count)
        {
            taskTip = _("Total Speed:");
            taskTip << " ";
            taskTip << MyUtilFunctions::SpeedText(totalspeed, true, false);
            taskTip << "\n";
            taskTip << _("Total Complete:");
            taskTip << " ";
            taskTip << wxString::Format("%lld%%", totalpercentual/count);
            taskTip << "\n";
            taskTip << _("Downloads in progress:");
            taskTip << " ";
            taskTip << wxString::Format("%d", count);
        }
    }
    return taskTip;
}

void mMainFrame::AddDownload(const wxString &destination, const wxString &comment, const wxString &reference, wxArrayString urls)
{
#if defined (__WXMAC__)
    mNewDialog dialog(nullptr);
#else
    mNewDialog dialog(this);
#endif
    dialog.SetUrls(urls);
    dialog.SetReferenceURL(reference);
    dialog.SetComment(comment);
    if(wxDirExists(destination))
        dialog.SetDestination(destination);
    if(moptions.rememberboxnewoptions())
    {
        dialog.SetBandwidth(moptions.lastbandwidth());
        dialog.SetCommand(moptions.lastcommand());
        if(!wxDirExists(destination))
            dialog.SetDestination(moptions.lastdestination());
        dialog.SetNumberOfConnections(moptions.lastnumberofconnections());
        dialog.SetOnTop(moptions.lastontopoption());
        dialog.SetStartOption(moptions.laststartoption());
        dialog.SetSize(moptions.lastx(),moptions.lasty(),-1,-1);
    }
    if(dialog.ShowModal() == wxID_OK)
    {
        mFileInfo info;
        if(dialog.GetOnTop())
        {
            info.setIndex(1);
            for(auto it=m_progressList.begin(); it!=m_progressList.end(); ++it)
            {
                (*it).setIndex((*it).index()+1);
            }
        }
        else
        {
            info.setIndex(LastIndex());
        }
        info.setGid(GenerateGID());
        info.setDestination(dialog.GetDestination());
        info.setUrls(dialog.GetUrls());
        info.setUser(dialog.GetUser());
        info.setPassword(dialog.GetPassword());
        if(dialog.GetStartOption() == NOW)
            info.setStatus(STATUS_STOPED);
        else if(dialog.GetStartOption() == MANUAL)
            info.setStatus(STATUS_QUEUE);
        else
            info.setStatus(STATUS_SCHEDULE_QUEUE);
        info.setConnections(dialog.GetNumberOfConnections());
        info.setBandwidth(dialog.GetBandwidth());
        info.setComment(dialog.GetComment());
        info.setLink(dialog.GetReferenceURL());
        info.setCommand(dialog.GetCommand());
        info.setName(dialog.GetUrls().Item(0).AfterLast('/'));
        m_progressList.push_back(info);
        std::sort(m_progressList.begin(), m_progressList.end(), SortFileInfo);
        UpdateProgressList();
        m_selectedProgress = m_progressList.size()-1;
        m_progresslistctrl->SetItemState(m_selectedProgress, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        if(moptions.rememberboxnewoptions())
        {
            int lastx, lasty;
            dialog.GetPosition(&lastx,&lasty);
            moptions.setLastx(lastx);
            moptions.setLasty(lasty);
            moptions.setLastbandwidth(dialog.GetBandwidth());
            moptions.setLastcommand(dialog.GetCommand());
            moptions.setLastdestination(dialog.GetDestination());
            moptions.setLastnumberofconnections(dialog.GetNumberOfConnections());
            moptions.setLastontopoption(dialog.GetOnTop());
            moptions.setLaststartoption(dialog.GetStartOption());
        }
        if(info.status() != STATUS_QUEUE)
            StartDownload(m_selectedProgress);
    }
}

void mMainFrame::OnClose(wxCloseEvent &/*event*/)
{
    if(moptions.closedialog() && (this->IsShown()))
    {
        wxMessageDialog *dlg = new wxMessageDialog(this, _("Do you want to close the program?"), _("Close..."), wxYES_NO | wxICON_QUESTION);
        if(dlg)
        {
            dlg->Centre(wxBOTH);
            if(dlg->ShowModal() == wxID_YES)
            {
                dlg->Destroy();
                {
                    wxCriticalSectionLocker enter(m_engineCS);
                    if(m_engine)
                    {
                        if(m_engine->Delete() != wxTHREAD_NO_ERROR)
                            wxLogError("Can't delete the thread!");
                    }
                }
                while(1)
                {
                    {
                        wxCriticalSectionLocker enter(m_engineCS);
                        if(!m_engine) break;
                    }
                    wxThread::This()->Sleep(1);
                }
                Destroy();
                return;
            }
            else
            {
                dlg->Destroy();
                return;
            }
        }
    }
    if(m_taskbaricon)
    {
        Show(false);
        frameShown = !frameShown;
    }
    else
    {
        {
            wxCriticalSectionLocker enter(m_engineCS);
            if(m_engine)
            {
                if(m_engine->Delete() != wxTHREAD_NO_ERROR)
                    wxLogError("Can't delete the thread!");
            }
        }
        while(1)
        {
            {
                wxCriticalSectionLocker enter(m_engineCS);
                if(!m_engine) break;
            }
            wxThread::This()->Sleep(1);
        }
        Destroy();
    }
}

void mMainFrame::OnQuit(wxCommandEvent &/*event*/)
{
    if(moptions.closedialog() && (this->IsShown()))
    {
        wxMessageDialog *dlg = new wxMessageDialog(this, _("Do you want to close the program?"), _("Close..."), wxYES_NO | wxICON_QUESTION);
        if(dlg)
        {
            dlg->Centre(wxBOTH);
            if(dlg->ShowModal() == wxID_YES)
            {
                dlg->Destroy();
                {
                    wxCriticalSectionLocker enter(m_engineCS);
                    if(m_engine)
                    {
                        if(m_engine->Delete() != wxTHREAD_NO_ERROR)
                            wxLogError("Can't delete the thread!");
                    }
                }
                while(1)
                {
                    {
                        wxCriticalSectionLocker enter(m_engineCS);
                        if(!m_engine) break;
                    }
                    wxThread::This()->Sleep(1);
                }
                Destroy();
                return;
            }
            else
            {
                dlg->Destroy();
                return;
            }
        }
    }
    {
        wxCriticalSectionLocker enter(m_engineCS);
        if(m_engine)
        {
            if(m_engine->Delete() != wxTHREAD_NO_ERROR)
                wxLogError("Can't delete the thread!");
        }
    }
    while(1)
    {
        {
            wxCriticalSectionLocker enter(m_engineCS);
            if(!m_engine) break;
        }
        wxThread::This()->Sleep(1);
    }
    Destroy();
}

void mMainFrame::OnAbout(wxCommandEvent &/*event*/)
{
    wxString description = wxString() << wxVERSION_STRING
#ifdef __WXGTK__
                            << " (gtk " << GTKV << ")\n"
#else
                            << "\n"
#endif
                            << "LibAria2 " ARIA2V;
    wxAboutDialogInfo info;
    info.SetIcon(ICO_LOGO);
    info.SetName(PROGRAM_NAME);
    info.SetVersion(VERSION);
    info.SetDescription(description);
    info.SetCopyright("(c) 2006 ~");
    info.SetWebSite("https://wxdfast.dxsolutions.org");
    wxArrayString developers;
    developers.Add(wxString::FromUTF8("Max Magalhães Velasques <maxvelasques@gmail.com>"));
    developers.Add("David Vachulka <archdvx@dxsolutions.org>");
    info.SetDevelopers(developers);
    wxArrayString translators;
    translators.Add(wxString::FromUTF8("(AR) Faiçal Marzouq <flmarzouq@gmail.com>, Haytham Abulela Hammam <haytham.hammam@gmail.com>"));
    translators.Add("(BG) Ivan Georgiev <IVG_18@yahoo.com>");
    translators.Add("(BN) Anindita Basu <ab.techwriter@gmail.com>");
    translators.Add("(CS) David Vachulka <archdvx@dxsolutions.org>");
    translators.Add("(DE) Stefan Westermann <raskolnikow83@gmx.de>");
    translators.Add("(ES) Carlos Garces <max_mv@yahoo.com.br>");
    translators.Add(wxString::FromUTF8("(FO) Jón Joensen <jon@umseting.com>"));
    translators.Add("(FR) Martin Ledoux <martinlmtl@gmail.com>");
    translators.Add("(HE) Elisheva Hershler <elisheva@hershler.com>, Shai Nave <shai.nve@gmail.com>");
    translators.Add("(HI) Chandra Nageswaran <channage@gmail.com>");
    translators.Add("(HU) Tequila <tequila_13@users.sourceforge.net>");
    translators.Add("(IT) Angelo Contardi <Angelo.Contardi@italtel.it>");
    translators.Add("(JA) Tilt <tiltstr@gmail.com>");
    translators.Add("(KO) Choi, JiHui <like.a.dust@gmail.com>");
    translators.Add("(LV) Valters Feists <valtersf@parks.lv>");
    translators.Add("(MS) S. Fazlulhuda Akmar Binti S. M. Kamaruddin <fazlul27@yahoo.com.sg>, Nor Aini Ismam <aini_ismam2008@hotmail.com>, Siti Rusni Hanafiah <Siti.Rusni@excite.com>");
    translators.Add("(PL) Marcin Trybus <mtrybus@o2.pl>");
    translators.Add(wxString::FromUTF8("(PT_BR) Max Magalhães Velasques <maxvelasques@gmail.com>"));
    translators.Add("(RU) Ruslan Fedyarov <fedyarov@mail.ru>");
    translators.Add("(SV) Stefan Gagner <stefan@mei-ya.se>");
    translators.Add("(TR) Erhan BURHAN <eburhan@gmail.com>");
    translators.Add(wxString::FromUTF8("(UK) Serhij Dubyk (Сергій Дубик) <dubyk@library.lviv.ua>, Maxym Mykhalchuk <mihmax@gmail.com>, Sergiy Martynenko <ramarren.ua@gmail.com>"));
    translators.Add("(UR) Sohaib Ikram <sohaib_uet@yahoo.com>, Huda Sarfraz <huda.sarfraz@gmail.com>");
    info.SetTranslators(translators);
    info.AddArtist("Erno Szabados");
    info.SetLicence(_(
                "This program is free software; you can redistribute it and/or modify it under the terms of\n"
                "the GNU General Public License as published by the Free Software Foundation;\n"
                "either version 2 of the License, or (at your option) any later version.\n\n"

                "This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;\n"
                "without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
                "See the GNU General Public License for more details.\n\n"

                "You should have received a copy of the GNU General Public License along with this program;\n"
                "if not, write to the Free Software Foundation, Inc., 51 Franklin Street,\n"
                "Fifth Floor, Boston, MA 02110-1301, USA"));
    wxAboutBox(info);
}

void mMainFrame::OnSite(wxCommandEvent &/*event*/)
{
    wxLaunchDefaultBrowser("https://wxdfast.dxsolutions.org");
}

void mMainFrame::OnNew(wxCommandEvent &event)
{
    OnPasteURL(event);
}

void mMainFrame::OnRemove(wxCommandEvent &/*event*/)
{
    if(m_book->GetSelection()==0) //Progress tab
    {
        if(m_selectedProgress == -1)
            return;
        std::vector<long> selected;
        long item = m_progresslistctrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        while(item != -1)
        {
            selected.push_back(item);
            item = m_progresslistctrl->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        }
        for(auto it = selected.rbegin(); it != selected.rend();++it)
        {
            if(m_progressList[static_cast<size_t>((*it))].status() == STATUS_QUEUE || m_progressList[static_cast<size_t>((*it))].status() == STATUS_SCHEDULE_QUEUE)
            {
                RemoveFromProgressList((*it));
            }
            else
            {
                m_engine->RemoveDownload(m_progressList[static_cast<size_t>((*it))].gid());
            }
        }
        selected.clear();
        m_progresslistctrl->ClearSelection();
        UpdateProgressList();
    }
    else
    {
        if(m_selectedFinished == -1)
            return;
        wxMessageDialog dlg(this, _("Do you also want to remove downloaded file(s) from the disk?"), _("Remove..."), wxYES_NO | wxICON_QUESTION);
        int response = dlg.ShowModal();
        std::vector<long> selected;
        long item = m_finishedlistctrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        while(item != -1)
        {
            selected.push_back(item);
            item = m_finishedlistctrl->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        }
        for(auto it = selected.rbegin(); it != selected.rend(); ++it)
        {
            RemoveFromFinishedList((*it), response == wxID_OK);
        }
        selected.clear();
        m_finishedlistctrl->ClearSelection();
        UpdateFinishedList();
        m_finishedinfoctrl->Refresh();
    }
}

void mMainFrame::OnSchedule(wxCommandEvent &/*event*/)
{
    if(m_selectedProgress == -1)
        return;
    if(m_progressList[static_cast<size_t>(m_selectedProgress)].status() != STATUS_SCHEDULE_QUEUE)
    {
        if(m_progressList[static_cast<size_t>(m_selectedProgress)].status() == STATUS_ACTIVE)
        {
            m_engine->PauseDownload(m_progressList[static_cast<size_t>(m_selectedProgress)].gid());
        }
        m_progressList[static_cast<size_t>(m_selectedProgress)].setStatus(STATUS_SCHEDULE_QUEUE);
        m_progresslistctrl->RefreshItem(m_selectedProgress);
    }
    EnableTools();
}

void mMainFrame::OnStart(wxCommandEvent &/*event*/)
{
    if(m_selectedProgress == -1)
        return;
    StartDownload(m_selectedProgress);
}

void mMainFrame::OnStop(wxCommandEvent &/*event*/)
{
    if(m_selectedProgress == -1)
        return;
    PauseDownload(m_selectedProgress);
}

void mMainFrame::OnStartAll(wxCommandEvent &/*event*/)
{
    for(size_t i=0; i < m_progressList.size(); i++)
    {
        if(m_progressList[i].status() == STATUS_QUEUE)
            m_progressList[i].setStatus(STATUS_STOPED);
        StartDownload(i);
    }
}

void mMainFrame::OnStopAll(wxCommandEvent &/*event*/)
{
    for(size_t i=0; i < m_progressList.size(); i++)
    {
        PauseDownload(i);
    }
}

void mMainFrame::OnBug(wxCommandEvent &/*event*/)
{
    wxLaunchDefaultBrowser("https://github.com/archdvx/wxdfast/issues");
}

void mMainFrame::OnDonate(wxCommandEvent &/*event*/)
{
    wxLaunchDefaultBrowser("https://dxsolutions.org/donate.html");
}

void mMainFrame::OnShowGraph(wxCommandEvent &/*event*/)
{
    moptions.setGraphshow(!moptions.graphshow());
    ShowHideResizeGraph();
}

void mMainFrame::OnDetails(wxCommandEvent &/*event*/)
{
    if(m_progresssplitter->IsSplit())
    {
        moptions.setSeparator1(m_progresssplitter->GetSashPosition());
        m_progresssplitter->Unsplit();
    }
    else
    {
        m_progresssplitter->SplitHorizontally(m_progresslistctrl, m_lbook, moptions.separator1());
    }
    if(m_finishedsplitter->IsSplit())
    {
        moptions.setSeparator2(m_finishedsplitter->GetSashPosition());
        m_finishedsplitter->Unsplit();
    }
    else
    {
        m_finishedsplitter->SplitHorizontally(m_finishedlistctrl, m_finishedinfoctrl, moptions.separator2());
    }
}

void mMainFrame::OnPasteURL(wxCommandEvent &event)
{
    wxString url = "";
    if(event.GetId() == ID_MENU_PASTEURL)
    {
        if(wxTheClipboard->Open())
        {
            if(wxTheClipboard->IsSupported(wxDF_TEXT) || wxTheClipboard->IsSupported(wxDF_UNICODETEXT))
            {
                wxTextDataObject data;
                wxTheClipboard->GetData(data);
                url = data.GetText().Strip(wxString::both);
            }
            wxTheClipboard->Close();
        }
    }
#if defined (__WXMAC__)
    mNewDialog dialog(nullptr);
#else
    mNewDialog dialog(this);
#endif
    if(!url.IsEmpty())
    {
        dialog.SetUrl(url);
    }
    if(moptions.rememberboxnewoptions())
    {
        dialog.SetBandwidth(moptions.lastbandwidth());
        dialog.SetCommand(moptions.lastcommand());
        dialog.SetDestination(moptions.lastdestination());
        dialog.SetNumberOfConnections(moptions.lastnumberofconnections());
        dialog.SetOnTop(moptions.lastontopoption());
        dialog.SetStartOption(moptions.laststartoption());
        dialog.SetSize(moptions.lastx(),moptions.lasty(),-1,-1);
    }
    if(dialog.ShowModal() == wxID_OK)
    {
        mFileInfo info;
        if(dialog.GetOnTop())
        {
            info.setIndex(1);
            for(auto it=m_progressList.begin(); it!=m_progressList.end(); ++it)
            {
                (*it).setIndex((*it).index()+1);
            }
        }
        else
        {
            info.setIndex(LastIndex());
        }
        info.setGid(GenerateGID());
        info.setDestination(dialog.GetDestination());
        info.setUrls(dialog.GetUrls());
        info.setUser(dialog.GetUser());
        info.setPassword(dialog.GetPassword());
        if(dialog.GetStartOption() == NOW)
            info.setStatus(STATUS_STOPED);
        else if(dialog.GetStartOption() == MANUAL)
            info.setStatus(STATUS_QUEUE);
        else
            info.setStatus(STATUS_SCHEDULE_QUEUE);
        info.setConnections(dialog.GetNumberOfConnections());
        info.setBandwidth(dialog.GetBandwidth());
        info.setComment(dialog.GetComment());
        info.setLink(dialog.GetReferenceURL());
        info.setCommand(dialog.GetCommand());
        info.setName(dialog.GetUrls().Item(0).AfterLast('/'));
        m_progressList.push_back(info);
        std::sort(m_progressList.begin(), m_progressList.end(), SortFileInfo);
        UpdateProgressList();
        m_selectedProgress = dialog.GetOnTop()?0:m_progressList.size()-1;
        m_progresslistctrl->SetItemState(m_selectedProgress, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        if(moptions.rememberboxnewoptions())
        {
            int lastx, lasty;
            dialog.GetPosition(&lastx,&lasty);
            moptions.setLastx(lastx);
            moptions.setLasty(lasty);
            moptions.setLastbandwidth(dialog.GetBandwidth());
            moptions.setLastcommand(dialog.GetCommand());
            moptions.setLastdestination(dialog.GetDestination());
            moptions.setLastnumberofconnections(dialog.GetNumberOfConnections());
            moptions.setLastontopoption(dialog.GetOnTop());
            moptions.setLaststartoption(dialog.GetStartOption());
        }
        if(info.status() != STATUS_QUEUE)
            StartDownload(m_selectedProgress);
    }
}

void mMainFrame::OnOptions(wxCommandEvent &/*event*/)
{
    mOptionsDialog dialog(this);
    if(dialog.ShowModal() == wxID_OK)
    {
        moptions.save();
        ShowHideResizeGraph();
        moptions.setLivebandwidthoption(moptions.bandwidthoption());
        SetBandwidth();
        SetStatusBandwidth();
        m_engine->UpdateOptions();
    }
}

void mMainFrame::OnProperties(wxCommandEvent &/*event*/)
{
    if(m_book->GetSelection()==0 && m_selectedProgress != -1)
    {
        mNewDialog dialog(this);
        dialog.SetTitle(_("Download Properties"));
        mFileInfo info = m_progressList[static_cast<size_t>(m_selectedProgress)];
        dialog.SetDestination(info.destination());
        dialog.SetUrls(info.urls());
        dialog.SetUser(info.user());
        dialog.SetPassword(info.password());
        if(info.status() == STATUS_ACTIVE || info.status() == STATUS_STOPED)
            dialog.SetStartOption(NOW);
        else if(info.status() == STATUS_QUEUE)
            dialog.SetStartOption(MANUAL);
        else
            dialog.SetStartOption(SCHEDULE);
        dialog.SetNumberOfConnections(info.connections());
        dialog.SetBandwidth(info.bandwidth());
        dialog.SetComment(info.comment());
        dialog.SetReferenceURL(info.link());
        dialog.SetCommand(info.command());
        bool enable = false;
        if(info.status() == STATUS_QUEUE || info.status() == STATUS_SCHEDULE_QUEUE)
            enable = true;
        if(!enable)
        {
            dialog.EnableAdd(false);
            dialog.EnableAdresslist(false);
            dialog.EnableBandwidth(false);
            dialog.EnableCommand(false);
            dialog.EnableComment(false);
            dialog.EnableDestination(false);
            dialog.EnableDirectory(false);
            dialog.EnableEdit(false);
            dialog.EnableManual(false);
            dialog.EnableNow(false);
            dialog.EnableOntop(false);
            dialog.EnablePassword(false);
            dialog.EnableReferenceurl(false);
            dialog.EnableSchedule(false);
            dialog.EnableSplit(false);
            dialog.EnableUrl(false);
            dialog.EnableUser(false);
        }
        dialog.ReLayout();
        if(dialog.ShowModal() == wxID_OK && enable)
        {
            if(dialog.GetOnTop())
            {
                for(auto it=m_progressList.begin(); it!=m_progressList.end(); ++it)
                {
                    (*it).setIndex((*it).index()+1);
                }
                info.setIndex(1);
            }
            info.setDestination(dialog.GetDestination());
            info.setUrls(dialog.GetUrls());
            info.setUser(dialog.GetUser());
            info.setPassword(dialog.GetPassword());
            if(dialog.GetStartOption() == NOW)
                info.setStatus(STATUS_STOPED);
            else if(dialog.GetStartOption() == MANUAL)
                info.setStatus(STATUS_QUEUE);
            else
                info.setStatus(STATUS_SCHEDULE_QUEUE);
            info.setConnections(dialog.GetNumberOfConnections());
            info.setBandwidth(dialog.GetBandwidth());
            info.setComment(dialog.GetComment());
            info.setLink(dialog.GetReferenceURL());
            info.setCommand(dialog.GetCommand());
            info.setName(dialog.GetUrls().Item(0).AfterLast('/'));
            m_progressList[static_cast<size_t>(m_selectedProgress)] = info;
            std::sort(m_progressList.begin(), m_progressList.end(), SortFileInfo);
            UpdateProgressList();
            long item = 0;
            for(auto it=m_progressList.begin(); it!=m_progressList.end(); ++it)
            {
                if((*it).index() == info.index())
                {
                    m_selectedProgress = item;
                    break;
                }
                item++;
            }
            m_selectedProgress = item;
            m_progresslistctrl->SetItemState(m_selectedProgress, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
            if(info.status() != STATUS_QUEUE)
                StartDownload(m_selectedProgress);
        }
    }
}

void mMainFrame::OnRemoveAll(wxCommandEvent &/*event*/)
{
    if(m_book->GetSelection()==0) //Progress tab
    {
        for(auto it=m_progressList.begin(); it!=m_progressList.end();)
        {
            if((*it).status() == STATUS_QUEUE || (*it).status() == STATUS_SCHEDULE_QUEUE)
            {
                it = m_progressList.erase(it);
            }
            else
            {
                m_engine->RemoveDownload((*it).gid());
                ++it;
            }
        }
        UpdateProgressList();
    }
    else
    {
        wxMessageDialog dlg(this, _("Do you also want to remove downloaded file(s) from the disk?"), _("Remove..."), wxYES_NO | wxICON_QUESTION);
        int response = dlg.ShowModal();
        for(auto it=m_finishedList.begin(); it!=m_finishedList.end();)
        {
            if(response == wxID_YES)
            {
                if(wxFileExists((*it).destination()+wxFILE_SEP_PATH+(*it).name()))
                    wxRemoveFile((*it).destination()+wxFILE_SEP_PATH+(*it).name());
            }
            it = m_finishedList.erase(it);
        }
        UpdateFinishedList();
        m_finishedinfoctrl->Refresh();
    }
}

void mMainFrame::OnFind(wxCommandEvent &/*event*/)
{
#if wxUSE_FINDREPLDLG
    if(m_dlgFind)
    {
        wxDELETE(m_dlgFind);
    }
    else
    {
        if(m_book->GetSelection()==0) //Progress tab
        {
            m_progresslistctrl->ClearSelection();
            m_selectedProgress = -1;
        }
        else
        {
            m_finishedlistctrl->ClearSelection();
            m_selectedFinished = -1;
        }
        wxFindReplaceData *data = new wxFindReplaceData();
        data->SetFlags(wxFR_DOWN);
        m_dlgFind = new wxFindReplaceDialog(this, data, _("Find file..."));
        m_dlgFind->Show(true);
    }
#endif // wxUSE_FINDREPLDLG
}

void mMainFrame::OnDownloadAgain(wxCommandEvent &/*event*/)
{
    if(m_book->GetSelection() == 1 && m_selectedFinished != -1) //Finished
    {
        mFileInfo info = m_finishedList[static_cast<size_t>(m_selectedFinished)];
        info.setIndex(LastIndex());
        info.setGid(GenerateGID());
        info.setTotalLength(0);
        info.setNumPieces(0);
        info.clearTimepassed();
        info.clearLog();
        info.setStatus(STATUS_STOPED);
        m_progressList.push_back(info);
        std::sort(m_progressList.begin(), m_progressList.end(), SortFileInfo);
        m_engine->AddFileInfo(info);
        UpdateProgressList();
        m_selectedProgress = m_progressList.size()-1;
        m_progresslistctrl->SetItemState(m_selectedProgress, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        StartDownload(m_selectedProgress);
    }
}

void mMainFrame::OnMove(wxCommandEvent &/*event*/)
{
    if(m_selectedFinished == -1)
        return;
    wxString dir = wxDirSelector(_("Select the directory:"));
    if(!dir.IsEmpty())
    {
        if(dir.Right(1) != wxFILE_SEP_PATH)
            dir << wxFILE_SEP_PATH;
        wxString destination = m_finishedList[static_cast<size_t>(m_selectedFinished)].destination();
        if(destination.Right(1) != wxFILE_SEP_PATH)
            destination << wxFILE_SEP_PATH;
        if(destination != dir)
        {
            if(wxFileExists(m_finishedList[static_cast<size_t>(m_selectedFinished)].destination() + wxFILE_SEP_PATH + m_finishedList[static_cast<size_t>(m_selectedFinished)].name()))
            {
                wxProgressDialog dlg(_("Moving..."),_("Moving file..."));
                if(wxCopyFile(destination+m_finishedList[static_cast<size_t>(m_selectedFinished)].name(), dir+m_finishedList[static_cast<size_t>(m_selectedFinished)].name(), true))
                {
                    dlg.Update(50);
                    m_finishedList[static_cast<size_t>(m_selectedFinished)].setDestination(dir.BeforeLast(wxFILE_SEP_PATH));
                    wxRemoveFile(destination+m_finishedList[static_cast<size_t>(m_selectedFinished)].name());
                }
                else
                {
                    wxMessageBox(_("Error moving file."), _("Error..."), wxOK|wxICON_ERROR, this);
                }
                dlg.Update(100);
            }
            else
            {
                if(wxMessageBox(_("The file isn't in the path that it was saved initially.\nDo you want to change only the old path for the current one?")+"\n"+_("Filename:")+" "
                                +m_finishedList[static_cast<size_t>(m_selectedFinished)].name(),
                                _("Continue..."), wxYES|wxNO|wxICON_QUESTION, this) == wxYES)
                {
                    m_finishedList[static_cast<size_t>(m_selectedFinished)].setDestination(dir.BeforeLast(wxFILE_SEP_PATH));
                }
            }
        }
        m_finishedinfoctrl->Refresh();
    }
}

void mMainFrame::OnCheckMD5(wxCommandEvent &/*event*/)
{
    if(m_selectedFinished == -1)
        return;
    wxString md5old, md5new;
    md5old = m_finishedList[static_cast<size_t>(m_selectedFinished)].MD5();
    if(wxFileExists(m_finishedList[static_cast<size_t>(m_selectedFinished)].destination() + wxFILE_SEP_PATH + m_finishedList[static_cast<size_t>(m_selectedFinished)].name()))
    {
        wxProgressDialog waitbox("MD5", _("Checking file's MD5..."));
        waitbox.Update(10);
        wxFileName filemd5 = wxFileName(m_finishedList[static_cast<size_t>(m_selectedFinished)].destination() + wxFILE_SEP_PATH + m_finishedList[static_cast<size_t>(m_selectedFinished)].name());
        wxMD5 md5(filemd5);
        md5new = md5.GetDigest(true); //TELL TO GETDIGETS THAT IS THE MAIN THREAD
        waitbox.Update(100);
        if(md5new == md5old)
        {
            wxMessageBox(_("The file was verified successfully."), _("Success..."), wxOK | wxICON_INFORMATION, this);
        }
        else
        {
            wxString msg;
            msg << _("The MD5 calculated previously is different from the current one.");
            msg << _("\nOld MD5 =\t\t") + md5old;
            msg << _("\nCurrent MD5 =\t") + md5new;
            wxMessageBox(msg, _("Error..."), wxOK | wxICON_ERROR, this);
        }
    }
    else
    {
        wxMessageBox(_("File not found."), _("Error..."), wxOK | wxICON_ERROR, this);
    }
}

void mMainFrame::OnOpenDestination(wxCommandEvent &/*event*/)
{
    if(m_selectedFinished == -1)
        return;
    if(!wxDirExists(m_finishedList[static_cast<size_t>(m_selectedFinished)].destination()))
    {
        wxMessageBox(wxString::Format(_("Folder %s doesn't exist."),m_finishedList[static_cast<size_t>(m_selectedFinished)].destination()), _("Error..."), wxOK|wxICON_ERROR, this);
        return;
    }
#if defined (__WXGTK__)
    wxExecute(wxString::Format("xdg-open %s", m_finishedList[static_cast<size_t>(m_selectedFinished)].destination()));
#endif
#if defined (__WXMAC__)
    wxExecute(wxString::Format("open %s", m_finishedList[static_cast<size_t>(m_selectedFinished)].destination()));
#endif
#if defined (__WXMSW__)
    wxExecute(wxString::Format("explorer %s", m_finishedList[static_cast<size_t>(m_selectedFinished)].destination()));
#endif
}

void mMainFrame::OnCopyURL(wxCommandEvent &/*event*/)
{
    if(m_book->GetSelection()==0) //Progress tab
    {
        if(m_selectedProgress== -1)
            return;
        if(wxTheClipboard->Open())
        {
            wxString info = "";
            if(m_progressList[static_cast<size_t>(m_selectedProgress)].urls().GetCount())
                info << m_progressList[static_cast<size_t>(m_selectedProgress)].urls().Item(0);
            wxTheClipboard->SetData(new wxTextDataObject(info));
            wxTheClipboard->Close();
        }
        else
        {
            wxMessageBox(_("Unable to open the clipboard!"),_("Error...") ,wxOK | wxICON_ERROR,this);
        }
    }
    else
    {
        if(m_selectedFinished == -1)
            return;
        if(wxTheClipboard->Open())
        {
            wxString info = "";
            if(m_finishedList[static_cast<size_t>(m_selectedFinished)].urls().GetCount())
                info << m_finishedList[static_cast<size_t>(m_selectedFinished)].urls().Item(0);
            wxTheClipboard->SetData(new wxTextDataObject(info));
            wxTheClipboard->Close();
        }
        else
        {
            wxMessageBox(_("Unable to open the clipboard!"),_("Error...") ,wxOK | wxICON_ERROR,this);
        }
    }
}

void mMainFrame::OnCopyDownloadData(wxCommandEvent &/*event*/)
{
    if(m_book->GetSelection()==0) //Progress tab
    {
        if(m_selectedProgress== -1)
            return;
        if(wxTheClipboard->Open())
        {
            wxString info;
            info << _("Name") << ": " << m_progressList[static_cast<size_t>(m_selectedProgress)].name() << "\n";
            info << _("File type") << ": " << MyUtilFunctions::MimeType(m_progressList[static_cast<size_t>(m_selectedProgress)].name()) << "\n";
            info << _("Size") << ": " << MyUtilFunctions::SizeText(m_progressList[static_cast<size_t>(m_selectedProgress)].totalLength()) << "\n";
            info << _("Time") << ": " << MyUtilFunctions::TimespanToWxstr(m_progressList[static_cast<size_t>(m_selectedProgress)].timepassed()) << "\n";
            info << _("Destination") << ": " << m_progressList[static_cast<size_t>(m_selectedProgress)].destination() << "\n";
            info << _("Reference URL") << ": " << m_progressList[static_cast<size_t>(m_selectedProgress)].link() << "\n";
            info << _("Comments") << ": " << m_progressList[static_cast<size_t>(m_selectedProgress)].comment() << "\n";
            info << _("URLs") << ": " << MyUtilFunctions::ArrayStringTowxString(m_progressList[static_cast<size_t>(m_selectedProgress)].urls()) << "\n";
            wxTheClipboard->SetData(new wxTextDataObject(info));
            wxTheClipboard->Close();
        }
        else
        {
            wxMessageBox(_("Unable to open the clipboard!"),_("Error...") ,wxOK | wxICON_ERROR,this);
        }
    }
    else
    {
        if(m_selectedFinished == -1)
            return;
        if(wxTheClipboard->Open())
        {
            wxString info;
            info << _("Name") << ": " << m_finishedList[static_cast<size_t>(m_selectedFinished)].name() << "\n";
            info << _("File type") << ": " << MyUtilFunctions::MimeType(m_finishedList[static_cast<size_t>(m_selectedFinished)].name()) << "\n";
            info << _("Size") << ": " << MyUtilFunctions::SizeText(m_finishedList[static_cast<size_t>(m_selectedFinished)].totalLength()) << "\n";
            info << _("Time") << ": " << MyUtilFunctions::TimespanToWxstr(m_finishedList[static_cast<size_t>(m_selectedFinished)].timepassed()) << "\n";
            info << _("Destination") << ": " << m_finishedList[static_cast<size_t>(m_selectedFinished)].destination() << "\n";
            info << _("MD5") << ": " << m_finishedList[static_cast<size_t>(m_selectedFinished)].MD5() << "\n";
            info << _("Reference URL") << ": " << m_finishedList[static_cast<size_t>(m_selectedFinished)].link() << "\n";
            info << _("Comments") << ": " << m_finishedList[static_cast<size_t>(m_selectedFinished)].comment() << "\n";
            info << _("URLs") << ": " << MyUtilFunctions::ArrayStringTowxString(m_finishedList[static_cast<size_t>(m_selectedFinished)].urls()) << "\n";
            wxTheClipboard->SetData(new wxTextDataObject(info));
            wxTheClipboard->Close();
        }
        else
        {
            wxMessageBox(_("Unable to open the clipboard!"),_("Error...") ,wxOK | wxICON_ERROR,this);
        }
    }
}

void mMainFrame::OnExportConf(wxCommandEvent &/*event*/)
{
    wxString dir = wxDirSelector(_("Select the directory:"));
    if(!dir.IsEmpty())
    {
        if(ExportConf(dir))
            wxMessageBox(_("The configuration was exported successfully."), _("Success..."), wxOK|wxICON_INFORMATION, this);
        else
            wxMessageBox(_("Error exporting configuration."), _("Error..."), wxOK|wxICON_ERROR, this);
    }
}

void mMainFrame::OnImportConf(wxCommandEvent &/*event*/)
{
    wxFileDialog dialog(this, _("Select the file..."), wxEmptyString, wxEmptyString, wxT("*.ini"));
    if(dialog.ShowModal() == wxID_OK)
    {
        //Save current
        moptions.save();
        SaveProgressList();
        SaveFinishedList();
        //Remove all progress downloads
        if(m_progressList.size())
        {
            for(auto it=m_progressList.begin(); it!=m_progressList.end();)
            {
                m_engine->RemoveDownload((*it).gid());
                if((*it).status() == STATUS_QUEUE || (*it).status() == STATUS_SCHEDULE_QUEUE)
                {
                    it = m_progressList.erase(it);
                }
                else
                {
                    ++it;
                }
            }
            UpdateProgressList();
            UpdateDetail();
        }
        //Remove all finihed downloads
        if(m_finishedList.size())
        {
            wxMessageDialog dlg(this, _("Do you also want to remove downloaded file(s) from the disk?"), _("Remove..."), wxYES_NO | wxICON_QUESTION);
            int response = dlg.ShowModal();
            for(auto it=m_finishedList.begin(); it!=m_finishedList.end();)
            {
                if(response == wxID_YES)
                {
                    if(wxFileExists((*it).destination()+wxFILE_SEP_PATH+(*it).name()))
                        wxRemoveFile((*it).destination()+wxFILE_SEP_PATH+(*it).name());
                }
                it = m_finishedList.erase(it);
            }
            UpdateFinishedList();
            m_finishedinfoctrl->Refresh();
        }
        //Load selected
        moptions.load(dialog.GetPath());
        ShowHideResizeGraph();
        moptions.setLivebandwidthoption(moptions.bandwidthoption());
        SetBandwidth();
        SetStatusBandwidth();
        if(m_progresssplitter->IsSplit())
        {
            m_progresssplitter->Unsplit();
            m_progresssplitter->SplitHorizontally(m_progresslistctrl, m_lbook, moptions.separator1());
        }
        if(m_finishedsplitter->IsSplit())
        {
            m_finishedsplitter->Unsplit();
            m_finishedsplitter->SplitHorizontally(m_finishedlistctrl, m_finishedinfoctrl, moptions.separator2());
        }
        ReadProgressList(wxFileName(dialog.GetPath()).GetPath());
        ReadFinishedList(wxFileName(dialog.GetPath()).GetPath());
        for(auto it=m_progressList.begin(); it!=m_progressList.end(); ++it)
        {
            if((*it).status() == STATUS_ACTIVE)
            {
                m_engine->AddFileInfo(*it); //AddFileInfo imadietelly start download
                (*it).appendLogLine(_("Download started")+" "+wxDateTime::Now().Format("%x %X"));
                UpdateDetail();
            }
        }
        UpdateProgressList();
        UpdateFinishedList();
        EnableTools();
    }
}

void mMainFrame::OnShutdown(wxCommandEvent &event)
{
    if(event.IsChecked())
    {
        moptions.setShutdown(true);
        wxMessageBox(_("Command :\t" ) + moptions.shutdowncmd(), _("Shutdown Command..."), wxOK|wxICON_INFORMATION, this);
    }
    else
    {
        moptions.setShutdown(false);
    }
}

void mMainFrame::OnDisconnect(wxCommandEvent &event)
{
    if(event.IsChecked())
    {
        moptions.setDisconnect(true);
        wxMessageBox(_("Command :\t" ) + moptions.disconnectcmd(), _("Disconnect Command..."), wxOK|wxICON_INFORMATION, this);
    }
    else
    {
        moptions.setDisconnect(false);
    }
}

void mMainFrame::OnUp(wxCommandEvent &/*event*/)
{
    if(m_selectedProgress == -1)
        return;
    if(m_progressList.size() < 2)
        return;
    int upindex;
    if(m_selectedProgress > 0)
    {
        int selectedindex = m_progressList[static_cast<size_t>(m_selectedProgress)].index();
        upindex = m_progressList[static_cast<size_t>(m_selectedProgress-1)].index();
        m_progressList[static_cast<size_t>(m_selectedProgress)].setIndex(upindex);
        m_progressList[static_cast<size_t>(m_selectedProgress-1)].setIndex(selectedindex);
        std::sort(m_progressList.begin(), m_progressList.end(), SortFileInfo);
        m_progresslistctrl->RefreshItem(m_selectedProgress);
        m_progresslistctrl->RefreshItem(m_selectedProgress-1);
    }
    else
    {
        int selectedindex = m_progressList[static_cast<size_t>(m_selectedProgress)].index();
        upindex = m_progressList[m_progressList.size()-1].index();
        m_progressList[static_cast<size_t>(m_selectedProgress)].setIndex(upindex);
        m_progressList[m_progressList.size()-1].setIndex(selectedindex);
        std::sort(m_progressList.begin(), m_progressList.end(), SortFileInfo);
        m_progresslistctrl->RefreshItem(m_selectedProgress);
        m_progresslistctrl->RefreshItem(m_progressList.size()-1);
    }
    SelectProgressByIndex(upindex);
}

void mMainFrame::OnDown(wxCommandEvent &/*event*/)
{
    if(m_selectedProgress == -1)
        return;
    if(m_progressList.size() < 2)
        return;
    int downindex;
    if(static_cast<size_t>(m_selectedProgress) == m_progressList.size()-1)
    {
        int selectedindex = m_progressList[static_cast<size_t>(m_selectedProgress)].index();
        downindex = m_progressList[0].index();
        m_progressList[static_cast<size_t>(m_selectedProgress)].setIndex(downindex);
        m_progressList[0].setIndex(selectedindex);
        std::sort(m_progressList.begin(), m_progressList.end(), SortFileInfo);
        m_progresslistctrl->RefreshItem(m_selectedProgress);
        m_progresslistctrl->RefreshItem(0);
    }
    else
    {
        int selectedindex = m_progressList[static_cast<size_t>(m_selectedProgress)].index();
        downindex = m_progressList[static_cast<size_t>(m_selectedProgress+1)].index();
        m_progressList[static_cast<size_t>(m_selectedProgress)].setIndex(downindex);
        m_progressList[static_cast<size_t>(m_selectedProgress+1)].setIndex(selectedindex);
        std::sort(m_progressList.begin(), m_progressList.end(), SortFileInfo);
        m_progresslistctrl->RefreshItem(m_selectedProgress);
        m_progresslistctrl->RefreshItem(m_selectedProgress+1);
    }
    SelectProgressByIndex(downindex);
}

void mMainFrame::OnProgressRightClick(wxListEvent &event)
{
    m_selectedProgress = event.GetIndex();
    EnableTools();
    UpdateDetail();
    if(m_selectedProgress== -1)
        return;
    wxMenu popup;
    wxMenuItem *itemnew = new wxMenuItem(&popup, ID_MENU_NEW, _("New Download"));
    itemnew->SetBitmap(ICO_NEW);
    popup.Append(itemnew);
    popup.AppendSeparator();
    wxMenuItem *itemremove = new wxMenuItem(&popup, ID_MENU_REMOVE, _("Remove Download"));
    itemremove->SetBitmap(ICO_REMOVE);
    popup.Append(itemremove);
    popup.AppendSeparator();
    if(m_progressList[static_cast<size_t>(m_selectedProgress)].status() != STATUS_SCHEDULE_QUEUE
            && m_progressList[static_cast<size_t>(m_selectedProgress)].status() != STATUS_SCHEDULE_ACTIVE)
    {
        wxMenuItem *itemschedule = new wxMenuItem(&popup, ID_MENU_SCHEDULE, _("Schedule Download"));
        itemschedule->SetBitmap(ICO_SCHEDULE);
        popup.Append(itemschedule);
    }
    if(m_progressList[static_cast<size_t>(m_selectedProgress)].status() != STATUS_ACTIVE
            && m_progressList[static_cast<size_t>(m_selectedProgress)].status() != STATUS_SCHEDULE_QUEUE
            && m_progressList[static_cast<size_t>(m_selectedProgress)].status() != STATUS_SCHEDULE_ACTIVE)
    {
        wxMenuItem *itemstart = new wxMenuItem(&popup, ID_MENU_START, _("Start Download"));
        itemstart->SetBitmap(ICO_START);
        popup.Append(itemstart);
    }
    if(m_progressList[static_cast<size_t>(m_selectedProgress)].status() == STATUS_ACTIVE)
    {
        wxMenuItem *itemstop = new wxMenuItem(&popup, ID_MENU_STOP, _("Stop Download"));
        itemstop->SetBitmap(ICO_STOP);
        popup.Append(itemstop);
    }
    popup.AppendSeparator();
    wxMenuItem *iteminfo = new wxMenuItem(&popup, ID_MENU_INFO, _("Download Properties"));
    iteminfo->SetBitmap(ICO_PROPERTIES);
    popup.Append(iteminfo);
    popup.AppendSeparator();
    wxMenuItem *itemurl = new wxMenuItem(&popup, ID_MENU_COPYURL, _("Copy URL"));
    itemurl->SetBitmap(ICO_COPYURL);
    popup.Append(itemurl);
    wxMenuItem *itemdata = new wxMenuItem(&popup, ID_MENU_COPYDOWNLOADDATA, _("Copy Download Data"));
    itemdata->SetBitmap(ICO_COPYDATA);
    popup.Append(itemdata);
    m_progresslistctrl->PopupMenu(&popup);
}

void mMainFrame::OnProgressActivated(wxListEvent &event)
{
    m_selectedProgress = event.GetIndex();
    EnableTools();
    UpdateDetail();
    if(m_progressList[static_cast<size_t>(m_selectedProgress)].status() == STATUS_QUEUE || m_progressList[static_cast<size_t>(m_selectedProgress)].status() == STATUS_STOPED)
    {
        StartDownload(m_selectedProgress);
        return;
    }
    if(m_progressList[static_cast<size_t>(m_selectedProgress)].status() == STATUS_ACTIVE)
    {
        PauseDownload(m_selectedProgress);
        return;
    }
}

void mMainFrame::OnProgressSelected(wxListEvent &event)
{
    m_selectedProgress = event.GetIndex();
    EnableTools();
    UpdateDetail();
}

void mMainFrame::OnProgressDeselected(wxListEvent &/*event*/)
{
    m_selectedProgress = -1;
    EnableTools();
    UpdateDetail();
}

void mMainFrame::OnFinishedRightClick(wxListEvent &event)
{
    m_selectedFinished = event.GetIndex();
    EnableTools();
    m_finishedinfoctrl->Refresh();
    if(m_selectedFinished == -1)
        return;
    wxMenu popup;
    wxMenuItem *itemremove = new wxMenuItem(&popup, ID_MENU_REMOVE, _("Remove Download"));
    itemremove->SetBitmap(ICO_REMOVE);
    popup.Append(itemremove);
    popup.AppendSeparator();
    wxMenuItem *itemurl = new wxMenuItem(&popup, ID_MENU_COPYURL, _("Copy URL"));
    itemurl->SetBitmap(ICO_COPYURL);
    popup.Append(itemurl);
    wxMenuItem *itemdata = new wxMenuItem(&popup, ID_MENU_COPYDOWNLOADDATA, _("Copy Download Data"));
    itemdata->SetBitmap(ICO_COPYDATA);
    popup.Append(itemdata);
    popup.AppendSeparator();
    popup.Append(ID_MENU_MOVE, _("Move File"));
    popup.AppendSeparator();
#if !(defined (__WXMAC__)) //TODO fix on next release
    popup.Append(ID_MENU_MD5, _("Check Integrity"));
#endif
    popup.Append(ID_MENU_OPENDESTINATION, _("Open Destination Directory"));
    popup.Append(ID_MENU_AGAIN, _("Download File Again"));
    m_finishedlistctrl->PopupMenu(&popup);
}

void mMainFrame::OnFinishedActivated(wxListEvent &event)
{
    m_selectedFinished = event.GetIndex();
    EnableTools();
    m_finishedinfoctrl->Refresh();
}

void mMainFrame::OnFinishedSelected(wxListEvent &event)
{
    m_selectedFinished = event.GetIndex();
    EnableTools();
    m_finishedinfoctrl->Refresh();
}

void mMainFrame::OnFinishedDeselected(wxListEvent &/*event*/)
{
    m_selectedFinished = -1;
    EnableTools();
    m_finishedinfoctrl->Refresh();
}

void mMainFrame::OnPageChanged(wxBookCtrlEvent &/*event*/)
{
    EnableTools();
}

void mMainFrame::OnGlobalStat(wxThreadEvent &event)
{
    m_graph->AddDownloadPoint(static_cast<GlobalEvent&>(event).GetDownloadSpeed());
    m_graph->AddUploadPoint(static_cast<GlobalEvent&>(event).GetUploadSpeed());
    m_graph->Refresh();
    SetStatusSpeed(static_cast<GlobalEvent&>(event).GetDownloadSpeed(), static_cast<GlobalEvent&>(event).GetUploadSpeed());
}

void mMainFrame::OnDownloadStat(wxThreadEvent &event)
{
    DownloadEvent de = static_cast<DownloadEvent&>(event);
    bool exists = false; //download from metalink or torrent
    long item = 0;
    for(auto it=m_progressList.begin(); it!=m_progressList.end(); ++it)
    {
        if(de.GetGid() == (*it).gid())
        {
            exists = true;
            if(!(*it).totalLength() || !(*it).numPieces())
            {
                (*it).setName(m_engine->RealFileName((*it).gid()));
                if(de.GetTotalLegth())
                    (*it).setTotalLength(de.GetTotalLegth());
                if(de.GetNumPieces())
                    (*it).setNumPieces(de.GetNumPieces());
                LogAfterFirstStart(item);
            }
            (*it).setStatusFromAriastatus(de.GetStatus());
            (*it).setDownloadLength(de.GetCompletedLegth());
            (*it).setDownloadSpeed(de.GetDownloadSpeed());
            (*it).setBitfield(de.GetBitfield());
            if(de.GetStatus() == aria2::DOWNLOAD_WAITING)
            {
                (*it).appendLogLine(_("Download waiting")+" "+wxDateTime::Now().Format("%x %X"));
                UpdateDetail();
            }
            break;
        }
        item++;
    }
    if(!exists)
    {
        mFileInfo info = m_engine->FillInfo(de.GetGid());
        if(info.gid())
        {
            info.setIndex(1);
            info.setStartTime(wxDateTime::Now());
            info.setEndTime(wxDateTime::Now());
            info.setStatus(STATUS_ACTIVE);
            info.appendLogLine(_("Download started")+" "+info.startTime().Format("%x %X"));
            UpdateDetail();
            for(auto it=m_progressList.begin(); it!=m_progressList.end(); ++it)
            {
                (*it).setIndex((*it).index()+1);
            }
            m_progressList.push_back(info);
            std::sort(m_progressList.begin(), m_progressList.end(), SortFileInfo);
            LogAfterFirstStart(0);
            UpdateProgressList();
        }
    }
}

void mMainFrame::OnCheckVersion(wxThreadEvent &/*event*/)
{
    wxFile fversion;
    if(wxFileExists(wxStandardPaths::Get().GetUserDataDir()+wxFILE_SEP_PATH+"wxdfast-version") && fversion.Open(wxStandardPaths::Get().GetUserDataDir()+wxFILE_SEP_PATH+"wxdfast-version"))
    {
        wxString text;
        fversion.ReadAll(&text);
        int newversion =0, oldversion = 0;
        wxStringTokenizer newtoken(text.BeforeFirst('\n'), ".");
        if(newtoken.CountTokens() == 3)
        {
            int i=0;
            long a = 0;
            while(newtoken.HasMoreTokens())
            {
                wxString token = newtoken.GetNextToken();
                switch(i) {
                case 0: token.ToLong(&a); newversion += a*1000; break;
                case 1: token.ToLong(&a); newversion += a*10; break;
                case 2: token.ToLong(&a); newversion += a; break;
                default: break;
                }
                i++;
            }
        }
        wxStringTokenizer oldtoken(VERSION, ".");
        if(oldtoken.CountTokens() == 3)
        {
            int i=0;
            long a = 0;
            while(oldtoken.HasMoreTokens())
            {
                wxString token = oldtoken.GetNextToken();
                switch(i) {
                case 0: token.ToLong(&a); oldversion += a*1000; break;
                case 1: token.ToLong(&a); oldversion += a*10; break;
                case 2: token.ToLong(&a); oldversion += a; break;
                default: break;
                }
                i++;
            }
        }
        fversion.Close();
        wxRemoveFile(wxStandardPaths::Get().GetUserDataDir()+wxFILE_SEP_PATH+"wxdfast-version");
        if(newversion > oldversion)
        {
#if wxUSE_STATUSBAR
            SetStatusText(wxString::Format(_("NEW RELEASE: wxDownload Fast %s is available."), text));
#endif // wxUSE_STATUSBAR
            wxMessageBox(wxString::Format(_("wxDownload Fast %s is available.\nVisit https://wxdfast.dxsolutions.org for more informations."), text));
        }
        moptions.setLastnewreleasecheck(wxDateTime::Now());
    }
}

void mMainFrame::OnDownloadRemoved(wxThreadEvent &event)
{
    DownloadEvent de = static_cast<DownloadEvent&>(event);
    long item = 0;
    for(auto it=m_progressList.begin(); it!=m_progressList.end(); ++it)
    {
        if(de.GetGid() == (*it).gid())
        {
            if(wxFileExists((*it).destination()+wxFILE_SEP_PATH+(*it).name()))
                wxRemoveFile((*it).destination()+wxFILE_SEP_PATH+(*it).name());
            if(wxFileExists((*it).destination()+wxFILE_SEP_PATH+(*it).name()+".aria2"))
                wxRemoveFile((*it).destination()+wxFILE_SEP_PATH+(*it).name()+".aria2");
            m_progressList.erase(it);
            UpdateProgressList();
            break;
        }
        item++;
    }
    if(m_selectedProgress != -1)
    {
        item = m_selectedProgress;
        m_progress->setBitfield(m_progressList[static_cast<size_t>(item)].bitfield());
        m_progress->setNumpieces(m_progressList[static_cast<size_t>(item)].numPieces());
    }
    else
    {
        m_progress->setNumpieces(0);
    }
    m_progress->Refresh();
}

void mMainFrame::OnEngineStarted(wxThreadEvent &/*event*/)
{
    ReadProgressList();
    ReadFinishedList();
    for(auto it=m_progressList.begin(); it!=m_progressList.end(); ++it)
    {
        if((*it).status() == STATUS_ACTIVE)
        {
            m_engine->AddFileInfo(*it); //AddFileInfo imadietelly start download
            (*it).appendLogLine(_("Download started")+" "+wxDateTime::Now().Format("%x %X"));
            UpdateDetail();
        }
    }
    UpdateProgressList();
    UpdateFinishedList();
    EnableTools();
    if(moptions.checkforupdates() && (wxDateTime::Now()-moptions.lastnewreleasecheck()).GetDays()>9)
    {
        if(wxFileExists(wxStandardPaths::Get().GetUserDataDir()+wxFILE_SEP_PATH+"wxdfast-version"))
        {
            wxRemoveFile(wxStandardPaths::Get().GetUserDataDir()+wxFILE_SEP_PATH+"wxdfast-version");
        }
        m_engine->CheckVersion();
    }
}

void mMainFrame::OnTimer(wxTimerEvent &/*event*/)
{
    long item = 0;
    for(auto it=m_progressList.begin(); it!=m_progressList.end();)
    {
        if((*it).status() == STATUS_ACTIVE || (*it).status() == STATUS_SCHEDULE_ACTIVE)
            (*it).addTimepassed(moptions.timerupdateinterval());
        if((*it).status() == STATUS_FINISHED || (*it).status() == STATUS_ERROR) //Move to finish list
        {
            (*it).addTimepassed(moptions.timerupdateinterval());
            (*it).setEndTime(wxDateTime::Now());
            (*it).setIndex(m_finishedList.size());
#if !(defined (__WXMAC__)) //TODO fix on next release
            if(frameShown)
            {
                wxProgressDialog waitbox("MD5", _("Calculating file's MD5..."));
                waitbox.Update(10);
                (*it).setMD5(wxMD5::GetDigest(wxFileName((*it).destination() + wxFILE_SEP_PATH + (*it).name())));
                waitbox.Update(100);
            }
            else
            {
                (*it).setMD5(wxMD5::GetDigest(wxFileName((*it).destination() + wxFILE_SEP_PATH + (*it).name())));
            }
#endif
            if(moptions.shownotify())
            {
                if((*it).status() == STATUS_FINISHED && (*it).downloadLength() >= (*it).totalLength())
                {
                    ShowNotify((*it).name(), _("Downloaded successfully"));
                }
                if((*it).status() == STATUS_ERROR)
                {
                    ShowNotify((*it).name(), _("Downloaded ended with error"));
                }
            }
            (*it).clearLog();
            if(!(*it).command().IsEmpty())
            {
                wxExecute((*it).command());
            }
            m_finishedList.push_back((*it));
            UpdateFinishedList();
            it = m_progressList.erase(it);
            UpdateProgressList();
            UpdateDetail();
            if(moptions.restoremainframe() && !frameShown && !SomeProcessOnDownload())
            {
                frameShown = true;
                frame->Iconize(false);
                frame->SetFocus();
                frame->Raise();
                frame->Show(true);
            }
            ShutDownOrDisconnect();
            continue;
        }
        if((*it).status() == STATUS_ACTIVE && (*it).timepassed()/1000 != 0 && ((*it).timepassed()/1000)%90 == 0
                && (*it).timepassed()/1000 != (*it).laststat()/1000)
        {
            (*it).appendLogLine(_("Download statistics")+" "+wxDateTime::Now().Format("%x %X"));
            (*it).appendLogLine(wxT("   ")+_("Completed")+": "+MyUtilFunctions::SizeText((*it).downloadLength()));
            (*it).appendLogLine(wxT("   ")+_("Percentage")+": "+((*it).totalLength()?wxString::Format("%.1f%%", (100.*(*it).downloadLength())/((*it).totalLength()/1.)):"0%"));
            (*it).appendLogLine(wxT("   ")+_("Time Passed")+": "+MyUtilFunctions::TimespanToWxstr((*it).timepassed()));
            (*it).appendLogLine(wxT("   ")+_("Remaining")+": "
                                +((*it).downloadSpeed()?wxTimeSpan(0,0,((*it).totalLength()-(*it).downloadLength())/(*it).downloadSpeed()).Format():wxString::FromUTF8("∞")));
            (*it).setLaststat((*it).timepassed());
            if(m_selectedProgress == item)
                UpdateDetail();
        }
        if((*it).status() == STATUS_STOPED)
        {
            StartDownload(item);
        }
        if(CurrentSimultaneous() < moptions.simultaneous() && (*it).status() == STATUS_SCHEDULE_QUEUE && moptions.IsTimeForSchedule())
        {
            if(m_engine->HasGid((*it).gid()))
                m_engine->ResumeDownload((*it).gid(), (*it).destination(), (*it).connections(), (*it).bandwidth());
            else
                m_engine->AddFileInfo((*it));
            (*it).setStatus(STATUS_SCHEDULE_ACTIVE);
            EnableTools();
            (*it).appendLogLine(_("Download started")+" "+wxDateTime::Now().Format("%x %X"));
            UpdateDetail();
        }
        if((*it).status() == STATUS_SCHEDULE_ACTIVE && !moptions.IsTimeForSchedule())
        {
            m_engine->PauseDownload((*it).gid());
            (*it).setStatus(STATUS_SCHEDULE_QUEUE);
            EnableTools();
            (*it).appendLogLine(_("Download paused")+" "+wxDateTime::Now().Format("%x %X"));
            UpdateDetail();
        }
        m_progresslistctrl->RefreshItem(item);
        item++;
        ++it;
    }
    if(m_selectedProgress != -1)
    {
        item = m_selectedProgress;
        m_progress->setBitfield(m_progressList[static_cast<size_t>(item)].bitfield());
        m_progress->setNumpieces(m_progressList[static_cast<size_t>(item)].numPieces());
    }
    else
    {
        m_progress->setNumpieces(0);
    }
    m_progress->Refresh();
    if(wxDateTime::Now().GetSecond()%2)
    {
        m_taskbaricon->SetIcon(ICO_TRAY,GetTaskTooltip());
    }
    if(m_timer.GetInterval() != moptions.timerupdateinterval())
    {
        m_timer.Stop();
        m_timer.Start(moptions.timerupdateinterval());
    }
}

void mMainFrame::OnBandUnlimited(wxCommandEvent &/*event*/)
{
    moptions.setLivebandwidthoption(0);
    SetBandwidth();
    SetStatusBandwidth();
}

void mMainFrame::OnBandControlOn(wxCommandEvent &/*event*/)
{
    moptions.setLivebandwidthoption(2);
    SetBandwidth();
    SetStatusBandwidth();
}

void mMainFrame::OnBandControlPerDownload(wxCommandEvent &/*event*/)
{
    moptions.setLivebandwidthoption(1);
    SetBandwidth();
    SetStatusBandwidth();
}

void mMainFrame::OnServerEvent(wxSocketEvent &/*event*/)
{
    wxSocketBase *sock = m_server->Accept(false);
    if(sock)
    {
#ifdef wxHAS_UNIX_DOMAIN_SOCKETS
        wxLogDebug("New connection from client accepted.");
#else
        wxIPV4address addr;
        if(!sock->GetPeer(addr))
        {
            wxLogDebug("New connection from unknown client accepted.");
        }
        else
        {
            wxLogDebug("New client connection from %s:%u accepted", addr.IPAddress(), addr.Service());
        }
#endif
    }
    else
    {
        wxLogMessage(_("Error: couldn't accept a new connection"));
        return;
    }
    sock->SetEventHandler(*this, ID_IPC_SOCKET);
    sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
    sock->Notify(true);
}

void mMainFrame::OnSocketEvent(wxSocketEvent &event)
{
    wxSocketBase *sock = event.GetSocket();
    switch(event.GetSocketEvent())
    {
        case wxSOCKET_INPUT:
        {
            sock->SetNotify(wxSOCKET_LOST_FLAG);
            char tmpbuf[MAX_BUFF_LEN];
            sock->Read(tmpbuf, sizeof(tmpbuf) - 1);
            tmpbuf[sock->LastReadCount()] = '\0';
            sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
            m_ipcString.Append(tmpbuf, sock->LastReadCount());
            while(m_ipcString.Find("</url>") != wxNOT_FOUND)
            {
                wxString line = m_ipcString.AfterFirst('>');
                line = line.BeforeFirst('<');
                ParseReceived(line);
                m_ipcString = m_ipcString.AfterLast('>');
            }
            break;
        }
        case wxSOCKET_LOST:
        {
            wxLogDebug("Deleting socket.");
            sock->Destroy();
            break;
        }
        default: ;
    }
}

void mMainFrame::OnFindDialog(wxFindDialogEvent &event)
{
    wxEventType type = event.GetEventType();
    if(type == wxEVT_FIND || type == wxEVT_FIND_NEXT)
    {
        if(m_book->GetSelection()==0) //Progress tab
        {
            int i = -1, index = -1, flags, countitems;
            bool found = false;
            index = m_selectedProgress;
            flags = event.GetFlags();
            if(flags & wxFR_DOWN)
            {
                i = index+1;
                countitems = m_progressList.size();
            }
            else
            {
                if(index > -1)
                    i = index-1;
                countitems = -1;
            }
            while(i != countitems)
            {
                if(flags & wxFR_WHOLEWORD)
                {
                    if(flags & wxFR_MATCHCASE)
                    {
                        if(m_progressList[static_cast<size_t>(i)].name() == event.GetFindString())
                            found = true;
                    }
                    else
                    {
                        if(m_progressList[static_cast<size_t>(i)].name().Lower() == event.GetFindString().Lower())
                            found = true;
                    }
                }
                else
                {
                    if(flags & wxFR_MATCHCASE)
                    {
                        if(m_progressList[static_cast<size_t>(i)].name().Find(event.GetFindString()) >= 0)
                            found = true;
                    }
                    else
                    {
                        if (m_progressList[static_cast<size_t>(i)].name().Lower().Find(event.GetFindString().Lower()) >= 0)
                            found = true;
                    }
                }
                if(found)
                {
                    m_progresslistctrl->SetItemState(i,wxLIST_STATE_SELECTED,wxLIST_STATE_SELECTED);
                    m_selectedProgress = i;
                    break;
                }
                if(flags & wxFR_DOWN)
                    i++;
                else
                    i--;
            }
            if(!found)
            {
                wxMessageBox(_("File not found."), _("Information..."), wxOK | wxICON_INFORMATION, this);
            }
            EnableTools();
            return;
        }
        else
        {
            int i = -1, index = -1, flags, countitems;
            bool found = false;
            index = m_selectedFinished;
            flags = event.GetFlags();
            if(flags & wxFR_DOWN)
            {
                i = index+1;
                countitems = m_finishedList.size();
            }
            else
            {
                if(index > -1)
                    i = index-1;
                countitems = -1;
            }
            while(i != countitems)
            {
                if(flags & wxFR_WHOLEWORD)
                {
                    if(flags & wxFR_MATCHCASE)
                    {
                        if(m_finishedList[static_cast<size_t>(i)].name() == event.GetFindString())
                            found = true;
                    }
                    else
                    {
                        if(m_finishedList[static_cast<size_t>(i)].name().Lower() == event.GetFindString().Lower())
                            found = true;
                    }
                }
                else
                {
                    if(flags & wxFR_MATCHCASE)
                    {
                        if(m_finishedList[static_cast<size_t>(i)].name().Find(event.GetFindString()) >= 0)
                            found = true;
                    }
                    else
                    {
                        if(m_finishedList[static_cast<size_t>(i)].name().Lower().Find(event.GetFindString().Lower()) >= 0)
                            found = true;
                    }
                }
                if(found)
                {
                    m_finishedlistctrl->SetItemState(i,wxLIST_STATE_SELECTED,wxLIST_STATE_SELECTED);
                    m_selectedFinished = i;
                    break;
                }
                if(flags & wxFR_DOWN)
                    i++;
                else
                    i--;
            }
            if(!found)
            {
                wxMessageBox(_("File not found."), _("Information..."), wxOK | wxICON_INFORMATION, this);
            }
            EnableTools();
            return;
        }
    }
    if(type == wxEVT_FIND_CLOSE)
    {
        wxFindReplaceDialog *dlg = event.GetDialog();
        if(dlg == m_dlgFind)
        {
            m_dlgFind = nullptr;
        }
        dlg->Destroy();
    }
}

void mMainFrame::SetBandwidth()
{
    m_engine->SetGlobalBandwith();
    for(const auto &d : m_progressList)
    {
        if(d.status() == STATUS_ACTIVE)
        {
            m_engine->SetDownloadBandwith(d.gid(), d.bandwidth());
        }
    }
}

void mMainFrame::SetStatusBandwidth()
{
    if(moptions.livebandwidthoption() == 1)
    {
        SetStatusText(_("Band control")+": "+_("Per Download"),2);
    }
    else if(moptions.livebandwidthoption() == 2)
    {
        SetStatusText(_("Band control")+": "+_("Active")+" ("+MyUtilFunctions::SpeedText(moptions.bandwidth()*1024, true, false)+")",2);
    }
    else
    {
        SetStatusText(_("Band control")+": "+_("Unlimited"),2);
    }
}

void mMainFrame::SetStatusSpeed(int down, int up)
{
    if(!down)
    {
        SetStatusText("",1);
        return;
    }
    if(!up)
    {
        SetStatusText(MyUtilFunctions::SpeedText(down,true),1);
        return;
    }
    SetStatusText(MyUtilFunctions::SpeedText(down,true)+","+MyUtilFunctions::SpeedText(up,false),1);
}

void mMainFrame::ShowHideResizeGraph()
{
    if(moptions.graphheight() != m_graph->GetSize().GetHeight()) //SHOW/HIDE/CHANGE THE HEIGHT OF THE GRAPH
    {
        m_graph->SetMinSize(wxSize(-1,moptions.graphheight()));
        m_graph->SetSize(wxSize(-1,moptions.graphheight()));
        m_graph->Hide();
        if(moptions.graphshow())
            m_graph->Show();
    }
    if(!moptions.graphshow())
        m_graph->Hide();
    else
        m_graph->Show();
    m_sizer->Layout();
}

void mMainFrame::UpdateProgressList()
{
    size_t oldcnt = m_progresslistctrl->GetItemCount();
    size_t cnt = m_progressList.size();
    m_progresslistctrl->SetItemCount(cnt);
    if(cnt)
    {
        if(m_selectedProgress >= static_cast<long>(cnt))
        {
            m_progresslistctrl->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
            m_selectedProgress = 0;
        }
        if(oldcnt <= cnt) m_progresslistctrl->RefreshItems(0, --cnt);
        else m_progresslistctrl->Refresh();
    }
    else
    {
        m_progresslistctrl->Refresh();
        m_progress->setNumpieces(0);
        m_progress->setFinished(false);
        m_detail->SetValue("");
        m_selectedProgress = -1;
    }
    EnableTools();
}

void mMainFrame::UpdateFinishedList()
{
    size_t oldcnt = m_finishedlistctrl->GetItemCount();
    size_t cnt = m_finishedList.size();
    m_finishedlistctrl->SetItemCount(cnt);
    if(cnt)
    {
        if(m_selectedFinished >= static_cast<long>(cnt))
        {
            m_finishedlistctrl->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
            m_selectedFinished = 0;
        }
        if(oldcnt <= cnt) m_finishedlistctrl->RefreshItems(0, --cnt);
        else m_finishedlistctrl->Refresh();
    }
    else
    {
        m_finishedlistctrl->Refresh();
        m_selectedFinished = -1;
    }
    EnableTools();
}

void mMainFrame::EnableTools()
{
    if(m_book->GetSelection()==0) //Progress tab
    {
        m_bar->EnableTool(ID_MENU_REMOVE, m_selectedProgress != -1);
        m_bar->EnableTool(ID_MENU_SCHEDULE, m_selectedProgress != -1
                && m_progressList[static_cast<size_t>(m_selectedProgress)].status() != STATUS_SCHEDULE_QUEUE
                && m_progressList[static_cast<size_t>(m_selectedProgress)].status() != STATUS_SCHEDULE_ACTIVE);
        if(m_selectedProgress != -1)
        {
            if(m_progressList[static_cast<size_t>(m_selectedProgress)].status() != STATUS_ACTIVE
                    && m_progressList[static_cast<size_t>(m_selectedProgress)].status() != STATUS_SCHEDULE_QUEUE
                    && m_progressList[static_cast<size_t>(m_selectedProgress)].status() != STATUS_SCHEDULE_ACTIVE)
                m_bar->EnableTool(ID_MENU_START, true);
            else
                m_bar->EnableTool(ID_MENU_START, false);
        }
        else
        {
            m_bar->EnableTool(ID_MENU_START, false);
        }
        if(m_selectedProgress != -1)
        {
            if(m_progressList[static_cast<size_t>(m_selectedProgress)].status() == STATUS_ACTIVE)
                m_bar->EnableTool(ID_MENU_STOP, true);
            else
                m_bar->EnableTool(ID_MENU_STOP, false);
        }
        else
        {
            m_bar->EnableTool(ID_MENU_STOP, false);
        }
        m_bar->EnableTool(ID_MENU_STARTALL, EnableStartAll());
        m_bar->EnableTool(ID_MENU_STOPALL, m_selectedProgress != -1 && SomeProcessActive());
        m_bar->EnableTool(ID_MENU_UP, m_selectedProgress != -1 && m_progressList.size() > 1);
        m_bar->EnableTool(ID_MENU_DOWN, m_selectedProgress != -1 && m_progressList.size() > 1);
        m_bar->EnableTool(ID_MENU_INFO, m_selectedProgress != -1);

        m_menucopyurl->Enable(m_selectedProgress != -1);
        m_menucopydownloaddata->Enable(m_selectedProgress != -1);
        m_menuremoveall->Enable(m_progressList.size());
        m_menufind->Enable(m_progressList.size() > 1);
        m_menuremove->Enable(m_selectedProgress != -1);
        m_menuschedule->Enable(m_selectedProgress != -1
                && m_progressList[static_cast<size_t>(m_selectedProgress)].status() != STATUS_SCHEDULE_QUEUE
                && m_progressList[static_cast<size_t>(m_selectedProgress)].status() != STATUS_SCHEDULE_ACTIVE);
        if(m_selectedProgress != -1)
        {
            if(m_progressList[static_cast<size_t>(m_selectedProgress)].status() != STATUS_ACTIVE
                    && m_progressList[static_cast<size_t>(m_selectedProgress)].status() != STATUS_SCHEDULE_QUEUE
                    && m_progressList[static_cast<size_t>(m_selectedProgress)].status() != STATUS_SCHEDULE_ACTIVE)
                m_menustart->Enable(true);
            else
                m_menustart->Enable(false);
        }
        else
        {
            m_menustart->Enable(false);
        }
        if(m_selectedProgress != -1)
        {
            if(m_progressList[static_cast<size_t>(m_selectedProgress)].status() == STATUS_ACTIVE)
                m_menustop->Enable(true);
            else
                m_menustop->Enable(false);
        }
        else
        {
            m_menustop->Enable(false);
        }
        m_menustartall->Enable(EnableStartAll());
        m_menustopall->Enable(m_selectedProgress != -1 && SomeProcessActive());
        m_menuproperties->Enable(m_selectedProgress != -1);
        m_menumove->Enable(false);
#if !(defined (__WXMAC__)) //TODO fix on next release
        m_menumd5->Enable(false);
#endif
        m_menuopendestination->Enable(false);
        m_menuagain->Enable(false);
    }
    else
    {
        m_bar->EnableTool(ID_MENU_REMOVE, m_selectedFinished != -1);
        m_bar->EnableTool(ID_MENU_SCHEDULE, false);
        m_bar->EnableTool(ID_MENU_START, false);
        m_bar->EnableTool(ID_MENU_STOP, false);
        m_bar->EnableTool(ID_MENU_STARTALL, false);
        m_bar->EnableTool(ID_MENU_STOPALL, false);
        m_bar->EnableTool(ID_MENU_UP, false);
        m_bar->EnableTool(ID_MENU_DOWN, false);
        m_bar->EnableTool(ID_MENU_INFO, false);

        m_menucopyurl->Enable(m_selectedFinished != -1);
        m_menucopydownloaddata->Enable(m_selectedFinished != -1);
        m_menuremoveall->Enable(m_finishedList.size());
        m_menufind->Enable(m_finishedList.size() > 1);
        m_menuremove->Enable(m_selectedFinished != -1);
        m_menuschedule->Enable(false);
        m_menustart->Enable(false);
        m_menustop->Enable(false);
        m_menustartall->Enable(false);
        m_menustopall->Enable(false);
        m_menuproperties->Enable(false);
        m_menumove->Enable(m_selectedFinished != -1 && m_finishedList[static_cast<size_t>(m_selectedFinished)].status() == STATUS_FINISHED);
#if !(defined (__WXMAC__)) //TODO fix on next release
        m_menumd5->Enable(m_selectedFinished != -1 && m_finishedList[static_cast<size_t>(m_selectedFinished)].status() == STATUS_FINISHED);
#endif
        m_menuopendestination->Enable(m_selectedFinished != -1 && m_finishedList[static_cast<size_t>(m_selectedFinished)].status() == STATUS_FINISHED);
        m_menuagain->Enable(m_selectedFinished != -1);
    }
}

bool mMainFrame::EnableStartAll()
{
    for(const auto &process : m_progressList)
    {
        if(process.status() == STATUS_QUEUE || process.status() == STATUS_STOPED)
        {
            return true;
            break;
        }
    }
    return false;
}

bool mMainFrame::SomeProcessActive()
{
    for(const auto &process : m_progressList)
    {
        if(process.status() == STATUS_ACTIVE)
        {
            return true;
        }
    }
    return false;
}

bool mMainFrame::SomeProcessOnDownload()
{
    for(const auto &process : m_progressList)
    {
        if(process.status() == STATUS_ACTIVE || process.status() == STATUS_SCHEDULE_ACTIVE)
        {
            return true;
        }
    }
    return false;
}

int mMainFrame::LastIndex()
{
    int index = 0;
    for(const auto &process : m_progressList)
    {
        if(process.index() > index)
        {
            index = process.index();
        }
    }
    return ++index;
}

void mMainFrame::StartDownload(size_t item)
{
    if(CurrentSimultaneous() >= moptions.simultaneous())
    {
        return;
    }
    if(m_progressList[item].status() == STATUS_QUEUE)
    {
        if(!m_progressList[item].downloadLength())
        {
            m_progressList[item].setStartTime(wxDateTime::Now());
            m_progressList[item].setEndTime(wxDateTime::Now());
        }
        if(m_engine->HasGid(m_progressList[item].gid()))
            m_engine->ResumeDownload(m_progressList[item].gid(), m_progressList[item].destination(), m_progressList[item].connections(), m_progressList[item].bandwidth());
        else
            m_engine->AddFileInfo(m_progressList[static_cast<size_t>(item)]);
        m_progressList[item].setStatus(STATUS_ACTIVE);
        EnableTools();
        m_progressList[item].appendLogLine(_("Download started")+" "+wxDateTime::Now().Format("%x %X"));
        UpdateDetail();
    }
    if(m_progressList[item].status() == STATUS_STOPED)
    {
        if(m_engine->HasGid(m_progressList[item].gid()))
            m_engine->ResumeDownload(m_progressList[item].gid(), m_progressList[item].destination(), m_progressList[item].connections(), m_progressList[item].bandwidth());
        else
            m_engine->AddFileInfo(m_progressList[static_cast<size_t>(item)]);
        m_progressList[item].setStatus(STATUS_ACTIVE);
        EnableTools();
        m_progressList[item].appendLogLine(_("Download started")+" "+wxDateTime::Now().Format("%x %X"));
        UpdateDetail();
    }
    if(moptions.hidemainframe() && frameShown)
    {
        frame->Hide();
        frameShown = false;
    }
}

void mMainFrame::PauseDownload(size_t item)
{
    if(m_progressList[item].status() == STATUS_ACTIVE)
    {
        m_engine->PauseDownload(m_progressList[item].gid());
        m_progressList[item].setStatus(STATUS_QUEUE);
        EnableTools();
        m_progressList[item].appendLogLine(_("Download paused")+" "+wxDateTime::Now().Format("%x %X"));
        UpdateDetail();
    }
}

bool mMainFrame::ReadProgressList(wxString dir)
{
    if(dir.IsEmpty())
    {
        dir = wxStandardPaths::Get().GetUserDataDir()+wxFILE_SEP_PATH;
    }
    if(dir.Right(1) != wxFILE_SEP_PATH)
        dir << wxFILE_SEP_PATH;
    if(!wxFile::Exists(dir+"progress.xml")) return false;
    wxFileInputStream fin(dir+"progress.xml");
    if(fin.IsOk())
    {
        wxXmlDocument xml;
        if(!xml.Load(fin)) return false;
        if(xml.GetRoot()->GetName() != "ProgressFiles") return false;
        wxXmlNode *rootNode = xml.GetRoot();
        wxXmlNode *elementNode = rootNode->GetChildren();
        while(elementNode)
        {
            mFileInfo info;
            if(info.fromXmlNode(elementNode))
            {
                m_progressList.push_back(info);
            }
            elementNode = elementNode->GetNext();
        }
        std::sort(m_progressList.begin(), m_progressList.end(), SortFileInfo);
        return true;
    }
    else
        return false;
}

bool mMainFrame::ReadFinishedList(wxString dir)
{
    if(dir.IsEmpty())
    {
        dir = wxStandardPaths::Get().GetUserDataDir()+wxFILE_SEP_PATH;
    }
    if(dir.Right(1) != wxFILE_SEP_PATH)
        dir << wxFILE_SEP_PATH;
    if(!wxFile::Exists(dir+"finished.xml")) return false;
    wxFileInputStream fin(dir+"finished.xml");
    if(fin.IsOk())
    {
        wxXmlDocument xml;
        if(!xml.Load(fin)) return false;
        if(xml.GetRoot()->GetName() != "FinishedFiles") return false;
        wxXmlNode *rootNode = xml.GetRoot();
        wxXmlNode *elementNode = rootNode->GetChildren();
        while(elementNode)
        {
            mFileInfo info;
            if(info.fromXmlNode(elementNode))
            {
                m_finishedList.push_back(info);
            }
            elementNode = elementNode->GetNext();
        }
        return true;
    }
    else
        return false;
}

bool mMainFrame::SaveProgressList()
{
    if(wxFileExists(wxStandardPaths::Get().GetUserDataDir()+wxFILE_SEP_PATH+"progress.xml"))
    {
        wxRemoveFile(wxStandardPaths::Get().GetUserDataDir()+wxFILE_SEP_PATH+"progress.xml");
    }
    wxFileOutputStream pout(wxStandardPaths::Get().GetUserDataDir()+wxFILE_SEP_PATH+"progress.xml");
    if(pout.IsOk())
    {
        wxXmlDocument xml;
        wxXmlNode *rootNode = new wxXmlNode(wxXML_ELEMENT_NODE, "ProgressFiles");
        for(auto p : m_progressList)
        {
            rootNode->AddChild(p.createXmlNode());
        }
        xml.SetRoot(rootNode);
        xml.Save(pout);
        return true;
    }
    return false;
}

bool mMainFrame::SaveFinishedList()
{
    if(wxFileExists(wxStandardPaths::Get().GetUserDataDir()+wxFILE_SEP_PATH+"finished.xml"))
    {
        wxRemoveFile(wxStandardPaths::Get().GetUserDataDir()+wxFILE_SEP_PATH+"finished.xml");
    }
    wxFileOutputStream fout(wxStandardPaths::Get().GetUserDataDir()+wxFILE_SEP_PATH+"finished.xml");
    if(fout.IsOk())
    {
        wxXmlDocument xml;
        wxXmlNode *rootNode = new wxXmlNode(wxXML_ELEMENT_NODE, "FinishedFiles");
        for(auto p : m_finishedList)
        {
            rootNode->AddChild(p.createXmlNode());
        }
        xml.SetRoot(rootNode);
        xml.Save(fout);
        return true;
    }
    return false;
}

std::string mMainFrame::CreateGID()
{
    const std::string CHARACTERS = "0123456789abcdef";
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);
    std::string random_string;
    for(std::size_t i = 0; i < 16; ++i)
    {
        random_string += CHARACTERS[distribution(generator)];
    }
    return random_string;
}

bool mMainFrame::HasGID(aria2::A2Gid gid)
{
    for(const auto &p : m_progressList)
    {
        if(p.gid() == gid)
            return true;
    }
    return false;
}

aria2::A2Gid mMainFrame::GenerateGID()
{
    std::string sgid = CreateGID();
    while(HasGID(aria2::hexToGid(sgid)))
    {
        sgid = CreateGID();
    }
    return aria2::hexToGid(sgid);
}

bool mMainFrame::ExportConf(wxString dir)
{
    SaveProgressList();
    SaveFinishedList();
    bool result;
    if(dir.Right(1) != wxFILE_SEP_PATH)
        dir << wxFILE_SEP_PATH;
    wxString source, destination;
    source = wxStandardPaths::Get().GetUserDataDir()+wxFILE_SEP_PATH+"wxdfast.ini";
    if(!wxFileExists(source))
        return false;
    destination = dir+"wxdfast.ini";
    result = wxCopyFile(source, destination , true);
    source = wxStandardPaths::Get().GetUserDataDir()+wxFILE_SEP_PATH+"progress.xml";
    if(wxFileExists(source))
    {
        destination = dir+"progress.xml";
        result = wxCopyFile(source, destination , true);
    }
    source = wxStandardPaths::Get().GetUserDataDir()+wxFILE_SEP_PATH+"finished.xml";
    if(wxFileExists(source))
    {
        destination = dir+"finished.xml";
        result = wxCopyFile(source, destination , true);
    }
    return result;
}

void mMainFrame::LogAfterFirstStart(size_t item)
{
    if(!m_progressList[item].totalLength() || !m_progressList[item].numPieces())
        return;
    m_progressList[item].appendLogLine(wxT("   ")+_("Destination") << ": " << m_progressList[item].destination());
    m_progressList[item].appendLogLine(wxT("   ")+_("File type") << ": " << MyUtilFunctions::MimeType(m_progressList[item].name()));
    m_progressList[item].appendLogLine(wxT("   ")+_("Size")+": "+MyUtilFunctions::SizeText(m_progressList[item].totalLength()));
    m_progressList[item].appendLogLine(wxT("   ")+_("Maximum number of connections")+": "+wxString::Format("%d",m_progressList[item].connections()));
    m_progressList[item].appendLogLine(wxT("   ")+_("Download bandwidth")+": "+(m_progressList[item].bandwidth()?MyUtilFunctions::SpeedText(m_progressList[item].bandwidth()*1024,true,false):_("Unlimited")));
    m_progressList[item].appendLogLine(wxT("   ")+_("Number of pieces")+": "+wxString::Format("%d",m_progressList[item].numPieces()));
    m_progressList[item].appendLogLine(wxT("   ")+_("URL")+": "+MyUtilFunctions::ArrayStringTowxString(m_progressList[item].urls()));
    if(m_progressList[item].status() == STATUS_SCHEDULE_QUEUE || m_progressList[item].status() == STATUS_SCHEDULE_ACTIVE)
        m_progressList[item].appendLogLine(wxT("   ")+_("Downloaded is scheduled"));
    m_progressList[item].addAttempt();
    UpdateDetail();
}

void mMainFrame::UpdateDetail()
{
    if(m_selectedProgress != -1)
    {
        m_detail->SetValue(m_progressList[static_cast<size_t>(m_selectedProgress)].log());
    }
    else
    {
        m_detail->SetValue("");
    }
    m_detail->ShowPosition(m_detail->GetLastPosition());
    m_detail->Update();
}

void mMainFrame::ShowNotify(const wxString &title, const wxString &message)
{
    if(m_havenotify)
    {
        wxExecute(wxString::Format("notify-send -i %s/icons/wxdfast.png '%s' '%s'", WXDFAST_DATADIR, title, message));
        return;
    }
#if __WXMAC__
    wxNotificationMessage msg(title+" "+message);
#else
    wxNotificationMessage msg(title, message);
#endif
    msg.Show();
}

void mMainFrame::ShutDownOrDisconnect()
{
    if(SomeProcessOnDownload())
    {
        return;
    }
    if(moptions.shutdown() || moptions.alwaysshutdown())
    {
        wxExecute(moptions.shutdowncmd());
    }
    if(moptions.disconnect() || moptions.alwaysdisconnect())
    {
        wxExecute(moptions.disconnectcmd());
    }
}

void mMainFrame::ParseReceived(const wxString &text)
{
    wxJSONValue root;
    wxJSONReader reader;
    int numErrors = reader.Parse(text, &root);
    if(numErrors > 0)
    {
        wxLogDebug("ERROR: the JSON document is not well-formed");
        const wxArrayString& errors = reader.GetErrors();
        for(size_t i=0; i < errors.GetCount(); i++)
        {
            wxLogDebug(errors[i]);
        }
        return;
    }
    if(!root.HasMember("URL"))
    {
        return;
    }
    bool batch = false;
    if(root.HasMember("Batch"))
    {
        batch = root["Batch"].AsBool();
    }
    if(batch)
    {
        wxArrayString urls;
        wxStringTokenizer urltoken(root["URL"].AsString(), "\n");
        while(urltoken.HasMoreTokens())
        {
            urls.Add(urltoken.GetNextToken());
        }
#if defined (__WXMAC__)
        mBatchDialog dialog(nullptr);
#else
        mBatchDialog dialog(this);
#endif
        dialog.SetUrls(urls);
        dialog.SetReferenceURL(root["Referer"].AsString());
        dialog.ReLayout();
        if(dialog.ShowModal() == wxID_OK)
        {
            urls.Clear();
            urls = dialog.GetUrls();
            for(const auto &url : urls)
            {
                mFileInfo info;
                info.setIndex(LastIndex());
                info.setGid(GenerateGID());
                info.setDestination(dialog.GetDestination());
                info.addUrl(url);
                info.setUser(dialog.GetUser());
                info.setPassword(dialog.GetPassword());
                if(dialog.GetStartOption() == NOW)
                    info.setStatus(STATUS_STOPED);
                else if(dialog.GetStartOption() == MANUAL)
                    info.setStatus(STATUS_QUEUE);
                else
                    info.setStatus(STATUS_SCHEDULE_QUEUE);
                info.setConnections(dialog.GetNumberOfParts());
                info.setBandwidth(dialog.GetBandwidth());
                info.setComment(dialog.GetComment());
                info.setLink(dialog.GetReferenceURL());
                info.setCommand(dialog.GetCommand());
                info.setName(url.AfterLast('/'));
                m_progressList.push_back(info);
                std::sort(m_progressList.begin(), m_progressList.end(), SortFileInfo);
                UpdateProgressList();
                m_selectedProgress = m_progressList.size()-1;
                m_progresslistctrl->SetItemState(m_selectedProgress, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
                if(info.status() != STATUS_QUEUE)
                    StartDownload(m_selectedProgress);
            }
        }
    }
    else
    {
#if defined (__WXMAC__)
        mNewDialog dialog(nullptr);
#else
        mNewDialog dialog(this);
#endif
        dialog.SetUrl(root["URL"].AsString());
        dialog.SetReferenceURL(root["Referer"].AsString());
        if(moptions.rememberboxnewoptions())
        {
            dialog.SetBandwidth(moptions.lastbandwidth());
            dialog.SetCommand(moptions.lastcommand());
            dialog.SetDestination(moptions.lastdestination());
            dialog.SetNumberOfConnections(moptions.lastnumberofconnections());
            dialog.SetOnTop(moptions.lastontopoption());
            dialog.SetStartOption(moptions.laststartoption());
            dialog.SetSize(moptions.lastx(),moptions.lasty(),-1,-1);
        }
        if(dialog.ShowModal() == wxID_OK)
        {
            mFileInfo info;
            if(dialog.GetOnTop())
            {
                info.setIndex(1);
                for(auto it=m_progressList.begin(); it!=m_progressList.end(); ++it)
                {
                    (*it).setIndex((*it).index()+1);
                }
            }
            else
            {
                info.setIndex(LastIndex());
            }
            info.setGid(GenerateGID());
            info.setDestination(dialog.GetDestination());
            info.setUrls(dialog.GetUrls());
            info.setUser(dialog.GetUser());
            info.setPassword(dialog.GetPassword());
            if(dialog.GetStartOption() == NOW)
                info.setStatus(STATUS_STOPED);
            else if(dialog.GetStartOption() == MANUAL)
                info.setStatus(STATUS_QUEUE);
            else
                info.setStatus(STATUS_SCHEDULE_QUEUE);
            info.setConnections(dialog.GetNumberOfConnections());
            info.setBandwidth(dialog.GetBandwidth());
            info.setComment(dialog.GetComment());
            info.setLink(dialog.GetReferenceURL());
            info.setCommand(dialog.GetCommand());
            info.setName(dialog.GetUrls().Item(0).AfterLast('/'));
            m_progressList.push_back(info);
            std::sort(m_progressList.begin(), m_progressList.end(), SortFileInfo);
            UpdateProgressList();
            m_selectedProgress = m_progressList.size()-1;
            m_progresslistctrl->SetItemState(m_selectedProgress, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
            if(moptions.rememberboxnewoptions())
            {
                int lastx, lasty;
                dialog.GetPosition(&lastx,&lasty);
                moptions.setLastx(lastx);
                moptions.setLasty(lasty);
                moptions.setLastbandwidth(dialog.GetBandwidth());
                moptions.setLastcommand(dialog.GetCommand());
                moptions.setLastdestination(dialog.GetDestination());
                moptions.setLastnumberofconnections(dialog.GetNumberOfConnections());
                moptions.setLastontopoption(dialog.GetOnTop());
                moptions.setLaststartoption(dialog.GetStartOption());
            }
            if(info.status() != STATUS_QUEUE)
                StartDownload(m_selectedProgress);
        }
    }
}

int mMainFrame::CurrentSimultaneous()
{
    int s = 0;
    for(const auto &file : m_progressList)
    {
        if(file.status() == STATUS_ACTIVE || file.status() == STATUS_SCHEDULE_ACTIVE)
            s++;
    }
    return s;
}

void mMainFrame::RemoveFromProgressList(size_t item)
{
    size_t i=0;
    for(auto it=m_progressList.begin(); it!=m_progressList.end(); ++it)
    {
        if(item == i)
        {
            if(wxFileExists((*it).destination()+wxFILE_SEP_PATH+(*it).name()))
                wxRemoveFile((*it).destination()+wxFILE_SEP_PATH+(*it).name());
            if(wxFileExists((*it).destination()+wxFILE_SEP_PATH+(*it).name()+".aria2"))
                wxRemoveFile((*it).destination()+wxFILE_SEP_PATH+(*it).name()+".aria2");
            m_progressList.erase(it);
            return;
        }
        i++;
    }
}

void mMainFrame::RemoveFromFinishedList(size_t item, bool removefile)
{
    size_t i=0;
    for(auto it=m_finishedList.begin(); it!=m_finishedList.end(); ++it)
    {
        if(item == i)
        {
            if(removefile)
            {
                if(wxFileExists((*it).destination()+wxFILE_SEP_PATH+(*it).name()))
                    wxRemoveFile((*it).destination()+wxFILE_SEP_PATH+(*it).name());
            }
            m_finishedList.erase(it);
            return;
        }
        i++;
    }
}

void mMainFrame::SelectProgressByIndex(int index)
{
    if(!m_progressList.size())
        return;
    m_progresslistctrl->ClearSelection();
    long i=0;
    for(auto it=m_progressList.begin(); it!=m_progressList.end(); ++it)
    {
        if((*it).index() == index)
        {
            break;
        }
        i++;
    }
    m_selectedProgress = i;
    m_progresslistctrl->SetItemState(m_selectedProgress, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    m_progresslistctrl->EnsureVisible(m_selectedProgress);
    EnableTools();
}

void ProgressCtrl::CreateColumns()
{
    InsertColumn(0, "");
    InsertColumn(1, _("Restart"));
    InsertColumn(2, _("Filename"));
    InsertColumn(3, _("Size"));
    InsertColumn(4, _("Completed"));
    InsertColumn(5, _("Percentage"), wxLIST_FORMAT_CENTER);
    InsertColumn(6, _("Time Passed"));
    InsertColumn(7, _("Remaining"));
    InsertColumn(8, _("Speed"));
    InsertColumn(9, _("Attempts"));
    InsertColumn(10, _("URL"));
    setResizeColumn(2);
    SetColumnWidth(0,25);
    SetColumnWidth(1,80);
    setResizeColumnMinWidth(160);
    SetColumnWidth(3,100);
    SetColumnWidth(4,100);
    SetColumnWidth(5,100);
    SetColumnWidth(6,100);
    SetColumnWidth(7,100);
    SetColumnWidth(8,100);
    SetColumnWidth(9,100);
    SetColumnWidth(10,200);
}

wxString ProgressCtrl::OnGetItemText(long item, long column) const
{
    return frame->GetDownloadItemText(item, column);
}

int ProgressCtrl::OnGetItemColumnImage(long item, long column) const
{
    return frame->GetDownloadItemColumnImage(item, column);
}

wxListItemAttr *ProgressCtrl::OnGetItemAttr(long /*item*/) const
{
    return nullptr;
}

void FinishedCtrl::CreateColumns()
{
    InsertColumn(0, "");
    InsertColumn(1, _("Filename"));
    InsertColumn(2, _("Size"));
    InsertColumn(3, _("Finished"));
    setResizeColumn(1);
    SetColumnWidth(0,25);
    setResizeColumnMinWidth(250);
    SetColumnWidth(2,110);
    SetColumnWidth(3,200);
}

wxString FinishedCtrl::OnGetItemText(long item, long column) const
{
    return frame->GetFinishItemText(item, column);
}

int FinishedCtrl::OnGetItemColumnImage(long item, long column) const
{
    return frame->GetFinishItemColumnImage(item, column);
}

wxListItemAttr *FinishedCtrl::OnGetItemAttr(long /*item*/) const
{
    return nullptr;
}

void FinishedInfoCtrl::CreateColumns()
{
    InsertColumn(0, "");
    InsertColumn(1, "");
    SetColumnWidth(0,120);
    setResizeColumn(1);
}

wxString FinishedInfoCtrl::OnGetItemText(long item, long column) const
{
    return frame->GetFinishInfoItemText(item, column);
}

int FinishedInfoCtrl::OnGetItemColumnImage(long /*item*/, long /*column*/) const
{
    return -1;
}

wxListItemAttr *FinishedInfoCtrl::OnGetItemAttr(long /*item*/) const
{
    return nullptr;
}

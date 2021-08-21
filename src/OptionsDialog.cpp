/***************************************************************
 * Name:      OptionsDialog.cpp
 * Author:    Max Magalhães Velasques (maxvelasques@gmail.com)
 *            David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2006
 * License:   GPL2
 **************************************************************/

#include <wx/gbsizer.h>
#include <wx/listbook.h>
#include <wx/tokenzr.h>
#include <wx/notifmsg.h>
#include <wx/hyperlink.h>
#include "OptionsDialog.h"
#include "Defs.h"
#include "UtilFunctions.h"

IMPLEMENT_DYNAMIC_CLASS(mOptionsDialog, wxDialog)

BEGIN_EVENT_TABLE(mOptionsDialog, wxDialog)
    EVT_BUTTON(wxID_OK, mOptionsDialog::OnOk)
    EVT_BUTTON(wxID_CANCEL, mOptionsDialog::OnCancel)
    EVT_BUTTON(ID_OPTIONS_DESTINATION, mOptionsDialog::OnDestination)
    EVT_BUTTON(ID_OPTIONS_FILEMANAGERPATH, mOptionsDialog::OnFilemanager)
    EVT_DATE_CHANGED(ID_OPTIONS_STARTDATE, mOptionsDialog::OnStartDate)
    EVT_TIME_CHANGED(ID_OPTIONS_STARTTIME, mOptionsDialog::OnStartTime)
    EVT_DATE_CHANGED(ID_OPTIONS_FINISHDATE, mOptionsDialog::OnFinishDate)
    EVT_TIME_CHANGED(ID_OPTIONS_FINISHTIME, mOptionsDialog::OnFinishTime)
    EVT_BUTTON(ID_OPTIONS_EXCEPTION_ADD, mOptionsDialog::OnAddException)
    EVT_BUTTON(ID_OPTIONS_EXCEPTION_REMOVE, mOptionsDialog::OnRemoveException)
    EVT_BUTTON(ID_OPTIONS_NOTIFY, mOptionsDialog::OnShowNotify)
END_EVENT_TABLE()

mOptionsDialog::mOptionsDialog(wxWindow *parent)
    : wxDialog(parent, wxID_ANY, _("Options"))
{
    for(size_t j=0; j < MAX_SCHEDULE_EXCEPTIONS; j++)
    {
        if(moptions.scheduleException(j).isactive)
            m_exceptions.push_back(moptions.scheduleException(j));
    }

    int i = 0;

    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

    wxListbook *book = new wxListbook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLB_LEFT);

    wxPanel *general = new wxPanel(book);
    wxBoxSizer *box1 = new wxBoxSizer(wxHORIZONTAL);
    wxGridBagSizer *gridSizer1 = new wxGridBagSizer(5, 5);
    gridSizer1->Add(new wxStaticText(general, wxID_ANY, _("Attempts:")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_attempts = new wxSpinCtrl(general);
    m_attempts->SetRange(5,1000);
    m_attempts->SetValue(moptions.attempts());
    gridSizer1->Add(m_attempts, wxGBPosition(i,1), wxDefaultSpan, wxEXPAND);
    i++;
    gridSizer1->Add(new wxStaticText(general, wxID_ANY, _("Time between the attempts in seconds:")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_attemptstime = new wxSpinCtrl(general);
    m_attemptstime->SetRange(0,100);
    m_attemptstime->SetValue(moptions.attemptstime());
    gridSizer1->Add(m_attemptstime, wxGBPosition(i,1), wxDefaultSpan, wxEXPAND);
    i++;
    gridSizer1->Add(new wxStaticText(general, wxID_ANY, _("Number of simultaneous downloads:")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_simultaneous = new wxSpinCtrl(general);
    m_simultaneous->SetRange(1,100);
    m_simultaneous->SetValue(moptions.simultaneous());
    gridSizer1->Add(m_simultaneous, wxGBPosition(i,1), wxDefaultSpan, wxEXPAND);
    i++;
    m_rememberboxnewoptions = new wxCheckBox(general, wxID_ANY, _("Remember the last settings on the New Download box"));
    m_rememberboxnewoptions->SetValue(moptions.rememberboxnewoptions());
    gridSizer1->Add(m_rememberboxnewoptions, wxGBPosition(i,0), wxGBSpan(1,2), wxEXPAND);
    i++;
    m_shutdown = new wxCheckBox(general, wxID_ANY, _("Shutdown when all downloads finish"));
    m_shutdown->SetValue(moptions.shutdown());
    gridSizer1->Add(m_shutdown, wxGBPosition(i,0), wxGBSpan(1,2), wxEXPAND);
    i++;
    m_disconnect = new wxCheckBox(general, wxID_ANY, _("Disconnect when all downloads finish"));
    m_disconnect->SetValue(moptions.disconnect());
    gridSizer1->Add(m_disconnect, wxGBPosition(i,0), wxGBSpan(1,2), wxEXPAND);
    i++;
    m_closedialog = new wxCheckBox(general, wxID_ANY, _("Show dialog box when program closes"));
    m_closedialog->SetValue(moptions.closedialog());
    gridSizer1->Add(m_closedialog, wxGBPosition(i,0), wxGBSpan(1,2), wxEXPAND);
    i++;
    gridSizer1->Add(new wxStaticText(general, wxID_ANY, _("Refresh rate of the download list (millisecs):")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_timerupdateinterval = new wxSpinCtrl(general);
    m_timerupdateinterval->SetRange(100,1000);
    m_timerupdateinterval->SetValue(moptions.timerupdateinterval());
    gridSizer1->Add(m_timerupdateinterval, wxGBPosition(i,1), wxDefaultSpan, wxEXPAND);
    i++;
    m_restoremainframe = new wxCheckBox(general, wxID_ANY, _("Restore the main window when all downloads finish"));
    m_restoremainframe->SetValue(moptions.restoremainframe());
    gridSizer1->Add(m_restoremainframe, wxGBPosition(i,0), wxGBSpan(1,2), wxEXPAND);
    i++;
    m_hidemainframe = new wxCheckBox(general, wxID_ANY, _("Hide the main window when you start a download"));
    m_hidemainframe->SetValue(moptions.hidemainframe());
    gridSizer1->Add(m_hidemainframe, wxGBPosition(i,0), wxGBSpan(1,2), wxEXPAND);
    i++;
    m_checkforupdates = new wxCheckBox(general, wxID_ANY, _("Check for updates automatically"));
    m_checkforupdates->SetValue(moptions.checkforupdates());
    gridSizer1->Add(m_checkforupdates, wxGBPosition(i,0), wxGBSpan(1,2), wxEXPAND);
    i++;
    m_shownotify = new wxCheckBox(general, wxID_ANY, _("Show notification on finish download"));
    m_shownotify->SetValue(moptions.shownotify());
    gridSizer1->Add(m_shownotify, wxGBPosition(i,0), wxGBSpan(1,2), wxEXPAND);
    m_shownotifyTest = new wxButton(general, ID_OPTIONS_NOTIFY, _("Preview"));
    gridSizer1->Add(m_shownotifyTest, wxGBPosition(i,2));
    box1->Add(gridSizer1, 1, wxEXPAND|wxALL, 5);
    general->SetSizer(box1);
    box1->Fit(general);
    box1->SetSizeHints(general);
    book->AddPage(general, _("General"));

    i = 0;
    wxPanel *paths = new wxPanel(book);
    wxBoxSizer *box2 = new wxBoxSizer(wxHORIZONTAL);
    wxGridBagSizer *gridSizer2 = new wxGridBagSizer(5, 5);
    gridSizer2->Add(new wxStaticText(paths, wxID_ANY, _("Default destination:")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_destination = new wxTextCtrl(paths, wxID_ANY, moptions.destination(), wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    m_destination->SetMinSize(wxSize(222,-1));
    m_destination->SetToolTip(moptions.destination());
    gridSizer2->Add(m_destination, wxGBPosition(i,1), wxDefaultSpan, wxEXPAND);
    m_destinationSelect = new wxButton(paths, ID_OPTIONS_DESTINATION, "...");
    m_destinationSelect->SetMaxSize(wxSize(35,-1));
    gridSizer2->Add(m_destinationSelect, wxGBPosition(i,2));
    i++;
    /** We use explorer on Win, xdg-open on *nix, open on Mac
    gridSizer2->Add(new wxStaticText(paths, wxID_ANY, _("File manager path:")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_filemanagerpath = new wxTextCtrl(paths, wxID_ANY, moptions.filemanagerpath(), wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    m_filemanagerpath->SetMinSize(wxSize(222,-1));
    m_filemanagerpath->SetValue(moptions.filemanagerpath());
    m_filemanagerpath->SetToolTip(moptions.filemanagerpath());
    gridSizer2->Add(m_filemanagerpath, wxGBPosition(i,1), wxDefaultSpan, wxEXPAND);
    m_filemanagerpathSelect = new wxButton(paths, ID_OPTIONS_FILEMANAGERPATH, "...");
    m_filemanagerpathSelect->SetMaxSize(wxSize(35,-1));
    gridSizer2->Add(m_filemanagerpathSelect, wxGBPosition(i,2));
    i++;
    */
    gridSizer2->Add(new wxStaticText(paths, wxID_ANY, _("Shutdown command:")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_shutdowncmd = new wxTextCtrl(paths, wxID_ANY);
    m_shutdowncmd->SetValue(moptions.shutdowncmd());
    gridSizer2->Add(m_shutdowncmd, wxGBPosition(i,1), wxGBSpan(1,2), wxEXPAND);
    i++;
    gridSizer2->Add(new wxStaticText(paths, wxID_ANY, _("Disconnect command:")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_disconnectcmd = new wxTextCtrl(paths, wxID_ANY);
    m_disconnectcmd->SetValue(moptions.disconnectcmd());
    gridSizer2->Add(m_disconnectcmd, wxGBPosition(i,1), wxGBSpan(1,2), wxEXPAND);
    box2->Add(gridSizer2, 1, wxEXPAND|wxALL, 5);
    paths->SetSizer(box2);
    box2->Fit(paths);
    box2->SetSizeHints(paths);
    book->AddPage(paths, _("Paths"));

    i = 0;
    wxPanel *proxy = new wxPanel(book);
    wxBoxSizer *box3 = new wxBoxSizer(wxHORIZONTAL);
    wxGridBagSizer *gridSizer3 = new wxGridBagSizer(5, 5);
    m_proxy = new wxCheckBox(proxy, wxID_ANY, _("Use a Proxy Server"));
    m_proxy->SetValue(moptions.proxy());
    gridSizer3->Add(m_proxy, wxGBPosition(i,0), wxDefaultSpan, wxEXPAND);
    i++;
    wxStaticBoxSizer *sbox1 = new wxStaticBoxSizer(wxHORIZONTAL, proxy, _("Proxy Server"));
    sbox1->AddSpacer(5);
    sbox1->Add(new wxStaticText(proxy, wxID_ANY, _("Server:")), 0, wxALIGN_CENTER_VERTICAL);
    m_proxyServer = new wxTextCtrl(proxy, wxID_ANY);
    m_proxyServer->SetMinSize(wxSize(222,-1));
    m_proxyServer->SetValue(moptions.proxyServer());
    sbox1->Add(m_proxyServer, 1, wxEXPAND, 5);
    sbox1->Add(new wxStaticText(proxy, wxID_ANY, _("Port:")), 0, wxALIGN_CENTER_VERTICAL);
    m_proxyPort = new wxTextCtrl(proxy, wxID_ANY);
    m_proxyPort->SetMaxSize(wxSize(65,-1));
    wxTextValidator symbolValidator(wxFILTER_INCLUDE_CHAR_LIST);
    symbolValidator.SetCharIncludes("0123456789");
    m_proxyPort->SetValidator(symbolValidator);
    m_proxyPort->SetMaxLength(5);
    m_proxyPort->SetValue(moptions.proxyPort());
    sbox1->Add(m_proxyPort, 1, wxEXPAND, 5);
    sbox1->AddSpacer(5);
    gridSizer3->Add(sbox1, wxGBPosition(i,0), wxDefaultSpan, wxEXPAND);
    i++;
    wxStaticBoxSizer *sbox2 = new wxStaticBoxSizer(wxHORIZONTAL, proxy, _("Proxy Server Authorization"));
    sbox2->AddSpacer(5);
    sbox2->Add(new wxStaticText(proxy, wxID_ANY, _("User:")), 0, wxALIGN_CENTER_VERTICAL);
    m_proxyUsername = new wxTextCtrl(proxy, wxID_ANY, moptions.proxyUsername());
    sbox2->Add(m_proxyUsername, 1, wxEXPAND, 5);
    sbox2->Add(new wxStaticText(proxy, wxID_ANY, _("Password:")), 0, wxALIGN_CENTER_VERTICAL);
    m_proxyAuthstring = new wxTextCtrl(proxy, wxID_ANY, moptions.proxyAuthstring(), wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
    sbox2->Add(m_proxyAuthstring, 1, wxEXPAND, 5);
    sbox2->AddSpacer(5);
    gridSizer3->Add(sbox2, wxGBPosition(i,0), wxDefaultSpan, wxEXPAND);
    box3->Add(gridSizer3, 1, wxEXPAND|wxALL, 5);
    proxy->SetSizer(box3);
    box3->Fit(proxy);
    box3->SetSizeHints(proxy);
    book->AddPage(proxy, _("Proxy"));

    i = 0;
    wxPanel *graph = new wxPanel(book);
    wxBoxSizer *box4 = new wxBoxSizer(wxHORIZONTAL);
    wxGridBagSizer *gridSizer4 = new wxGridBagSizer(5, 5);
    m_graphshow = new wxCheckBox(graph, wxID_ANY, _("Show Graph"));
    m_graphshow->SetValue(moptions.graphshow());
    gridSizer4->Add(m_graphshow, wxGBPosition(i,0), wxGBSpan(1,2), wxEXPAND);
    i++;
    gridSizer4->Add(new wxStaticText(graph, wxID_ANY, _("Graph refresh rate (millisecs):")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_graphrefreshtime = new wxSpinCtrl(graph);
    m_graphrefreshtime->SetRange(50,1000);
    m_graphrefreshtime->SetValue(moptions.graphrefreshtime());
    gridSizer4->Add(m_graphrefreshtime, wxGBPosition(i,1), wxDefaultSpan, wxEXPAND);
    i++;
    gridSizer4->Add(new wxStaticText(graph, wxID_ANY, _("Graph maximum value:")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_graphscale = new wxChoice(graph, wxID_ANY);
    m_graphscale->Append("500 KB/S");
    m_graphscale->Append("1 MB/S");
    m_graphscale->Append("5 MB/S");
    m_graphscale->Append("10 MB/S");
    m_graphscale->Append("20 MB/S");
    m_graphscale->Append("50 MB/S");
    m_graphscale->Append("100 MB/S");
    m_graphscale->Append("500 MB/S");
    if(moptions.graphscale() < 501)
        m_graphscale->SetSelection(0);
    else if(moptions.graphscale()>501 && moptions.graphscale()<1025)
        m_graphscale->SetSelection(1);
    else if(moptions.graphscale()>1024 && moptions.graphscale()<(5*1024+1))
        m_graphscale->SetSelection(2);
    else if(moptions.graphscale()>5*1024 && moptions.graphscale()<(10*1024+1))
        m_graphscale->SetSelection(3);
    else if(moptions.graphscale()>10*1024 && moptions.graphscale()<(20*1024+1))
        m_graphscale->SetSelection(4);
    else if(moptions.graphscale()>20*1024 && moptions.graphscale()<(50*1024+1))
        m_graphscale->SetSelection(5);
    else if(moptions.graphscale()>50*1024 && moptions.graphscale()<(100*1024+1))
        m_graphscale->SetSelection(6);
    else
        m_graphscale->SetSelection(7);
    gridSizer4->Add(m_graphscale, wxGBPosition(i,1), wxDefaultSpan, wxEXPAND);
    i++;
    gridSizer4->Add(new wxStaticText(graph, wxID_ANY, _("The text area Width (pixels):")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_graphtextarea = new wxSpinCtrl(graph);
    m_graphtextarea->SetRange(20,200);
    m_graphtextarea->SetValue(moptions.graphtextarea());
    gridSizer4->Add(m_graphtextarea, wxGBPosition(i,1), wxDefaultSpan, wxEXPAND);
    i++;
    gridSizer4->Add(new wxStaticText(graph, wxID_ANY, _("Graph height:")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_graphheight = new wxSpinCtrl(graph);
    m_graphheight->SetRange(40,200);
    m_graphheight->SetValue(moptions.graphheight());
    gridSizer4->Add(m_graphheight, wxGBPosition(i,1), wxDefaultSpan, wxEXPAND);
    i++;
    gridSizer4->Add(new wxStaticText(graph, wxID_ANY, _("Text font size (points):")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_graphspeedfontsize = new wxSpinCtrl(graph);
    m_graphspeedfontsize->SetRange(10,30);
    m_graphspeedfontsize->SetValue(moptions.graphspeedfontsize());
    gridSizer4->Add(m_graphspeedfontsize, wxGBPosition(i,1), wxDefaultSpan, wxEXPAND);
    i++;
    gridSizer4->Add(new wxStaticText(graph, wxID_ANY, _("Download line width:")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_graphlinewidth = new wxSpinCtrl(graph);
    m_graphlinewidth->SetRange(1,10);
    m_graphlinewidth->SetValue(moptions.graphlinewidth());
    gridSizer4->Add(m_graphlinewidth, wxGBPosition(i,1), wxDefaultSpan, wxEXPAND);
    i++;
    gridSizer4->Add(new wxStaticText(graph, wxID_ANY, _("Background colour:")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_graphbackcolor = new wxColourPickerCtrl(graph, wxID_ANY, moptions.graphbackcolor());
    gridSizer4->Add(m_graphbackcolor, wxGBPosition(i,1));
    i++;
    gridSizer4->Add(new wxStaticText(graph, wxID_ANY, _("Graph grid colour:")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_graphgridcolor = new wxColourPickerCtrl(graph, wxID_ANY, moptions.graphgridcolor());
    gridSizer4->Add(m_graphgridcolor, wxGBPosition(i,1));
    i++;
    gridSizer4->Add(new wxStaticText(graph, wxID_ANY, _("Download line colour:")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_graphlinecolor = new wxColourPickerCtrl(graph, wxID_ANY, moptions.graphlinecolor());
    gridSizer4->Add(m_graphlinecolor, wxGBPosition(i,1));
    i++;
    //NOTE 0.7.0 will not work as BT downloader/uploader
//    gridSizer4->Add(new wxStaticText(graph, wxID_ANY, _("Upload line colour:")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
//    m_graphuploadcolor = new wxColourPickerCtrl(graph, wxID_ANY, moptions.graphuploadcolor());
//    gridSizer4->Add(m_graphuploadcolor, wxGBPosition(i,1));
//    i++;
    gridSizer4->Add(new wxStaticText(graph, wxID_ANY, _("Speed font colour:")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_graphfontcolor = new wxColourPickerCtrl(graph, wxID_ANY, moptions.graphfontcolor());
    gridSizer4->Add(m_graphfontcolor, wxGBPosition(i,1));
    box4->Add(gridSizer4, 1, wxEXPAND|wxALL, 5);
    graph->SetSizer(box4);
    box4->Fit(graph);
    box4->SetSizeHints(graph);
    book->AddPage(graph, _("Graph"));

    i = 0;
    wxPanel *progress = new wxPanel(book);
    wxBoxSizer *box8 = new wxBoxSizer(wxHORIZONTAL);
    wxGridBagSizer *gridSizer8 = new wxGridBagSizer(5, 5);
    gridSizer8->Add(new wxStaticText(progress, wxID_ANY, _("Finished piece colour:")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_finishedpiece = new wxColourPickerCtrl(progress, wxID_ANY, moptions.finishedpiece());
    gridSizer8->Add(m_finishedpiece, wxGBPosition(i,1));
    i++;
    gridSizer8->Add(new wxStaticText(progress, wxID_ANY, _("Unfinished piece colour:")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_unfinishedpiece = new wxColourPickerCtrl(progress, wxID_ANY, moptions.unfinishedpiece());
    gridSizer8->Add(m_unfinishedpiece, wxGBPosition(i,1));
    box8->Add(gridSizer8, 1, wxEXPAND|wxALL, 5);
    progress->SetSizer(box8);
    box8->Fit(progress);
    box8->SetSizeHints(progress);
    book->AddPage(progress, _("Progress"));

    i = 0;
    wxPanel *scheduling = new wxPanel(book);
    wxBoxSizer *box5 = new wxBoxSizer(wxHORIZONTAL);
    wxGridBagSizer *gridSizer5 = new wxGridBagSizer(5, 5);
    m_activatescheduling = new wxCheckBox(scheduling, wxID_ANY, _("Activate scheduling"));
    m_activatescheduling->SetValue(moptions.activatescheduling());
    gridSizer5->Add(m_activatescheduling, wxGBPosition(i,0), wxGBSpan(1,2), wxEXPAND);
    i++;
    gridSizer5->Add(new wxStaticText(scheduling, wxID_ANY, _("Start date:")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    gridSizer5->Add(new wxStaticText(scheduling, wxID_ANY, _("Start time:")), wxGBPosition(i,1), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    i++;
    m_startdatetimeDate = new wxDatePickerCtrl(scheduling, ID_OPTIONS_STARTDATE, moptions.startdatetime());
    gridSizer5->Add(m_startdatetimeDate, wxGBPosition(i,0));
    m_startdatetimeTime = new wxTimePickerCtrl(scheduling, ID_OPTIONS_STARTTIME, moptions.startdatetime());
    gridSizer5->Add(m_startdatetimeTime, wxGBPosition(i,1));
    i++;
    gridSizer5->Add(new wxStaticText(scheduling, wxID_ANY, _("Finish date:")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    gridSizer5->Add(new wxStaticText(scheduling, wxID_ANY, _("Finish time:")), wxGBPosition(i,1), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    i++;
    m_finishdatetimeDate = new wxDatePickerCtrl(scheduling, ID_OPTIONS_FINISHDATE, moptions.finishdatetime());
    gridSizer5->Add(m_finishdatetimeDate, wxGBPosition(i,0));
    m_finishdatetimeTime = new wxTimePickerCtrl(scheduling, ID_OPTIONS_FINISHTIME, moptions.finishdatetime());
    gridSizer5->Add(m_finishdatetimeTime, wxGBPosition(i,1));
    i++;
    gridSizer5->Add(new wxStaticText(scheduling, wxID_ANY, _("Deactivate downloads on:")), wxGBPosition(i,0), wxGBSpan(1,2), wxALIGN_CENTER_VERTICAL);
    i++;
    wxBoxSizer *box5a = new wxBoxSizer(wxHORIZONTAL);
    m_exceptionday = new wxChoice(scheduling, wxID_ANY);
    m_exceptionday->Append(wxDateTime::GetWeekDayName(wxDateTime::Sun));
    m_exceptionday->Append(wxDateTime::GetWeekDayName(wxDateTime::Mon));
    m_exceptionday->Append(wxDateTime::GetWeekDayName(wxDateTime::Tue));
    m_exceptionday->Append(wxDateTime::GetWeekDayName(wxDateTime::Wed));
    m_exceptionday->Append(wxDateTime::GetWeekDayName(wxDateTime::Thu));
    m_exceptionday->Append(wxDateTime::GetWeekDayName(wxDateTime::Fri));
    m_exceptionday->Append(wxDateTime::GetWeekDayName(wxDateTime::Sat));
    m_exceptionday->SetSelection(0);
    box5a->Add(m_exceptionday, 0, wxALIGN_CENTER_VERTICAL, 2);
    box5a->Add(new wxStaticText(scheduling, wxID_ANY, _("Between")), 0, wxALIGN_CENTER_VERTICAL, 2);
    m_exceptionstart = new wxTimePickerCtrl(scheduling, wxID_ANY, wxDateTime::Today());
    box5a->Add(m_exceptionstart, 0, wxALIGN_CENTER_VERTICAL, 2);
    box5a->Add(new wxStaticText(scheduling, wxID_ANY, _("and")), 0, wxALIGN_CENTER_VERTICAL, 2);
    m_exceptionfinish = new wxTimePickerCtrl(scheduling, wxID_ANY, wxDateTime::Today());
    box5a->Add(m_exceptionfinish, 0, wxALIGN_CENTER_VERTICAL, 2);
    box5a->AddSpacer(7);
    m_exceptionadd = new wxButton(scheduling, ID_OPTIONS_EXCEPTION_ADD, _("Add"));
    box5a->Add(m_exceptionadd, 0, wxALIGN_CENTER_VERTICAL, 2);
    gridSizer5->Add(box5a, wxGBPosition(i,0), wxGBSpan(1,2));
    i++;
    wxBoxSizer *box5b = new wxBoxSizer(wxHORIZONTAL);
    m_exceptionsList = new wxListBox(scheduling, wxID_ANY);
    for(mScheduleException e : m_exceptions)
    {
        m_exceptionsList->Append(e.ToWxstring());
    }
    m_exceptionsList->SetMinSize(wxSize(300,220));
    box5b->Add(m_exceptionsList);
    box5b->AddSpacer(7);
    m_exceptionremove = new wxButton(scheduling, ID_OPTIONS_EXCEPTION_REMOVE, _("Remove"));
    box5b->Add(m_exceptionremove);
    gridSizer5->Add(box5b, wxGBPosition(i,0), wxGBSpan(1,2));
    box5->Add(gridSizer5, 1, wxEXPAND|wxALL, 5);
    scheduling->SetSizer(box5);
    box5->Fit(scheduling);
    box5->SetSizeHints(scheduling);
    book->AddPage(scheduling, _("Scheduling"));

    i = 0;
    wxPanel *bandwidth = new wxPanel(book);
    wxBoxSizer *box6 = new wxBoxSizer(wxHORIZONTAL);
    wxGridBagSizer *gridSizer6 = new wxGridBagSizer(5, 5);
    m_bandwidthunlimited = new wxRadioButton(bandwidth, wxID_ANY, _("Unlimited bandwidth"));
    gridSizer6->Add(m_bandwidthunlimited, wxGBPosition(i,0));
    i++;
    m_bandwidthindependently = new wxRadioButton(bandwidth, wxID_ANY, _("Limit band width independently for each download"));
    gridSizer6->Add(m_bandwidthindependently, wxGBPosition(i,0));
    i++;
    m_bandwidthcustom = new wxRadioButton(bandwidth, wxID_ANY, _("Limit band width for the program (in KB/s):"));
    gridSizer6->Add(m_bandwidthcustom, wxGBPosition(i,0));
    m_bandwidthcustomSpin = new wxSpinCtrl(bandwidth);
    m_bandwidthcustomSpin->SetRange(1,4096);
    m_bandwidthcustomSpin->SetValue(moptions.bandwidth());
    gridSizer6->Add(m_bandwidthcustomSpin, wxGBPosition(i,1));
    if(moptions.bandwidthoption() == 0)
    {
        m_bandwidthunlimited->SetValue(true);
    }
    else if(moptions.bandwidthoption() == 1)
    {
        m_bandwidthindependently->SetValue(true);
    }
    else
    {
        m_bandwidthcustom->SetValue(true);
    }
    box6->Add(gridSizer6, 1, wxEXPAND|wxALL, 5);
    bandwidth->SetSizer(box6);
    box6->Fit(bandwidth);
    box6->SetSizeHints(bandwidth);
    book->AddPage(bandwidth, _("Bandwidth"));

    i = 0;
    wxPanel *browser = new wxPanel(book);
    wxBoxSizer *box7 = new wxBoxSizer(wxHORIZONTAL);
    wxGridBagSizer *gridSizer7 = new wxGridBagSizer(5, 5);
    gridSizer7->Add(new wxStaticText(browser, wxID_ANY, _("Please visit")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    i++;
    gridSizer7->Add(new wxHyperlinkCtrl(browser, wxID_ANY, _("wxDownload Fast Web Extensions"), "https://wxdfast.dxsolutions.org/extensions.html"), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    box7->Add(gridSizer7, 1, wxEXPAND|wxALL, 5);
    browser->SetSizer(box7);
    box7->Fit(browser);
    box7->SetSizeHints(browser);
    book->AddPage(browser, _("Browser Integration"));

    mainSizer->Add(book);

    wxStdDialogButtonSizer *btnSizer = new wxStdDialogButtonSizer();
    wxButton* okButton = new wxButton(this, wxID_OK, "", wxDefaultPosition, wxDefaultSize, 0);
    btnSizer->AddButton(okButton);
    wxButton* cancelButton = new wxButton(this, wxID_CANCEL, "", wxDefaultPosition, wxDefaultSize, 0);
    btnSizer->AddButton(cancelButton);
    btnSizer->Realize();
    mainSizer->Add(btnSizer, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);

    this->SetSizer(mainSizer);
    mainSizer->Fit(this);
    mainSizer->SetSizeHints(this);
}

void mOptionsDialog::OnOk(wxCommandEvent &/*event*/)
{
    //General
    moptions.setAttempts(m_attempts->GetValue());
    moptions.setClosedialog(m_closedialog->GetValue());
    moptions.setSimultaneous(m_simultaneous->GetValue());
    moptions.setAttemptstime(m_attemptstime->GetValue());
    moptions.setShutdown(m_shutdown->GetValue());
    moptions.setDisconnect(m_disconnect->GetValue());
    moptions.setTimerupdateinterval(m_timerupdateinterval->GetValue());
    moptions.setRestoremainframe(m_restoremainframe->GetValue());
    moptions.setHidemainframe(m_hidemainframe->GetValue());
    moptions.setCheckforupdates(m_checkforupdates->GetValue());
    moptions.setRememberboxnewoptions(m_rememberboxnewoptions->GetValue());
    //Graph
    moptions.setGraphshow(m_graphshow->GetValue());
    moptions.setGraphrefreshtime(m_graphrefreshtime->GetValue());
    if(m_graphscale->GetSelection()==0)
        moptions.setGraphscale(500);
    else if(m_graphscale->GetSelection()==1)
        moptions.setGraphscale(1024);
    else if(m_graphscale->GetSelection()==2)
        moptions.setGraphscale(5*1024);
    else if(m_graphscale->GetSelection()==3)
        moptions.setGraphscale(10*1024);
    else if(m_graphscale->GetSelection()==4)
        moptions.setGraphscale(20*1024);
    else if(m_graphscale->GetSelection()==5)
        moptions.setGraphscale(50*1024);
    else if(m_graphscale->GetSelection()==6)
        moptions.setGraphscale(100*1024);
    else
        moptions.setGraphscale(500*1024);
    moptions.setGraphtextarea(m_graphtextarea->GetValue());
    moptions.setGraphheight(m_graphheight->GetValue());
    moptions.setGraphspeedfontsize(m_graphspeedfontsize->GetValue());
    moptions.setGraphbackcolor(m_graphbackcolor->GetColour());
    moptions.setGraphgridcolor(m_graphgridcolor->GetColour());
    moptions.setGraphlinecolor(m_graphlinecolor->GetColour());
    //NOTE 0.7.0 will not work as BT downloader/uploader
//    moptions.setGraphuploadcolor(m_graphuploadcolor->GetColour());
    moptions.setGraphlinewidth(m_graphlinewidth->GetValue());
    moptions.setGraphfontcolor(m_graphfontcolor->GetColour());
    //Path
    moptions.setShutdowncmd(m_shutdowncmd->GetValue());
    moptions.setDisconnectcmd(m_disconnectcmd->GetValue());
    moptions.setDestination(m_destination->GetValue());
    /** We use explorer on Win, xdg-open on *nix, open on Mac
    moptions.setFilemanagerpath(m_filemanagerpath->GetValue());
    */
    //Schedule
    moptions.setActivatescheduling(m_activatescheduling->GetValue());
    moptions.setStartdatetime(GetStart());
    moptions.setFinishdatetime(GetFinish());
    moptions.clearScheduleExceptions();
    size_t i =0;
    for(const mScheduleException &ex : m_exceptions)
    {
        if(ex.isactive)
        {
            moptions.setScheduleException(i,ex);
        }
        i++;
    }
    //Bandwidth
    if(m_bandwidthunlimited->GetValue())
        moptions.setBandwidthoption(0);
    else if(m_bandwidthindependently->GetValue())
        moptions.setBandwidthoption(1);
    else
        moptions.setBandwidthoption(2);
    moptions.setBandwidth(m_bandwidthcustomSpin->GetValue());
    //Proxy
    moptions.setProxy(m_proxy->GetValue());
    moptions.setProxyServer(m_proxyServer->GetValue());
    moptions.setProxyPort(m_proxyPort->GetValue());
    moptions.setProxyUsername(m_proxyUsername->GetValue());
    moptions.setProxyAuthstring(m_proxyAuthstring->GetValue());
    //Progress
    moptions.setFinishedpiece(m_finishedpiece->GetColour());
    moptions.setUnfinishedpiece(m_unfinishedpiece->GetColour());
    EndModal(wxID_OK);
}

void mOptionsDialog::OnCancel(wxCommandEvent &/*event*/)
{
    EndModal(wxID_CANCEL);
}

void mOptionsDialog::OnDestination(wxCommandEvent &/*event*/)
{
    wxString dir = wxDirSelector(_("Select the directory"), wxGetHomeDir());
    if(!dir.IsEmpty())
    {
        m_destination->SetValue(dir);
        m_destination->SetToolTip(dir);
    }
}

void mOptionsDialog::OnFilemanager(wxCommandEvent &/*event*/)
{
    wxFileDialog dialog(this, _("Choose a file"), wxEmptyString, wxEmptyString, "*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if(dialog.ShowModal() == wxID_OK)
    {
        m_filemanagerpath->SetValue(dialog.GetPath());
        m_filemanagerpath->SetToolTip(dialog.GetPath());
    }
}

void mOptionsDialog::OnStartDate(wxDateEvent &/*event*/)
{
    if(GetStart().IsLaterThan(GetFinish()))
    {
        wxDateTime finish = GetStart()+wxTimeSpan::Minute();
        m_finishdatetimeDate->SetValue(finish);
        m_finishdatetimeTime->SetValue(finish);
    }
}

void mOptionsDialog::OnStartTime(wxDateEvent &/*event*/)
{
    if(GetStart().IsLaterThan(GetFinish()))
    {
        wxDateTime finish = GetStart()+wxTimeSpan::Minute();
        m_finishdatetimeDate->SetValue(finish);
        m_finishdatetimeTime->SetValue(finish);
    }
}

void mOptionsDialog::OnFinishDate(wxDateEvent &/*event*/)
{
    if(GetStart().IsLaterThan(GetFinish()))
    {
        wxDateTime start = GetFinish()-wxTimeSpan::Minute();
        m_startdatetimeDate->SetValue(start);
        m_startdatetimeTime->SetValue(start);
    }
}

void mOptionsDialog::OnFinishTime(wxDateEvent &/*event*/)
{
    if(GetStart().IsLaterThan(GetFinish()))
    {
        wxDateTime start = GetFinish()-wxTimeSpan::Minute();
        m_startdatetimeDate->SetValue(start);
        m_startdatetimeTime->SetValue(start);
    }
}

void mOptionsDialog::OnAddException(wxCommandEvent &/*event*/)
{
    if(m_exceptionsList->GetCount() < MAX_SCHEDULE_EXCEPTIONS)
    {
        if(m_exceptionstart->GetValue().IsEqualTo(m_exceptionfinish->GetValue()) || m_exceptionstart->GetValue().IsLaterThan(m_exceptionfinish->GetValue()))
        {
            m_exceptionfinish->SetValue(m_exceptionstart->GetValue()+wxTimeSpan::Minute());
        }
        mScheduleException exception;
        exception.isactive = true;
        exception.day = m_exceptionday->GetSelection();
        exception.startHour = m_exceptionstart->GetValue().GetHour();
        exception.startMinute = m_exceptionstart->GetValue().GetMinute();
        exception.finishHour = m_exceptionfinish->GetValue().GetHour();
        exception.finishMinute = m_exceptionfinish->GetValue().GetMinute();
        m_exceptions.push_back(exception);
        m_exceptionsList->Insert(exception.ToWxstring(),0);
    }
    else
    {
        wxMessageBox(_("Remove some items before adding a new one."));
    }
}

void mOptionsDialog::OnRemoveException(wxCommandEvent &/*event*/)
{
    std::vector<mScheduleException>::const_iterator it = m_exceptions.begin();
    for(unsigned int i=0; i < m_exceptionsList->GetCount(); i++, it++)
    {
        if(m_exceptionsList->IsSelected(i))
        {
            m_exceptionsList->Delete(i);
            m_exceptions.erase(it);
            break;
        }
    }
}

void mOptionsDialog::OnShowNotify(wxCommandEvent &/*event*/)
{
    bool haveNotify = false;
#ifdef __WXGTK__
    wxString contents;
    if(wxGetEnv("PATH", &contents))
    {
        wxArrayString path = wxStringTokenize(contents, wxPATH_SEP);
        for(size_t i=0; i<path.GetCount(); i++)
        {
            if(wxFileExists(path[i]+wxFILE_SEP_PATH+"notify-send"))
            {
                haveNotify = true;
                break;
            }
        }
    }
#endif // __WXGTK__
    if(haveNotify)
    {
        wxExecute(wxString::Format("notify-send -i %s/icons/wxdfast.png 'wxdfast.tar.gz' '%s'", WXDFAST_DATADIR, _("Downloaded successfully")));
        return;
    }
    wxNotificationMessage msg("wxdfast.tar.gz", _("Downloaded successfully"));
    msg.Show();
}

wxDateTime mOptionsDialog::GetStart()
{
    wxDateTime start = m_startdatetimeDate->GetValue();
    start.SetHour(m_startdatetimeTime->GetValue().GetHour());
    start.SetMinute(m_startdatetimeTime->GetValue().GetMinute());
    start.SetSecond(m_startdatetimeTime->GetValue().GetSecond());
    return start;
}

wxDateTime mOptionsDialog::GetFinish()
{
    wxDateTime finish = m_finishdatetimeDate->GetValue();
    finish.SetHour(m_finishdatetimeTime->GetValue().GetHour());
    finish.SetMinute(m_finishdatetimeTime->GetValue().GetMinute());
    finish.SetSecond(m_finishdatetimeTime->GetValue().GetSecond());
    return finish;
}

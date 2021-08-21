/***************************************************************
 * Name:      OptionsDialog.h
 * Author:    Max Magalhães Velasques (maxvelasques@gmail.com)
 *            David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2006
 * License:   GPL2
 **************************************************************/

#ifndef MOPTIONSDIALOG_H
#define MOPTIONSDIALOG_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/spinctrl.h>
#include <wx/clrpicker.h>
#include <wx/datectrl.h>
#include <wx/timectrl.h>
#include <wx/dateevt.h>
#include <wx/listctrl.h>
#include <vector>
#include "Options.h"

class mOptionsDialog: public wxDialog
{
    DECLARE_DYNAMIC_CLASS(OptionsDialog)
    DECLARE_EVENT_TABLE()
public:
    mOptionsDialog() {}
    mOptionsDialog(wxWindow *parent);
private:
    wxSpinCtrl *m_attempts;
    wxSpinCtrl *m_attemptstime;
    wxSpinCtrl *m_simultaneous;
    wxCheckBox *m_rememberboxnewoptions;
    wxCheckBox *m_shutdown;
    wxCheckBox *m_disconnect;
    wxCheckBox *m_closedialog;
    wxSpinCtrl *m_timerupdateinterval;
    wxCheckBox *m_restoremainframe;
    wxCheckBox *m_hidemainframe;
    wxCheckBox *m_checkforupdates;
    wxTextCtrl *m_destination;
    wxButton *m_destinationSelect;
    wxTextCtrl *m_filemanagerpath;
    wxButton *m_filemanagerpathSelect;
    wxTextCtrl *m_shutdowncmd;
    wxTextCtrl *m_disconnectcmd;
    wxCheckBox *m_proxy;
    wxTextCtrl *m_proxyServer;
    wxTextCtrl *m_proxyPort;
    wxTextCtrl *m_proxyUsername;
    wxTextCtrl *m_proxyAuthstring;
    wxCheckBox *m_graphshow;
    wxSpinCtrl *m_graphrefreshtime;
    wxChoice *m_graphscale;
    wxSpinCtrl *m_graphtextarea;
    wxSpinCtrl *m_graphheight;
    wxSpinCtrl *m_graphspeedfontsize;
    wxSpinCtrl *m_graphlinewidth;
    wxColourPickerCtrl *m_graphbackcolor;
    wxColourPickerCtrl *m_graphgridcolor;
    wxColourPickerCtrl *m_graphlinecolor;
    wxColourPickerCtrl *m_graphuploadcolor;
    wxColourPickerCtrl *m_graphfontcolor;
    wxCheckBox *m_activatescheduling;
    wxDatePickerCtrl *m_startdatetimeDate;
    wxTimePickerCtrl *m_startdatetimeTime;
    wxDatePickerCtrl *m_finishdatetimeDate;
    wxTimePickerCtrl *m_finishdatetimeTime;
    wxChoice *m_exceptionday;
    wxTimePickerCtrl *m_exceptionstart;
    wxTimePickerCtrl *m_exceptionfinish;
    wxButton *m_exceptionadd;
    std::vector<mScheduleException> m_exceptions;
    wxListBox *m_exceptionsList;
    wxButton *m_exceptionremove;
    wxRadioButton *m_bandwidthunlimited;
    wxRadioButton *m_bandwidthindependently;
    wxRadioButton *m_bandwidthcustom;
    wxSpinCtrl *m_bandwidthcustomSpin;
    wxColourPickerCtrl *m_finishedpiece;
    wxColourPickerCtrl *m_unfinishedpiece;
    wxCheckBox *m_shownotify;
    wxButton *m_shownotifyTest;

    void OnOk(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnDestination(wxCommandEvent& event);
    void OnFilemanager(wxCommandEvent& event);
    void OnStartDate(wxDateEvent& event);
    void OnStartTime(wxDateEvent& event);
    void OnFinishDate(wxDateEvent& event);
    void OnFinishTime(wxDateEvent& event);
    void OnAddException(wxCommandEvent& event);
    void OnRemoveException(wxCommandEvent& event);
    void OnShowNotify(wxCommandEvent& event);
    wxDateTime GetStart();
    wxDateTime GetFinish();
};

#endif // MOPTIONSDIALOG_H

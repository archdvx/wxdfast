/***************************************************************
 * Name:      BatchDialog.h
 * Author:    David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2021
 * License:   GPL2
 **************************************************************/

#ifndef MBATCHDIALOG_H
#define MBATCHDIALOG_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/spinctrl.h>

class mBatchDialog : public wxDialog
{
    DECLARE_DYNAMIC_CLASS(mBatchDialog)
    DECLARE_EVENT_TABLE()
public:
    mBatchDialog() {}
    mBatchDialog(wxWindow *parent);

    void SetDestination(const wxString &destination);
    void SetNumberOfParts(int numberofparts);
    void SetStartOption(int startoption);
    void SetCommand(const wxString &command);
    void SetBandwidth(int bandwidth);
    void SetUrls(const wxArrayString &urls);
    void SetUser(const wxString &user);
    void SetPassword(const wxString &password);
    void SetComment(const wxString &comment);
    void SetReferenceURL(const wxString &url);
    wxString GetDestination() const;
    int GetNumberOfParts() const;
    int GetStartOption() const;
    wxString GetCommand() const;
    int GetBandwidth() const;
    wxArrayString GetUrls();
    wxString GetUser() const;
    wxString GetPassword() const;
    wxString GetComment() const;
    wxString GetReferenceURL() const;
    void ReLayout();
    void EnableAdresslist(bool enable);
    void EnableUser(bool enable);
    void EnablePassword(bool enable);
    void EnableManual(bool enable);
    void EnableNow(bool enable);
    void EnableSchedule(bool enable);
    void EnableSplit(bool enable);
    void EnableComment(bool enable);
    void EnableBandwidth(bool enable);
    void EnableReferenceurl(bool enable);
    void EnableCommand(bool enable);
private:
    wxBoxSizer *m_mainSizer;
    wxTextCtrl *m_destination;
    wxCheckListBox *m_adresslist;
    wxTextCtrl *m_user;
    wxTextCtrl *m_password;
    wxRadioButton *m_manual;
    wxRadioButton *m_now;
    wxRadioButton *m_schedule;
    wxSpinCtrl *m_split;
    wxTextCtrl *m_comment;
    wxSpinCtrl *m_bandwidth;
    wxTextCtrl *m_referenceurl;
    wxTextCtrl *m_command;

    void OnOk(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnDir(wxCommandEvent& event);
};

#endif // MBATCHDIALOG_H

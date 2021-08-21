/***************************************************************
 * Name:      NewDialog.h
 * Author:    Max Magalhães Velasques (maxvelasques@gmail.com)
 *            David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2006
 * License:   GPL2
 **************************************************************/

#ifndef MNEWDIALOG_H
#define MNEWDIALOG_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/spinctrl.h>
#include <wx/uri.h>

class mNewDialog : public wxDialog
{
    DECLARE_DYNAMIC_CLASS(mNewDialog)
    DECLARE_EVENT_TABLE()
public:
    mNewDialog() {}
    mNewDialog(wxWindow *parent);

    void SetDestination(const wxString &destination);
    void SetNumberOfConnections(int numberofconnections);
    void SetStartOption(int startoption);
    void SetCommand(const wxString &command);
    void SetOnTop(bool ontop);
    void SetBandwidth(int bandwidth);
    void SetUrls(const wxArrayString &urls);
    void SetUrl(const wxString &url);
    void SetUser(const wxString &user);
    void SetPassword(const wxString &password);
    void SetComment(const wxString &comment);
    void SetReferenceURL(const wxString &url);
    wxString GetDestination() const;
    int GetNumberOfConnections() const;
    int GetStartOption() const;
    wxString GetCommand() const;
    bool GetOnTop() const;
    int GetBandwidth() const;
    wxArrayString GetUrls();
    wxString GetUser() const;
    wxString GetPassword() const;
    wxString GetComment() const;
    wxString GetReferenceURL() const;
    void ReLayout();
    void EnableDestination(bool enable);
    void EnableDirectory(bool enable);
    void EnableAdd(bool enable);
    void EnableEdit(bool enable);
    void EnableAdresslist(bool enable);
    void EnableOntop(bool enable);
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
    void EnableUrl(bool enable);
private:
    wxBoxSizer *m_mainSizer;
    wxTextCtrl *m_destination;
    wxButton *m_directory;
    wxTextCtrl *m_url;
    wxButton *m_add;
    wxCheckListBox *m_adresslist;
    wxButton *m_edit;
    wxCheckBox *m_addontop;
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
    void OnAdd(wxCommandEvent& event);
    void OnEdit(wxCommandEvent& event);
    int CheckURI(wxString uritxt);
};

#endif // MNEWDIALOG_H

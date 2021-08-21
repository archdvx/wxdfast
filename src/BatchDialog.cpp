/***************************************************************
 * Name:      BatchDialog.cpp
 * Author:    David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2021
 * License:   GPL2
 **************************************************************/

#include <wx/gbsizer.h>
#include <wx/listbook.h>
#include "BatchDialog.h"
#include "Defs.h"
#include "UtilFunctions.h"
#include "Options.h"

IMPLEMENT_DYNAMIC_CLASS(mBatchDialog, wxDialog)

BEGIN_EVENT_TABLE(mBatchDialog, wxDialog)
    EVT_BUTTON(wxID_OK, mBatchDialog::OnOk)
    EVT_BUTTON(wxID_CANCEL, mBatchDialog::OnCancel)
    EVT_BUTTON(ID_BATCH_DIRECTORY, mBatchDialog::OnDir)
END_EVENT_TABLE()

mBatchDialog::mBatchDialog(wxWindow *parent)
    : wxDialog(parent, wxID_ANY, _("Add new downloads"))
{
    int i =0;
    m_mainSizer = new wxBoxSizer(wxVERTICAL);
    wxGridBagSizer *gridSizer = new wxGridBagSizer(5, 5);
    gridSizer->Add(new wxStaticText(this, wxID_ANY, _("Save to:")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_destination = new wxTextCtrl(this, wxID_ANY, moptions.destination());
    m_destination->SetMinSize(wxSize(222,-1));
    m_destination->SetToolTip(moptions.destination());
    gridSizer->Add(m_destination, wxGBPosition(i,1), wxDefaultSpan, wxEXPAND);
    gridSizer->Add(new wxButton(this, ID_BATCH_DIRECTORY, _("Directory")), wxGBPosition(i,2), wxDefaultSpan, wxEXPAND);
    i++;
    gridSizer->Add(new wxStaticText(this, wxID_ANY, _("Address List:")), wxGBPosition(i,0), wxGBSpan(1,2));
    i++;
    m_adresslist = new wxCheckListBox(this, wxID_ANY);
    m_adresslist->SetToolTip(_("Only the selected addresses will be downloaded"));
    m_adresslist->SetMaxClientSize(wxSize(450,-1));
    gridSizer->Add(m_adresslist, wxGBPosition(i,0), wxGBSpan(1,2), wxEXPAND);
    i++;
    wxStaticBoxSizer *sbox1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Server Permission"));
    wxGridBagSizer *gridSizer1 = new wxGridBagSizer(5, 5);
    gridSizer1->Add(new wxStaticText(this, wxID_ANY, _("User:")), wxGBPosition(0,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_user = new wxTextCtrl(this, wxID_ANY);
    m_user->SetMinSize(wxSize(222,-1));
    gridSizer1->Add(m_user, wxGBPosition(0,1), wxDefaultSpan, wxALL|wxEXPAND);
    gridSizer1->Add(new wxStaticText(this, wxID_ANY, _("Password:")), wxGBPosition(1,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_password = new wxTextCtrl(this, wxID_ANY);
    gridSizer1->Add(m_password, wxGBPosition(1,1), wxDefaultSpan, wxALL|wxEXPAND);
    sbox1->Add(gridSizer1);
    gridSizer->Add(sbox1, wxGBPosition(i,0), wxGBSpan(1,2), wxEXPAND);
    wxStaticBoxSizer *sbox2 = new wxStaticBoxSizer(wxVERTICAL, this, _("Start Download"));
    m_manual = new wxRadioButton(this, wxID_ANY, _("Manual"));
    sbox2->Add(m_manual, 0);
    m_now = new wxRadioButton(this, wxID_ANY, _("Now"));
    m_now->SetValue(true);
    sbox2->Add(m_now, 0);
    m_schedule = new wxRadioButton(this, wxID_ANY, _("Schedule"));
    sbox2->Add(m_schedule, 0);
    gridSizer->Add(sbox2, wxGBPosition(i,2), wxGBSpan(1,2), wxEXPAND);
    i++;
    wxStaticBoxSizer *sbox3 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Segmented download"));
    wxGridBagSizer *gridSizer3 = new wxGridBagSizer(5, 5);
    gridSizer3->Add(new wxStaticText(this, wxID_ANY, _("Split download into how many\npieces?")), wxGBPosition(0,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_split = new wxSpinCtrl(this, wxID_ANY, "5");
    m_split->SetRange(1,30);
    gridSizer3->Add(m_split, wxGBPosition(1,0), wxDefaultSpan, wxALL|wxEXPAND);
    sbox3->Add(gridSizer3);
    gridSizer->Add(sbox3, wxGBPosition(i,0), wxGBSpan(1,2), wxEXPAND);
    wxStaticBoxSizer *sbox4 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Comments"));
    wxGridBagSizer *gridSizer4 = new wxGridBagSizer(5, 5);
    m_comment = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    m_comment->SetMinClientSize(wxSize(200,80));
    gridSizer4->Add(m_comment, wxGBPosition(0,0), wxDefaultSpan, wxALL|wxEXPAND);
    sbox4->Add(gridSizer4);
    gridSizer->Add(sbox4, wxGBPosition(i,2), wxGBSpan(1,2), wxEXPAND);
    i++;
    wxStaticBoxSizer *sbox5 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Download bandwidth (in KB/s)"));
    m_bandwidth = new wxSpinCtrl(this, wxID_ANY);
    m_bandwidth->SetRange(0,5*1024);
    m_bandwidth->SetToolTip(_("Set band width limit for this download.\nSet \"0\" to unlimited."));
    sbox5->Add(m_bandwidth);
    gridSizer->Add(sbox5, wxGBPosition(i,0), wxGBSpan(1,2));
    i++;
    wxStaticBoxSizer *sbox6 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Reference URL"));
    m_referenceurl = new wxTextCtrl(this, wxID_ANY);
    sbox6->Add(m_referenceurl, 1, wxEXPAND, 5);
    gridSizer->Add(sbox6, wxGBPosition(i,0), wxGBSpan(1,4), wxEXPAND);
    i++;
    wxStaticBoxSizer *sbox7 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Execute the following command after download completes"));
    m_command = new wxTextCtrl(this, wxID_ANY);
    sbox7->Add(m_command, 1, wxEXPAND, 5);
    gridSizer->Add(sbox7, wxGBPosition(i,0), wxGBSpan(1,4), wxEXPAND);
    m_mainSizer->Add(gridSizer, 1, wxEXPAND|wxALL, 5);

    wxStdDialogButtonSizer *btnSizer = new wxStdDialogButtonSizer();
    wxButton* okButton = new wxButton(this, wxID_OK, "", wxDefaultPosition, wxDefaultSize, 0);
    btnSizer->AddButton(okButton);
    wxButton* cancelButton = new wxButton(this, wxID_CANCEL, "", wxDefaultPosition, wxDefaultSize, 0);
    btnSizer->AddButton(cancelButton);
    btnSizer->Realize();
    m_mainSizer->Add(btnSizer, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);

    this->SetSizer(m_mainSizer);
    m_mainSizer->Fit(this);
    m_mainSizer->SetSizeHints(this);
}

void mBatchDialog::SetDestination(const wxString &destination)
{
    m_destination->SetValue(destination);
}

void mBatchDialog::SetNumberOfParts(int numberofparts)
{
    m_split->SetValue(numberofparts);
}

void mBatchDialog::SetStartOption(int startoption)
{
    if(startoption == 0)
        m_manual->SetValue(true);
    else if(startoption == 1)
        m_now->SetValue(true);
    else
        m_schedule->SetValue(true);
}

void mBatchDialog::SetCommand(const wxString &command)
{
    m_command->SetValue(command);
}

void mBatchDialog::SetBandwidth(int bandwidth)
{
    m_bandwidth->SetValue(bandwidth);
}

void mBatchDialog::SetUrls(const wxArrayString &urls)
{
    m_adresslist->Clear();
    m_adresslist->Append(urls);
    for(unsigned int i=0; i < m_adresslist->GetCount(); i++)
    {
        m_adresslist->Check(i);
    }
}

void mBatchDialog::SetUser(const wxString &user)
{
    m_user->SetValue(user);
}

void mBatchDialog::SetPassword(const wxString &password)
{
    m_password->SetValue(password);
}

void mBatchDialog::SetComment(const wxString &comment)
{
    m_comment->SetValue(comment);
}

void mBatchDialog::SetReferenceURL(const wxString &url)
{
    m_referenceurl->SetValue(url);
}

wxString mBatchDialog::GetDestination() const
{
    return m_destination->GetValue();
}

int mBatchDialog::GetNumberOfParts() const
{
    return m_split->GetValue();
}

int mBatchDialog::GetStartOption() const
{
    if(m_manual->GetValue())
        return 0;
    if(m_now->GetValue())
        return 1;
    return 2;
}

wxString mBatchDialog::GetCommand() const
{
    return m_command->GetValue();
}

int mBatchDialog::GetBandwidth() const
{
    return m_bandwidth->GetValue();
}

wxArrayString mBatchDialog::GetUrls()
{
    wxArrayString urls;
    for(unsigned int i=0; i < m_adresslist->GetCount(); i++)
    {
        if(m_adresslist->IsChecked(i))
            urls.Add(m_adresslist->GetString(i));
    }
    return urls;
}

wxString mBatchDialog::GetUser() const
{
    return m_user->GetValue();
}

wxString mBatchDialog::GetPassword() const
{
    return m_password->GetValue();
}

wxString mBatchDialog::GetComment() const
{
    return m_comment->GetValue();
}

wxString mBatchDialog::GetReferenceURL() const
{
    return m_referenceurl->GetValue();
}

void mBatchDialog::ReLayout()
{
    m_mainSizer->Layout();
    m_mainSizer->Fit(this);
}

void mBatchDialog::EnableAdresslist(bool enable)
{
    m_adresslist->Enable(enable);
}

void mBatchDialog::EnableUser(bool enable)
{
    m_user->Enable(enable);
}

void mBatchDialog::EnablePassword(bool enable)
{
    m_password->Enable(enable);
}

void mBatchDialog::EnableManual(bool enable)
{
    m_manual->Enable(enable);
}

void mBatchDialog::EnableNow(bool enable)
{
    m_now->Enable(enable);
}

void mBatchDialog::EnableSchedule(bool enable)
{
    m_schedule->Enable(enable);
}

void mBatchDialog::EnableSplit(bool enable)
{
    m_split->Enable(enable);
}

void mBatchDialog::EnableComment(bool enable)
{
    m_comment->Enable(enable);
}

void mBatchDialog::EnableBandwidth(bool enable)
{
    m_bandwidth->Enable(enable);
}

void mBatchDialog::EnableReferenceurl(bool enable)
{
    m_referenceurl->Enable(enable);
}

void mBatchDialog::EnableCommand(bool enable)
{
    m_command->Enable(enable);
}

void mBatchDialog::OnOk(wxCommandEvent &/*event*/)
{
    if(!GetUrls().GetCount())
    {
        wxMessageBox(_("The URLs are empty"), _("Warning..."), wxOK|wxICON_WARNING, this);
        m_adresslist->SetFocus();
        return;
    }
    if(!wxDirExists(m_destination->GetValue()))
    {
        wxMessageBox(_("The destination doesn't exist"), _("Warning..."), wxOK|wxICON_WARNING, this);
        wxCommandEvent event(wxEVT_BUTTON, ID_NEW_DIRECTORY);
        wxPostEvent(this, event);
        return;
    }
    EndModal(wxID_OK);
}

void mBatchDialog::OnCancel(wxCommandEvent &/*event*/)
{
    EndModal(wxID_CANCEL);
}

void mBatchDialog::OnDir(wxCommandEvent &/*event*/)
{
    wxString dir = wxDirSelector(_("Select the directory:"), m_destination->GetValue());
    if(!dir.IsEmpty())
    {
        m_destination->SetValue(dir);
        m_destination->SetToolTip(dir);
    }
}

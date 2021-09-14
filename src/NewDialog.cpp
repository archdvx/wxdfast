/***************************************************************
 * Name:      NewDialog.cpp
 * Author:    Max Magalhães Velasques (maxvelasques@gmail.com)
 *            David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2006
 * License:   GPL2
 **************************************************************/

#include <wx/gbsizer.h>
#include <wx/listbook.h>
#include "NewDialog.h"
#include "Defs.h"
#include "UtilFunctions.h"
#include "Options.h"

IMPLEMENT_DYNAMIC_CLASS(mNewDialog, wxDialog)

BEGIN_EVENT_TABLE(mNewDialog, wxDialog)
    EVT_BUTTON(wxID_OK, mNewDialog::OnOk)
    EVT_BUTTON(wxID_CANCEL, mNewDialog::OnCancel)
    EVT_BUTTON(ID_NEW_DIRECTORY, mNewDialog::OnDir)
    EVT_BUTTON(ID_NEW_ADD, mNewDialog::OnAdd)
    EVT_BUTTON(ID_NEW_EDIT, mNewDialog::OnEdit)
END_EVENT_TABLE()

mNewDialog::mNewDialog(wxWindow *parent)
    : wxDialog(parent, wxID_ANY, _("Add new download"))
{
    int i =0;
    m_mainSizer = new wxBoxSizer(wxVERTICAL);
    wxGridBagSizer *gridSizer = new wxGridBagSizer(5, 5);
    gridSizer->Add(new wxStaticText(this, wxID_ANY, _("Save to:")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_destination = new wxTextCtrl(this, wxID_ANY, moptions.destination());
    m_destination->SetMinSize(wxSize(222,-1));
    m_destination->SetToolTip(moptions.destination());
    gridSizer->Add(m_destination, wxGBPosition(i,1), wxDefaultSpan, wxEXPAND);
    m_directory = new wxButton(this, ID_NEW_DIRECTORY, _("Directory"));
    gridSizer->Add(m_directory, wxGBPosition(i,2), wxDefaultSpan, wxEXPAND);
    i++;
    gridSizer->Add(new wxStaticText(this, wxID_ANY, _("Address:")), wxGBPosition(i,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_url = new wxTextCtrl(this, wxID_ANY);
    m_url->SetMinSize(wxSize(222,-1));
#if DEBUG
    m_url->SetValue("https://files.dxsolutions.org/dxfaktura-1.00.0-win.zip");
#endif
    m_url->SetToolTip(_("URL to download\nExample: http://www.foo.com/foo.exe\nExample: http://www.foo.com:80/foo.exe\nExample: ftp://ftp.foo.com:21/foo.exe"));
    gridSizer->Add(m_url, wxGBPosition(i,1), wxDefaultSpan, wxEXPAND);
    m_add = new wxButton(this, ID_NEW_ADD, _("Add"));
    gridSizer->Add(m_add, wxGBPosition(i,2));
    i++;
    gridSizer->Add(new wxStaticText(this, wxID_ANY, _("Address List:")), wxGBPosition(i,0), wxGBSpan(1,2));
    i++;
    m_adresslist = new wxCheckListBox(this, wxID_ANY);
    m_adresslist->SetToolTip(_("Only the selected addresses will be downloaded"));
    gridSizer->Add(m_adresslist, wxGBPosition(i,0), wxGBSpan(1,2), wxEXPAND);
    m_edit = new wxButton(this, ID_NEW_EDIT, _("&Edit"));
    gridSizer->Add(m_edit, wxGBPosition(i,2));
    i++;
    m_addontop = new wxCheckBox(this, wxID_ANY, _("Add on top of download queue"));
    gridSizer->Add(m_addontop, wxGBPosition(i,0), wxGBSpan(1,3), wxEXPAND);
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
    gridSizer3->Add(new wxStaticText(this, wxID_ANY, _("Split download into how many\nconnections?")), wxGBPosition(0,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
    m_split = new wxSpinCtrl(this, wxID_ANY, "5");
    m_split->SetRange(1,16);
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

void mNewDialog::SetDestination(const wxString &destination)
{
    m_destination->SetValue(destination);
}

void mNewDialog::SetNumberOfConnections(int numberofconnections)
{
    m_split->SetValue(numberofconnections);
}

void mNewDialog::SetStartOption(int startoption)
{
    if(startoption == 0)
        m_manual->SetValue(true);
    else if(startoption == 1)
        m_now->SetValue(true);
    else
        m_schedule->SetValue(true);
}

void mNewDialog::SetCommand(const wxString &command)
{
    m_command->SetValue(command);
}

void mNewDialog::SetOnTop(bool ontop)
{
    m_addontop->SetValue(ontop);
}

void mNewDialog::SetBandwidth(int bandwidth)
{
    m_bandwidth->SetValue(bandwidth);
}

void mNewDialog::SetUrls(const wxArrayString &urls)
{
    m_adresslist->Clear();
    m_url->SetValue("");
    m_adresslist->Append(urls);
    for(unsigned int i=0; i < m_adresslist->GetCount(); i++)
    {
        m_adresslist->Check(i);
    }
}

void mNewDialog::SetUrl(const wxString &url)
{
    m_url->SetValue(url);
}

void mNewDialog::SetUser(const wxString &user)
{
    m_user->SetValue(user);
}

void mNewDialog::SetPassword(const wxString &password)
{
    m_password->SetValue(password);
}

void mNewDialog::SetComment(const wxString &comment)
{
    m_comment->SetValue(comment);
}

void mNewDialog::SetReferenceURL(const wxString &url)
{
    m_referenceurl->SetValue(url);
}

wxString mNewDialog::GetDestination() const
{
    return m_destination->GetValue();
}

int mNewDialog::GetNumberOfConnections() const
{
    return m_split->GetValue();
}

int mNewDialog::GetStartOption() const
{
    if(m_manual->GetValue())
        return 0;
    if(m_now->GetValue())
        return 1;
    return 2;
}

wxString mNewDialog::GetCommand() const
{
    return m_command->GetValue();
}

bool mNewDialog::GetOnTop() const
{
    return m_addontop->GetValue();
}

int mNewDialog::GetBandwidth() const
{
    return m_bandwidth->GetValue();
}

wxArrayString mNewDialog::GetUrls()
{
    wxArrayString urls;
    for(unsigned int i=0; i < m_adresslist->GetCount(); i++)
    {
        if(m_adresslist->IsChecked(i))
            urls.Add(m_adresslist->GetString(i));
    }
    if(!m_url->GetValue().IsEmpty() && urls.Index(m_url->GetValue()) == wxNOT_FOUND && CheckURI(m_url->GetValue()) > 0)
    {
        urls.Add(m_url->GetValue());
    }
    return urls;
}

wxString mNewDialog::GetUser() const
{
    return m_user->GetValue();
}

wxString mNewDialog::GetPassword() const
{
    return m_password->GetValue();
}

wxString mNewDialog::GetComment() const
{
    return m_comment->GetValue();
}

wxString mNewDialog::GetReferenceURL() const
{
    return m_referenceurl->GetValue();
}

void mNewDialog::ReLayout()
{
    m_mainSizer->Layout();
    m_mainSizer->Fit(this);
}

void mNewDialog::EnableDestination(bool enable)
{
    m_destination->Enable(enable);
}

void mNewDialog::EnableDirectory(bool enable)
{
    m_directory->Enable(enable);
}

void mNewDialog::EnableAdd(bool enable)
{
    m_add->Enable(enable);
}

void mNewDialog::EnableEdit(bool enable)
{
    m_edit->Enable(enable);
}

void mNewDialog::EnableAdresslist(bool enable)
{
    m_adresslist->Enable(enable);
}

void mNewDialog::EnableOntop(bool enable)
{
    m_addontop->Enable(enable);
}

void mNewDialog::EnableUser(bool enable)
{
    m_user->Enable(enable);
}

void mNewDialog::EnablePassword(bool enable)
{
    m_password->Enable(enable);
}

void mNewDialog::EnableManual(bool enable)
{
    m_manual->Enable(enable);
}

void mNewDialog::EnableNow(bool enable)
{
    m_now->Enable(enable);
}

void mNewDialog::EnableSchedule(bool enable)
{
    m_schedule->Enable(enable);
}

void mNewDialog::EnableSplit(bool enable)
{
    m_split->Enable(enable);
}

void mNewDialog::EnableComment(bool enable)
{
    m_comment->Enable(enable);
}

void mNewDialog::EnableBandwidth(bool enable)
{
    m_bandwidth->Enable(enable);
}

void mNewDialog::EnableReferenceurl(bool enable)
{
    m_referenceurl->Enable(enable);
}

void mNewDialog::EnableCommand(bool enable)
{
    m_command->Enable(enable);
}

void mNewDialog::EnableUrl(bool enable)
{
    m_url->Enable(enable);
}

void mNewDialog::OnOk(wxCommandEvent &/*event*/)
{
    if(!GetUrls().GetCount())
    {
        wxMessageBox(_("The URLs are empty"), _("Warning..."), wxOK|wxICON_WARNING, this);
        m_url->SetFocus();
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

void mNewDialog::OnCancel(wxCommandEvent &/*event*/)
{
    EndModal(wxID_CANCEL);
}

void mNewDialog::OnDir(wxCommandEvent &/*event*/)
{
    wxString dir = wxDirSelector(_("Select the directory:"), m_destination->GetValue());
    if(!dir.IsEmpty())
    {
        m_destination->SetValue(dir);
        m_destination->SetToolTip(dir);
    }
}

void mNewDialog::OnAdd(wxCommandEvent &/*event*/)
{
    int result = CheckURI(m_url->GetValue());
    if(result == 1)
    {
        m_url->SetValue(wxEmptyString);
        m_url->SetFocus();
    }
    else if(result == 2)
    {
        m_adresslist->Insert(m_url->GetValue(),0);
        m_adresslist->Check(0);
        m_url->SetValue(wxEmptyString);
        m_url->SetFocus();
    }
    else
        wxMessageBox(_("The URL is invalid!"), _("Error..."), wxOK|wxICON_ERROR, this);
}

void mNewDialog::OnEdit(wxCommandEvent &/*event*/)
{
    for(unsigned int i=0; i < m_adresslist->GetCount(); i++)
    {
        if(m_adresslist->IsSelected(i))
        {
            while(true)
            {
                wxTextEntryDialog dialog(this, _("Edit the URL"), _("Edit the URL"), m_adresslist->GetString(i));
                if(dialog.ShowModal() == wxID_OK)
                {
                    int result = CheckURI(dialog.GetValue());
                    if(result == 1)
                    {
                        if(m_adresslist->GetString(i).Lower() != dialog.GetValue().Lower())
                            m_adresslist->Delete(i);
                        break;
                    }
                    else if(result == 2)
                    {
                        m_adresslist->Insert(dialog.GetValue(),0);
                        m_adresslist->Check(0);
                        break;
                    }
                    else
                        wxMessageBox(_("The URL is invalid!"), _("Error..."), wxOK|wxICON_ERROR, this);
                }
                else
                    break;
            }
            break;
        }
    }
}

int mNewDialog::CheckURI(wxString uritxt)
{
    if((uritxt.Mid(0,4).Lower()) == wxT("www."))
        uritxt = wxT("http://") + uritxt;
    else if((uritxt.Mid(0,4).Lower()) == wxT("ftp."))
        uritxt = wxT("ftp://") + uritxt;
    else if((uritxt.Mid(0,1).Lower()) == wxT("/"))
        uritxt = wxT("file://") + uritxt;
    else if((uritxt.Mid(1,1).Lower()) == wxT(":"))
        uritxt = wxT("file://") + uritxt;
    wxURI uri(uritxt);
    bool iscomplete = false;
    bool localfile = uritxt.StartsWith("file");
    if(localfile)
        iscomplete = uri.HasScheme() && uri.HasPath();
    else
        iscomplete = uri.HasServer() && uri.HasScheme() && uri.HasPath();
    if(iscomplete)
    {
        for(unsigned int i = 0; i < m_adresslist->GetCount(); i++)
        {
            if(m_adresslist->GetString(i) == uritxt)
                //alreadyexist
                return 1;
        }
        return 2;
    }
    else
        return -1;
}

/***************************************************************
 * Name:      dxtoolbar.cpp
 * Author:    David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2021
 * License:   GPL3
 **************************************************************/

#include "dxtoolbar.h"

dxToolBar::dxToolBar(wxWindow *parent)
    : wxToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxTB_FLAT|wxTB_NODIVIDER|wxTB_TEXT)
{
#if defined (__WXMAC__)
    Connect(wxEVT_PAINT, wxPaintEventHandler(dxToolBar::OnToolbarPaint), nullptr, this);
#endif
}

#if defined (__WXMAC__)
void dxToolBar::OnToolbarPaint(wxPaintEvent &event)
{
    wxWindow* pWnd = (wxWindow*)event.GetEventObject();
    wxPaintDC dc(pWnd);
    wxBrush brush(pWnd->GetParent()->GetBackgroundColour());
    dc.SetBackground(brush);
    dc.Clear();
}
#endif

/***************************************************************
 * Name:      dxlistctrl.cpp
 * Author:    David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2014
 * License:   GPL3
 **************************************************************/

#include "dxlistctrl.h"

BEGIN_EVENT_TABLE(dxListCtrl, wxListCtrl)
    EVT_SIZE(dxListCtrl::OnResize)
END_EVENT_TABLE()

dxListCtrl::dxListCtrl(wxWindow *parent, const wxWindowID id, bool virt, bool resize)
    : wxListCtrl(parent, id, wxDefaultPosition, wxDefaultSize,
                 virt ? wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_VIRTUAL | wxLC_SINGLE_SEL : wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL),
      m_resizeColumn(0), m_resizeColumnMinWidth(80), m_resize(resize)
{
    Connect(wxEVT_LIST_COL_END_DRAG, wxSizeEventHandler(dxListCtrl::OnResize), nullptr, this);
}

int dxListCtrl::resizeColumn() const
{
    return m_resizeColumn;
}

void dxListCtrl::setResizeColumn(int resizeColumn)
{
    m_resizeColumn = resizeColumn;
}

int dxListCtrl::resizeColumnMinWidth() const
{
    return m_resizeColumnMinWidth;
}

void dxListCtrl::setResizeColumnMinWidth(int resizeColumnMinWidth)
{
    m_resizeColumnMinWidth = resizeColumnMinWidth;
}

void dxListCtrl::SetColumnHeaderFiltered(int column, bool filtered)
{
    wxListItem item;
    item.SetMask(wxLIST_MASK_TEXT);
    GetColumn(column, item);
    if(filtered && item.GetText().Right(1) != "*")
    {
        item.SetText(item.GetText()+"*");
    }
    if(!filtered && item.GetText().Right(1) == "*")
    {
        item.SetText(item.GetText().Left(item.GetText().Len()-1));
    }
    SetColumn(column, item);
}

void dxListCtrl::ClearSelection()
{
    long item = -1;
    for(;;)
    {
        item = GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if(item == -1)
            break;
        SetItemState(item, 0, wxLIST_STATE_SELECTED);
    }
}

void dxListCtrl::OnResize(wxSizeEvent &event)
{
    if(m_resize)
    {
#ifdef __WXGTK__
        resize();
#else
        CallAfter(&dxListCtrl::resize);
#endif
    }
    event.Skip();
}

void dxListCtrl::resize()
{
    if(GetSize().y < 32) return; //avoid an endless update bug when the height is small
    int listWidth = GetClientSize().x;
#ifndef __WXMSW__
    if(GetItemCount() > GetCountPerPage())
    {
        int scrollWidth = wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
        listWidth = listWidth - scrollWidth;
    }
#endif
    int columnsWidth = 0;
    int columns = GetColumnCount();
    if(!columns) return; //nothing to resize
    for(int i=0; i<columns; i++)
    {
        if(i != m_resizeColumn)
            columnsWidth += GetColumnWidth(i);
    }
    if(columnsWidth+m_resizeColumnMinWidth>listWidth)
    {
        SetColumnWidth(m_resizeColumn, m_resizeColumnMinWidth);
        return;
    }
    SetColumnWidth(m_resizeColumn, listWidth-columnsWidth);
}

wxString dxListCtrl::OnGetItemText(long /*item*/, long /*column*/) const
{
    return "";
}

int dxListCtrl::OnGetItemColumnImage(long /*item*/, long /*column*/) const
{
    return 0;
}

wxListItemAttr *dxListCtrl::OnGetItemAttr(long /*item*/) const
{
    return nullptr;
}


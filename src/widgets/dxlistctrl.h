/***************************************************************
 * Name:      dxlistctrl.h
 * Author:    David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2014
 * License:   GPL3
 **************************************************************/

#ifndef DXLISTCTRL_H
#define DXLISTCTRL_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/listctrl.h>

class dxListCtrl: public wxListCtrl
{
    DECLARE_EVENT_TABLE()
public:
    dxListCtrl(wxWindow *parent, const wxWindowID id, bool virt = true, bool resize = true);

    int resizeColumn() const;
    void setResizeColumn(int resizeColumn);
    int resizeColumnMinWidth() const;
    void setResizeColumnMinWidth(int resizeColumnMinWidth);
    void SetColumnHeaderFiltered(int column, bool filtered);
    void ClearSelection();
private:
    int m_resizeColumn;
    int m_resizeColumnMinWidth;
    bool m_resize;

    virtual wxString OnGetItemText(long item, long column) const;
    virtual int OnGetItemColumnImage(long item, long column) const;
    virtual wxListItemAttr *OnGetItemAttr(long item) const;
    void OnResize(wxSizeEvent& event);
    void resize();
};

#endif  /* DXLISTCTRL_H */


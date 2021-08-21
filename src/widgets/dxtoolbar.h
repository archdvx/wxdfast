/***************************************************************
 * Name:      dxtoolbar.h
 * Author:    David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2021
 * License:   GPL3
 **************************************************************/

#ifndef DXTOOLBAR_H
#define DXTOOLBAR_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class dxToolBar: public wxToolBar
{
public:
    dxToolBar(wxWindow *parent);

private:
#if defined (__WXMAC__)
    void OnToolbarPaint(wxPaintEvent& event);
#endif
};

#endif  /* DXTOOLBAR_H */


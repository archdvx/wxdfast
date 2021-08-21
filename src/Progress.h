/***************************************************************
 * Name:      Progress.h
 * Author:    David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2021
 * License:   GPL2
 **************************************************************/

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#ifndef MPROGRESS_H
#define MPROGRESS_H

class mProgress : public wxPanel
{
    DECLARE_DYNAMIC_CLASS(mGraph)
    DECLARE_EVENT_TABLE()
public:
    mProgress() : wxPanel(nullptr) {}
    explicit mProgress(wxWindow *parent);

    void OnPaint(wxPaintEvent &event);
    std::string bitfield() const;
    void setBitfield(const std::string &bitfield);
    int numpieces() const;
    void setNumpieces(int numpieces);
    bool finished() const;
    void setFinished(bool finished);
private:
    std::string m_bitfield;
    int m_numpieces;
    bool m_finished;
};

#endif // MPROGRESS_H

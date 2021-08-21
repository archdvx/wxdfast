/***************************************************************
 * Name:      Graph.h
 * Author:    Max Magalhães Velasques (maxvelasques@gmail.com)
 *            David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2006
 * License:   GPL2
 **************************************************************/

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <vector>

#ifndef MGRAPH_H
#define MGRAPH_H

class mGraph : public wxPanel
{
    DECLARE_DYNAMIC_CLASS(mGraph)
    DECLARE_EVENT_TABLE()
public:
    mGraph() : wxPanel(nullptr) {}
    explicit mGraph(wxWindow *parent);

    void OnPaint(wxPaintEvent &event);
    void AddDownloadPoint(int point);
    void AddUploadPoint(int point);
private:
    std::vector<int> m_downloadpoints;
    std::vector<int> m_uploadpoints;
};

#endif // MGRAPH_H

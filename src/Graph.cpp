/***************************************************************
 * Name:      Graph.cpp
 * Author:    Max Magalhães Velasques (maxvelasques@gmail.com)
 *            David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2006
 * License:   GPL2
 **************************************************************/

#include <wx/dcbuffer.h>
#include <vector>
#include "Graph.h"
#include "Options.h"
#include "UtilFunctions.h"

IMPLEMENT_DYNAMIC_CLASS(mGraph, wxPanel)

BEGIN_EVENT_TABLE(mGraph, wxPanel)
    EVT_PAINT(mGraph::OnPaint)
END_EVENT_TABLE()

#define DX 3.0

mGraph::mGraph(wxWindow *parent)
    : wxPanel(parent)
{
}

void mGraph::OnPaint(wxPaintEvent &/*event*/)
{
    wxBufferedPaintDC dc(this);
    int x, y, lastx, lasty, count;
    float dx, dy;

    //DEFINE PARAMETERS
    int scale = moptions.graphscale()*1024.; //graphscale at KB/S, values at B/S
    int textarea = moptions.graphtextarea();
    wxFont bigfont;
    bigfont.SetPointSize(moptions.graphspeedfontsize());

    //SET THE BACKGROUND COLOR
    wxBrush b(moptions.graphbackcolor(), wxBRUSHSTYLE_SOLID);
    dc.SetBackground(b);
    dc.Clear();

    //GET THE LIMITS OF THE WINDOW
    this->GetSize(&x,&y);
    dx = DX;
    dy = ((float) y)/((float) scale);

    //DRAW A GRID FOR THE GRAPH
    wxPen gridpen(moptions.graphgridcolor());
    dc.SetPen(gridpen);
    int grid = y/5;
    count = 1;
    while (y >= (grid*count))
    {
        dc.DrawLine(textarea,y-grid*count ,x-((int)dx),y-grid*count);
        count++;
    }
    dc.DrawLine(textarea,0 ,textarea,y);

    //DRAW THE DOWNLOAD GRAPH
    wxPen linepen(moptions.graphlinecolor());
    linepen.SetWidth(moptions.graphlinewidth());
    dc.SetPen(linepen);
    if(m_downloadpoints.size())
    {
        int current = *m_downloadpoints.begin();
        count = 0;

        //CALCULATE THE STARTPOINT
        int xstart = x - textarea-5;
        int nitens = m_downloadpoints.size();
        int startitem = 0;
        if(xstart > (int)(nitens*dx))
        {
            xstart = x-(int)(nitens*dx);
        }
        else
        {
            startitem = (int)(((nitens*dx)-((float)xstart))/dx);
            xstart = textarea+5;
        }

        lastx = xstart+((int)dx*count);
        lasty = y-((int)(dy*current));
        for(const auto &node : m_downloadpoints)
        {
            current = node;
            count++;
            if(startitem <= count)
                dc.DrawLine(lastx,lasty,xstart+((int)dx)*(count-startitem),y-((int)(dy*(current>scale?scale:current))));

            lastx = xstart+((int)dx*(count-startitem));
            lasty = y-((int)(dy*current));

            //DON'T DRAW THE GRAPH AFTER THE END OF THE WINDOW
            if (lastx >= x)
                break;
        }
        //WRITE THE CURRENT DOWNLOAD SPEED
        dc.SetTextForeground(moptions.graphfontcolor());
        dc.SetFont(bigfont);
        wxCoord speedh;
        dc.GetTextExtent(MyUtilFunctions::SpeedText(current,true), nullptr, &speedh);
        dc.DrawText(MyUtilFunctions::SpeedText(current,true),5,y/2-speedh/2);
    }
    else
    {
        //WRITE THE DEFAULT DOWNLOAD SPEED
        dc.SetTextForeground(moptions.graphfontcolor());
        dc.SetFont(bigfont);
        wxCoord speedh;
        dc.GetTextExtent(MyUtilFunctions::SpeedText(0,true), nullptr, &speedh);
        dc.DrawText(MyUtilFunctions::SpeedText(0,true),5,y/2-speedh/2);
    }
    /* NOTE 0.7.0 will not work as BT downloader/uploader
    //DRAW THE UPLOAD GRAPH
    linepen.SetColour(moptions.graphuploadcolor());
    dc.SetPen(linepen);
    if(m_uploadpoints.size())
    {
        int current = *m_uploadpoints.begin();
        count = 0;

        //CALCULATE THE STARTPOINT
        int xstart = x - textarea-5;
        int nitens = m_uploadpoints.size();
        int startitem = 0;
        if(xstart > (int)(nitens*dx))
        {
            xstart = x-(int)(nitens*dx);
        }
        else
        {
            startitem = (int)(((nitens*dx)-((float)xstart))/dx);
            xstart = textarea+5;
        }

        lastx = xstart+((int)dx*count);
        lasty = y-((int)(dy*current));
        for(const auto &node : m_uploadpoints)
        {
            current = node;
            count++;
            if(startitem <= count)
                dc.DrawLine(lastx,lasty ,xstart+((int)dx)*(count-startitem),y-((int)(dy*(current>scale?scale:current))));

            lastx = xstart+((int)dx*(count-startitem));
            lasty = y-((int)(dy*current));

            //DON'T DRAW THE GRAPH AFTER THE END OF THE WINDOW
            if(lastx >= x)
                break;
        }
        //WRITE THE CURRENT UPLOAD SPEED
        dc.SetTextForeground(moptions.graphfontcolor());
        dc.SetFont(bigfont);
        dc.DrawText(MyUtilFunctions::SpeedText(current,false),5,y/2+5);
    }
    else
    {
        //WRITE THE DEFAULT UPLOAD SPEED
        dc.SetTextForeground(moptions.graphfontcolor());
        dc.SetFont(bigfont);
        dc.DrawText(MyUtilFunctions::SpeedText(0,false),5,y/2+5);
    }
    */
}

void mGraph::AddDownloadPoint(int point)
{
    if(m_downloadpoints.size() && (int)(m_downloadpoints.size()*DX) > this->GetSize().GetWidth()-moptions.graphtextarea())
    {
        m_downloadpoints.erase(m_downloadpoints.begin());
    }
    m_downloadpoints.push_back(point);
}

void mGraph::AddUploadPoint(int /*point*/)
{
    /* NOTE 0.7.0 will not work as BT downloader/uploader
    if(m_uploadpoints.size() && (int)(m_uploadpoints.size()*DX) > this->GetSize().GetWidth()-moptions.graphtextarea())
    {
        m_uploadpoints.erase(m_uploadpoints.begin());
    }
    m_uploadpoints.push_back(point);
    */
}

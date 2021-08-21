/***************************************************************
 * Name:      Progress.cpp
 * Author:    David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2021
 * License:   GPL2
 **************************************************************/

#include <wx/dcbuffer.h>
#include "Progress.h"
#include "Options.h"

#define PIECE_WIDTH 10
#define PIECE_HEIGHT 14
#define PIECE_SPACE 1

IMPLEMENT_DYNAMIC_CLASS(mProgress, wxPanel)

BEGIN_EVENT_TABLE(mProgress, wxPanel)
    EVT_PAINT(mProgress::OnPaint)
END_EVENT_TABLE()

mProgress::mProgress(wxWindow *parent)
    : wxPanel(parent), m_bitfield(""), m_numpieces(0), m_finished(false)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void mProgress::OnPaint(wxPaintEvent &/*event*/)
{
    wxBufferedPaintDC dc(this);
    int w, h;
    this->GetSize(&w,&h);

    //CLEAR THE PROGRESS AREA
    dc.Clear();
    if(!m_numpieces)
        return;

    int x = PIECE_SPACE, y = PIECE_SPACE;
    if(m_finished)
    {
        dc.SetBrush(wxBrush(moptions.finishedpiece()));
        dc.SetPen(*wxBLACK_PEN);
        for(int i=0; i < m_numpieces; i++)
        {
            dc.DrawRectangle(x,y,PIECE_WIDTH,PIECE_HEIGHT);
            if(x+2*(PIECE_WIDTH+PIECE_SPACE) > w)
            {
                y += PIECE_HEIGHT+2*PIECE_SPACE;
                x = PIECE_SPACE;
            }
            else
            {
                x += PIECE_WIDTH+2*PIECE_SPACE;
            }
        }
        return;
    }
    if(m_bitfield.length()) //something downloaded
    {
        //wxBrush for downloaded piece
        wxBrush d(wxBrush(moptions.finishedpiece()));
        //wxBrush for non-downloaded piece
        wxBrush nd(wxBrush(moptions.unfinishedpiece()));
        dc.SetPen(*wxBLACK_PEN);
        int piecesProcessed = 0;
        for(size_t i = 0; i < m_bitfield.length(); ++i)
        {
            unsigned char byte = static_cast<unsigned char>(m_bitfield[i]);
            for(int n = 7; n >= 0; n--)
            {
                int bit = (byte >> n) & 1;
                if(bit) dc.SetBrush(d);
                else dc.SetBrush(nd);
                dc.DrawRectangle(x,y,PIECE_WIDTH,PIECE_HEIGHT);
                if(x+2*(PIECE_WIDTH+PIECE_SPACE) > w)
                {
                    y += PIECE_HEIGHT+2*PIECE_SPACE;
                    x = PIECE_SPACE;
                }
                else
                {
                    x += PIECE_WIDTH+2*PIECE_SPACE;
                }
                piecesProcessed++;
                if(piecesProcessed == m_numpieces)
                {
                    break;
                }
            }
        }
    }
    else
    {
        dc.SetBrush(wxBrush(moptions.unfinishedpiece()));
        dc.SetPen(*wxBLACK_PEN);
        for(int i=0; i < m_numpieces; i++)
        {
            dc.DrawRectangle(x,y,PIECE_WIDTH,PIECE_HEIGHT);
            if(x+2*(PIECE_WIDTH+PIECE_SPACE) > w)
            {
                y += PIECE_HEIGHT+2*PIECE_SPACE;
                x = PIECE_SPACE;
            }
            else
            {
                x += PIECE_WIDTH+2*PIECE_SPACE;
            }
        }
    }
}

std::string mProgress::bitfield() const
{
    return m_bitfield;
}

void mProgress::setBitfield(const std::string &bitfield)
{
    m_bitfield = bitfield;
}

int mProgress::numpieces() const
{
    return m_numpieces;
}

void mProgress::setNumpieces(int numpieces)
{
    m_numpieces = numpieces;
}

bool mProgress::finished() const
{
    return m_finished;
}

void mProgress::setFinished(bool finished)
{
    m_finished = finished;
}

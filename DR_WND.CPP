// Implementation von Node.

#include "stdafx.h"
#include "train.h"
#include "pageserv.h"
#include "drawdoc.h"
#include "basevw.h"
#include "drawobj.h"
#include "drawtool.h"
#include "mconnect.h"
#include "dr_wnd.h"

#include "cntritem.h"
#include "rectdlg.h"

#include <io.h>
#include <errno.h>

#include <math.h>
#include <direct.h>
#include <limits.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#pragma code_seg(DrawObj_seg);

IMPLEMENT_SERIAL(CDrawWindow, CDrawRect, 0)

CDrawWindow::CDrawWindow()
{
    m_Rand = 4;
    m_Angle = 0;
    m_bObjectChanged = TRUE;
    m_bBrush = TRUE;
}

CDrawWindow::CDrawWindow(const CString& Data, const CString& Typ)
          :CDrawRect(CRect(0, 0, 0, 0))
{
    ASSERT_VALID(this);

    DataElement = Data;
    DataTyp = Typ;
   
    m_bBrush = TRUE;
    m_FontColor = RGB(0, 0, 0);
    m_Rand = 4;
    m_Angle = 0;
    m_bObjectChanged = TRUE;
    ::GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &m_Logfont);
}

CDrawWindow::~CDrawWindow()
{
    ASSERT_VALID(this);
}

void CDrawWindow::OnOpen(CBaseView* )
{
        ASSERT_VALID(this);

        CTextDlg dlg;
        dlg.m_bNoFill = !m_bBrush;
        dlg.m_penSize = m_bPen ? m_logpen.lopnWidth.x : 0;
        dlg.m_Text = DataElement;
        dlg.m_Font = m_Logfont;
        dlg.m_Angle = m_Angle;
        dlg.m_FontColor = m_FontColor;

        if (dlg.DoModal() != IDOK)
                return;

        m_bBrush = !dlg.m_bNoFill;
        DataElement = dlg.m_Text;
        m_Logfont = dlg.m_Font;
        m_Angle = dlg.m_Angle;
        m_Logfont.lfEscapement = m_Angle;
        m_FontColor = dlg.m_FontColor;

        m_bPen = dlg.m_penSize > 0;
        if (m_bPen)
        {
                m_logpen.lopnWidth.x = dlg.m_penSize;
                m_logpen.lopnWidth.y = dlg.m_penSize;
        }
        m_bObjectChanged = TRUE;
        Invalidate();
        m_pDocument->SetModifiedFlag();
}


void CDrawWindow::KillConnections()
{
    ASSERT_VALID(m_pDocument);
    RemoveConnections(m_pDocument);
}

void CDrawWindow::Serialize(CArchive& ar)
{
    ASSERT_VALID(this);
    CDrawRect::Serialize(ar);
    if (ar.IsStoring())
    {
        ar << DataElement;
        ar << DataTyp;
        ar.Write(&m_Angle, sizeof(m_Angle));
        ar.Write(&m_Logfont, sizeof(m_Logfont));
        ar.Write(&m_FontColor, sizeof(m_FontColor));
    }
    else
    {
        ar >> DataElement;
        ar >> DataTyp;
        ar.Read(&m_Angle, sizeof(m_Angle));
        ar.Read(&m_Logfont, sizeof(m_Logfont));
        ar.Read(&m_FontColor, sizeof(m_FontColor));        
    }
}

CRect CDrawWindow::CalcRect(CDC* pDC)
{
    CSize size = pDC->GetTextExtent(DataElement, DataElement.GetLength());

    CRect rect = GetRect();
    CRect oRect = rect;

    double cx = size.cx;
    double cy = size.cy;

    double ang = m_Angle;

    double wink = ((3600.0 - ang)/3600.0) * 3.1415;

    if (m_bBrush)
    {
    char buf[100];

    sprintf(buf, "wink = %1.3f, cos(wink) = %2.3f, sin(wink) = %2.3f\nTest: sin(1.570796327) = %f",
           wink, cos(wink), sin(wink), sin(1.570796327));
    AfxMessageBox(buf);
    }
    double x = cx * cos(wink) + cy * sin(wink);
    double y = cx * sin(wink) + cy * cos(wink);

    rect.InflateRect(((x - rect.Size().cx) / 2) + m_Rand,
                     ((y - rect.Size().cy) / 2) + m_Rand);

    SetRect(rect);

    if (m_bObjectChanged)
    {
        m_bObjectChanged = FALSE;
        Invalidate();
    }

    return rect;
}

void CDrawWindow::DrawInClient(CDC* pDC)
{
    SetActualConnection("CT_ServerObject");
    if (GetConnCount() == 1)
    {
        AfxMessageBox("Draw in Client");
    }
    else
      AfxMessageBox("Bei DrawInClient darf genau ein Server vorhanden sein!");
}

void CDrawWindow::Draw(CDC* pDC)
{
    ASSERT_VALID(this);
    ASSERT_VALID(m_pDocument);

    CBrush brush;
    if (!brush.CreateBrushIndirect(&m_logbrush))
       return;
    CPen pen;
    if (!pen.CreatePenIndirect(&m_logpen))
       return;

    CFont font;
    if (!font.CreateFontIndirect(&m_Logfont))
       return;

    CBrush* pOldBrush;
    CPen* pOldPen;
    CFont* pOldFont;


    pOldFont = pDC->SelectObject(&font);

    if (m_bBrush)
            pOldBrush = pDC->SelectObject(&brush);
    else
            pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);

    if (m_bPen)
            pOldPen = pDC->SelectObject(&pen);
    else
            pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);

    CRect rect = CalcRect(pDC);

    pDC->Rectangle(rect);

    CRect rectout = rect;
    rect.InflateRect(-m_Rand, -m_Rand);

    pDC->SetTextColor(m_FontColor);

    pDC->ExtTextOut(rect.left, rect.bottom,
                    ETO_CLIPPED/*|ETO_OPAQUE*/,
                    rectout,
                    DataElement,
                    DataElement.GetLength(),
                    NULL);

    pDC->SelectObject(pOldBrush);
    pDC->SelectObject(pOldPen);
    pDC->SelectObject(pOldFont);
}

//  point is in logical coordinates
/*int CDrawWindow::HitTest(CPoint point, CBaseView* pView, BOOL bSelected)
{
        ASSERT_VALID(this);
        ASSERT(pView != NULL);

        return 0;
}*/

// position is in logical
void CDrawWindow::MoveTo(const CRect& position, BOOL NurSelbst)
{
        ASSERT_VALID(this);

        CRect OldPos = GetRect();

        if (position == GetRect())
                return;


        SetRect(InflatedRect(OldPos));
        UpDateViews();
        SetRect(InflatedRect(position));
        UpDateViews();
        SetRect(position);

        ASSERT_VALID(m_pDocument);
        m_pDocument->SetModifiedFlag();

        // Der volgende Befehl bewegt alle angehängten Objekte
        // je nach MT - Option an die neue Position

        //if (pView->m_selection.Find(this) != NULL)
        MoveConnections(position, OldPos);
}

CDrawObj* CDrawWindow::Clone(CDrawDoc* pDoc, CObList* pView)
{
        ASSERT_VALID(this);

        CDrawWindow* pClone = new CDrawWindow(DataElement, DataTyp);
        pClone->m_bPen = m_bPen;
        pClone->m_logpen = m_logpen;
        pClone->m_bBrush = m_bBrush;
        pClone->m_logbrush = m_logbrush;
        pClone->m_nShape = m_nShape;
        pClone->m_roundness = m_roundness;
        ASSERT_VALID(pClone);

        if (pDoc != NULL)
                pDoc->Add(pClone, pView);

        ASSERT_VALID(pClone);
        return pClone;
}


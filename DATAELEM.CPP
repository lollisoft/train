// Implementation von Node.

#include "stdafx.h"
#include "train.h"
#include "pageserv.h"
#include "drawdoc.h"
#include "basevw.h"

#include "drawobj.h"
#include "drawtool.h"
#include "mconnect.h"
#include "dataelem.h"

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



IMPLEMENT_SERIAL(CDataElem, CDrawRect, 0)

CDataElem::CDataElem()
{
    m_Rand = 4;
    m_Angle = 0;
    m_FontColor = RGB(0, 0, 0);
    m_bObjectChanged = TRUE;
}

CDataElem::CDataElem(CDrawDoc* pDoc, const CString& Data, const CString& Typ)
          :CDrawRect(CRect(0, 0, 0, 0))
{
    ASSERT_VALID(this);

    DataElement = Data;
    DataTyp = Typ;

    m_FontColor = RGB(0, 0, 0);
    m_Rand = 4;
    m_Angle = 0;
    m_bObjectChanged = TRUE;
    m_Logfont = pDoc->GetStandartFont();
}

CDataElem::~CDataElem()
{
    ASSERT_VALID(this);
}

void CDataElem::OnOpen(CBaseView* )
{
        ASSERT_VALID(this);

        CTextDlg dlg;
        dlg.m_bNoFill = !m_bBrush;
        dlg.m_penSize = m_bPen ? m_logpen.lopnWidth.x : 0;
        dlg.m_Text = DataElement;
        dlg.m_Font = m_Logfont;
        dlg.m_StandartFont = m_pDocument->GetStandartFont();
        dlg.m_Angle = m_Angle;
        dlg.m_FontColor = m_FontColor;

        if (dlg.DoModal() != IDOK)
                return;

        m_bBrush = !dlg.m_bNoFill;
        DataElement = dlg.m_Text;
        m_Logfont = dlg.m_Font;
        m_pDocument->SetStandartFont(dlg.m_StandartFont);
        m_FontColor = dlg.m_FontColor;

        SetAngle(dlg.m_Angle);

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

void CDataElem::SetAngle(int a)
{
    m_Angle = a;
    m_Logfont.lfEscapement = 3600 - m_Angle;
    
}

void CDataElem::KillConnections()
{
    ASSERT_VALID(m_pDocument);
    RemoveConnections(m_pDocument);
}

void CDataElem::Serialize(CArchive& ar)
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

CRect CDataElem::CalcRect(CDC* pDC)
{
    CSize size;

    size = pDC->GetTextExtent(DataElement, DataElement.GetLength());

    CRect rect = GetRect();
    CPoint oPos = GetPos(rect);
    CRect oRect = rect;

    double cx = size.cx;
    double cy = size.cy;
    double ang = m_Angle;

    #define degtorad(rad) (((rad)/1800.0) * 3.141592654)

    double wink = degtorad(ang);


    // Berechne die Ausmaﬂe des Rechtecks

    double s = sin(wink);
    if (s < 0.) s = s * - 1.;
    double c = cos(wink);
    if (c < 0.) c = c * - 1.; 
    double x = cx * c + cy * s;
    double y = cx * s + cy * c;

    rect.InflateRect(((x - rect.Size().cx) / 2 + 1),
                     ((y - rect.Size().cy) / 2 + 1));

    // Nun muﬂ noch der Punkt berechnet werden, ab dem der Text ausgegeben werden soll

    CPoint q1 = GetPos(rect);

    ((CBaseView*) m_ViewList.GetHead())->ClientToDoc(rect);

// Berechnung der Rotation nach Buch ...


    double sizex = rect.Size().cx;
    double sizey = rect.Size().cy;

    double r = sqrt((sizex*sizex) + (sizey*sizey))/2.;

    double dx = cx;
    double dy = cy;

    double alpha = degtorad(180.) - atan(dy/dx);

    CPoint q = GetPos(rect);

    p.x = r * cos(wink + alpha - degtorad(180.));
    p.y = r * sin(wink + alpha - degtorad(180.));

    p.x = - p.x;

    p.x = q.x + p.x;
    p.y = q.y + p.y;    

    ((CBaseView*) m_ViewList.GetHead())->DocToClient(p);
    ((CBaseView*) m_ViewList.GetHead())->DocToClient(q);
    ((CBaseView*) m_ViewList.GetHead())->DocToClient(rect);

    rect.InflateRect(m_Rand, m_Rand);

    p = p + (q1 - q);
 
    rect.OffsetRect(oPos - GetPos(rect));

    SetRect(rect);


    if (m_bObjectChanged)
    {
        m_bObjectChanged = FALSE;
        Invalidate();
    }

    return rect;
}

void CDataElem::DrawInClient(CDC* pDC)
{
    SetActualConnection("CT_ServerObject");
    if (GetConnCount() == 1)
    {
        AfxMessageBox("Draw in Client");
    }
    else
      AfxMessageBox("Bei DrawInClient darf genau ein Server vorhanden sein!");
}

void CDataElem::Draw(CDC* pDC)
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

    CRect rect;

    rect = CalcRect(pDC);

    pDC->Rectangle(rect);

    CRect rectout = rect;
    rect.InflateRect(-m_Rand, -m_Rand);

    pDC->SetTextColor(m_FontColor);

    pDC->ExtTextOut(p.x, p.y,
                    ETO_CLIPPED/*|ETO_OPAQUE*/,
                    rect,
                    DataElement,
                    DataElement.GetLength(),
                    NULL);

    pDC->SelectObject(pOldBrush);
    pDC->SelectObject(pOldPen);
    pDC->SelectObject(pOldFont);
}

// position is in logical
void CDataElem::MoveTo(const CRect& position, BOOL NurSelbst)
{
        ASSERT_VALID(this);

        CRect OldPos = GetRect();

        //OMoves++;

//        if (position == GetRect()) return;

        OMoves++;

//        if (Moved.Find(this) != NULL) return;

        //OMoves++;


        SetRect(InflatedRect(OldPos));
        UpDateViews();
        SetRect(InflatedRect(position));
        UpDateViews();
        SetRect(position);

        ASSERT_VALID(m_pDocument);
        m_pDocument->SetModifiedFlag();

        // Der volgende Befehl bewegt alle angeh‰ngten Objekte
        // je nach MT - Option an die neue Position

        //if (pView->m_selection.Find(this) != NULL)
        MoveConnections(position, OldPos);
}

CDrawObj* CDataElem::Clone(CDrawDoc* pDoc, CObList* pView)
{
        ASSERT_VALID(this);

        CDataElem* pClone = new CDataElem(pDoc, DataElement, DataTyp);
        pClone->m_bPen = m_bPen;
        pClone->m_logpen = m_logpen;
        pClone->m_bBrush = m_bBrush;
        pClone->m_logbrush = m_logbrush;
        pClone->m_Logfont = m_Logfont;
        pClone->m_nShape = m_nShape;
        pClone->m_roundness = m_roundness;
        ASSERT_VALID(pClone);

        if (pDoc != NULL)
                pDoc->Add(pClone, pView);

        ASSERT_VALID(pClone);
        return pClone;
}


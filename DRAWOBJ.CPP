// drawobj.cpp - implementation for drawing objects
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1993 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp and/or WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.


#include "stdafx.h"
#include "train.h"
#include "pageserv.h"
#include "drawdoc.h"
#include "basevw.h"
#include "pagevw.h"
#include "tabvw.h"
#include "mconnect.h"
#include "drawobj.h"

#include "cntritem.h"
#include "rectdlg.h"
#include <stdlib.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#pragma code_seg(DrawObj_seg);

IMPLEMENT_SERIAL(CDrawObj, CMConnect, 0)

CDrawObj::CDrawObj()
{
    Fang = 0;
}

CDrawObj::~CDrawObj()
{
    RemoveConnectionsAndReDraw();
    concount.Sub(this);
}

void CDrawObj::AddViewList(CObList* pList)
{
    POSITION pos = pList->GetHeadPosition();

    while (pos != NULL) m_ViewList.AddHead(pList->GetNext(pos));
}

void CDrawObj::SetRect(const CRect& rect)
{
    m_position = rect;
}

CRect CDrawObj::GetClientRect(CDrawObj* pObj)
{
    return m_position;
}

CRect CDrawObj::InflatedRect(CRect r)
{
    r.InflateRect(m_logpen.lopnWidth.x/2 + 1, m_logpen.lopnWidth.y/2 + 1);
    return r;
}

POSITION CDrawObj::FindView(CBaseView* pView)
{
    POSITION pos = m_ViewList.GetHeadPosition();
    while(pos != NULL)
    {
        POSITION oldpos = pos;
        CNamedObject* pObj = (CNamedObject*) m_ViewList.GetNext(pos);
        if ((pObj->GetObj() == this) &&
           (pObj->GetName() == pView->GetActualPage()))
        {
            return oldpos;
        }
    }
    return NULL;
}

void CDrawObj::SetView(CBaseView* p)
{
    if(m_ViewList.Find(p) == NULL)
      m_ViewList.AddHead(p);
}

void CDrawObj::RemoveView(CBaseView* p)
{
    POSITION pos = m_ViewList.Find(p);
    if (pos != NULL) m_ViewList.RemoveAt(pos);
}

CBaseView* CDrawObj::FindActiveView()
{
    POSITION pos = m_ViewList.GetHeadPosition();

    while (pos != NULL)
    {
        CBaseView* pView = (CBaseView*) m_ViewList.GetNext(pos);
        if (pView->IsActive()) return pView;
    }

    TRACE0("View nicht gefunden!");
    ASSERT(FALSE);
    return NULL;
    
}


CBaseView* CDrawObj::FindHomeView()
{
    POSITION pos = m_ViewList.GetHeadPosition();

    while (pos != NULL)
    {
        CBaseView* pView = (CBaseView*) m_ViewList.GetNext(pos);
        if (pView->GetDocObjects()->Find(this) != NULL) return pView;
    }

    TRACE0("View nicht gefunden!");
    ASSERT(FALSE);
    return NULL;
}

CObList* CDrawObj::GetViewList()
{
    return &m_ViewList;
};


CDrawObj::CDrawObj(const CRect& position)
        : CMConnect()
{
    Fang = 0;
    Netz = KeinNetz;

    concount.Add(this);

    SetRect(position);

    m_bPen = TRUE;
    m_logpen.lopnStyle = PS_INSIDEFRAME;
    m_logpen.lopnWidth.x = 1;
    m_logpen.lopnWidth.y = 1;
    SetFillColor(RGB(0, 0, 0));

    m_bBrush = TRUE;
    m_logbrush.lbStyle = BS_SOLID;
    m_logbrush.lbColor = RGB(192, 192, 192);
    m_logbrush.lbHatch = HS_HORIZONTAL;
}

void CDrawObj::Serialize(CArchive& ar)
{
        CMConnect::Serialize(ar);
        if (ar.IsStoring())
        {
                ar.Write(&Fang, sizeof(int));            
                ar << number;
                ar << GetRect();
                ar << (WORD)m_bPen;
                ar.Write(&m_logpen, sizeof(LOGPEN));
                ar << (WORD)m_bBrush;
                ar.Write(&m_logbrush, sizeof(LOGBRUSH));
        }
        else
        {
                WORD wTemp;
                CRect rect;

                ar.Read(&Fang, sizeof(int));
                ar >> number;
                ar >> rect;
                SetRect(rect);
                ar >> wTemp; m_bPen = (BOOL)wTemp;
                ar.Read(&m_logpen,sizeof(LOGPEN));
                ar >> wTemp; m_bBrush = (BOOL)wTemp;
                ar.Read(&m_logbrush, sizeof(LOGBRUSH));
        }
}

//////////////////////////////////////////////////////////////
// Benutzeraktionen, die auf die Objekte weitergeleitet werden

void CDrawObj::OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point)
{
}

// ?        virtual void OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point);

void CDrawObj::OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point)
{
}

void CDrawObj::OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point)
{
}

void CDrawObj::OnCancel()
{
}





CPoint CDrawObj::GetPos(const CRect& rect)
{
    int xCenter, yCenter;
    
    // this gets the center regardless of left/right and top/bottom ordering
    xCenter = rect.left + rect.Width() / 2;
    yCenter = rect.top + rect.Height() / 2;

    return CPoint(xCenter, yCenter);
}


void CDrawObj::Draw(CDC* )
{
}

void CDrawObj::DrawInClient(CDC*)
{
}

CRect CDrawObj::CalcClientRect()
{
    CRect rect;

    SetActualConnection("CT_ServerObject"); // z.B. Tabelle

    int c = GetConnCount();

    if (c > 1)
    {
        AfxMessageBox("Client besitzt zwei Server!!");
    }

    if (c == 1)
    {
        // Server da...

        CDrawObj* pObj = (CDrawObj*) GetConnection();

        rect = pObj->GetClientRect(this); // Wo soll ich mich zeichnen ?
    }
    
    return rect;
}

BOOL CDrawObj::IsDrawInClient()
{
    if (GetDT_Mode() == DT_DrawInClient)
        return TRUE;
    else
        return FALSE;
}

void CDrawObj::DrawTracker(CDC* pDC, TrackerState state)
{
        ASSERT_VALID(this);

        switch (state)
        {
        case normal:
                break;

        case selected:
        case active:
                {
                        int nHandleCount = GetHandleCount();
                        for (int nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
                        {
                                CPoint handle = GetHandle(nHandle);
                                pDC->PatBlt(handle.x - 3, handle.y - 3, 7, 7, DSTINVERT);
                        }
                }
                break;
        }
}

void CDrawObj::UpDateViews()
{
    POSITION pos = m_ViewList.GetHeadPosition();

    while (pos != NULL)
    {
        CBaseView* pV = (CBaseView*) m_ViewList.GetNext(pos);
        pV->InvalObj(this);
    }
}

// position is in logical
void CDrawObj::MoveTo(const CRect& position, BOOL NurSelbst)
{
        ASSERT_VALID(this);

        CRect OldPos = GetRect();

        if (position == GetRect())
                return;


                UpDateViews();
                SetRect(position);
                UpDateViews();


        MoveConnections(position, OldPos);
        m_pDocument->SetModifiedFlag();
}

// Note: if bSelected, hit-codes start at one for the top-left
// and increment clockwise, 0 means no hit.
// If !bSelected, 0 = no hit, 1 = hit (anywhere)

// point is in logical coordinates
int CDrawObj::HitTest(CPoint point, CBaseView* pView, BOOL bSelected)
{
        ASSERT_VALID(this);
        ASSERT(pView != NULL);

        if (bSelected)
        {
                int nHandleCount = GetHandleCount();
                for (int nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
                {
                        // GetHandleRect returns in logical coords
                        CRect rc = GetHandleRect(nHandle,pView);
                        if (point.x >= rc.left && point.x < rc.right &&
                                point.y <= rc.top && point.y > rc.bottom)
                                return nHandle;
                }
        }
        else
        {
                if (point.x >= GetRect().left && point.x < GetRect().right &&
                        point.y <= GetRect().top && point.y > GetRect().bottom)
                        return 1;
        }
        return 0;
}

// rect must be in logical coordinates
BOOL CDrawObj::Intersects(const CRect& rect)
{
        ASSERT_VALID(this);

        CRect fixed = GetRect();
        fixed.NormalizeRect();
        CRect rectT = rect;
        rectT.NormalizeRect();
        return !(rectT & fixed).IsRectEmpty();
}

int CDrawObj::GetHandleCount()
{
        ASSERT_VALID(this);
        return 8;
}

// returns logical coords of center of handle
CPoint CDrawObj::GetHandle(int nHandle)
{
        ASSERT_VALID(this);
        int x, y, xCenter, yCenter;

        // this gets the center regardless of left/right and top/bottom ordering
        xCenter = GetRect().left + GetRect().Width() / 2;
        yCenter = GetRect().top + GetRect().Height() / 2;

        switch (nHandle)
        {
        default:
                ASSERT(FALSE);

        case 1:
                x = GetRect().left;
                y = GetRect().top;
                break;

        case 2:
                x = xCenter;
                y = GetRect().top;
                break;

        case 3:
                x = GetRect().right;
                y = GetRect().top;
                break;

        case 4:
                x = GetRect().right;
                y = yCenter;
                break;

        case 5:
                x = GetRect().right;
                y = GetRect().bottom;
                break;

        case 6:
                x = xCenter;
                y = GetRect().bottom;
                break;

        case 7:
                x = GetRect().left;
                y = GetRect().bottom;
                break;

        case 8:
                x = GetRect().left;
                y = yCenter;
                break;
        }

        return CPoint(x, y);
}

// return rectange of handle in logical coords
CRect CDrawObj::GetHandleRect(int nHandleID, CBaseView* pView)
{
        ASSERT_VALID(this);
        ASSERT(pView != NULL);

        CRect rect;
        // get the center of the handle in logical coords
        CPoint point = GetHandle(nHandleID);
        // convert to client/device coords
        pView->DocToClient(point);
        // return CRect of handle in device coords
        rect.SetRect(point.x-3, point.y-3, point.x+3, point.y+3);
        pView->ClientToDoc(rect);

        return rect;
}

HCURSOR CDrawObj::GetHandleCursor(int nHandle)
{
        ASSERT_VALID(this);

        LPCSTR id;
        switch (nHandle)
        {
        default:
                ASSERT(FALSE);

        case 1:
        case 5:
                id = IDC_SIZENWSE;
                break;

        case 2:
        case 6:
                id = IDC_SIZENS;
                break;

        case 3:
        case 7:
                id = IDC_SIZENESW;
                break;

        case 4:
        case 8:
                id = IDC_SIZEWE;
                break;
        }

        return AfxGetApp()->LoadStandardCursor(id);
}

// point must be in logical
void CDrawObj::MoveHandleTo(int nHandle, CPoint point)
{
        ASSERT_VALID(this);

        CRect position = GetRect();
        switch (nHandle)
        {
        default:
                ASSERT(FALSE);

        case 1:
                position.left = point.x;
                position.top = point.y;
                break;

        case 2:
                position.top = point.y;
                break;

        case 3:
                position.right = point.x;
                position.top = point.y;
                break;

        case 4:
                position.right = point.x;
                break;

        case 5:
                position.right = point.x;
                position.bottom = point.y;
                break;

        case 6:
                position.bottom = point.y;
                break;

        case 7:
                position.left = point.x;
                position.bottom = point.y;
                break;

        case 8:
                position.left = point.x;
                break;
        }

        MoveTo(position);
}

void CDrawObj::Invalidate()
{
        ASSERT_VALID(this);
        m_pDocument->UpdateAllViews(NULL, HINT_UPDATE_DRAWOBJ, this);
}

CDrawObj* CDrawObj::Clone(CDrawDoc* pDoc)
{
        ASSERT_VALID(this);

        CDrawObj* pClone = new CDrawObj(GetRect());
        pClone->m_bPen = m_bPen;
        pClone->m_logpen = m_logpen;
        pClone->m_bBrush = m_bBrush;
        pClone->m_logbrush = m_logbrush;
        ASSERT_VALID(pClone);

        if (pDoc != NULL)
                pDoc->Add(pClone, &m_ViewList);

        return pClone;
}

void CDrawObj::OnOpen(CBaseView* )
{
        ASSERT_VALID(this);

        CRectDlg dlg;
        dlg.m_bNoFill = !m_bBrush;
        dlg.m_penSize = m_bPen ? m_logpen.lopnWidth.x : 0;

        if (dlg.DoModal() != IDOK)
                return;

        m_bBrush = !dlg.m_bNoFill;
        m_bPen = dlg.m_penSize > 0;
        if (m_bPen)
        {
                m_logpen.lopnWidth.x = dlg.m_penSize;
                m_logpen.lopnWidth.y = dlg.m_penSize;
        }

        Invalidate();
        m_pDocument->SetModifiedFlag();
}

void CDrawObj::SetLineColor(COLORREF color)
{
        ASSERT_VALID(this);

        m_logpen.lopnColor = color;
        Invalidate();
        m_pDocument->SetModifiedFlag();
}

void CDrawObj::SetFillColor(COLORREF color)
{
        ASSERT_VALID(this);

        m_logbrush.lbColor = color;
        Invalidate();
        m_pDocument->SetModifiedFlag();
}

#ifdef _DEBUG
void CDrawObj::AssertValid()
{
        ASSERT(GetRect().left <= GetRect().right);
        ASSERT(GetRect().bottom <= GetRect().top);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CDrawRect

IMPLEMENT_SERIAL(CDrawRect, CDrawObj, 0)

CDrawRect::CDrawRect()
{
}

CDrawRect::CDrawRect(const CRect& position)
        : CDrawObj(position)
{
        ASSERT_VALID(this);

        m_nShape = rectangle;
        m_roundness.x = 16;
        m_roundness.y = 16;
}

void CDrawRect::Serialize(CArchive& ar)
{
        ASSERT_VALID(this);
        
        CDrawObj::Serialize(ar);
        if (ar.IsStoring())
        {
                ar << (WORD) m_nShape;
                ar << m_roundness;
        }
        else
        {
                WORD wTemp;
                ar >> wTemp; m_nShape = (Shape)wTemp;
                ar >> m_roundness;
        }
}

void CDrawRect::KillConnections()
{
    ASSERT_VALID(m_pDocument);
    RemoveConnections(m_pDocument);
}

void CDrawRect::Draw(CDC* pDC)
{
        ASSERT_VALID(this);

        CBrush brush;
        if (!brush.CreateBrushIndirect(&m_logbrush))
                return;
        CPen pen;
        if (!pen.CreatePenIndirect(&m_logpen))
                return;

        CBrush* pOldBrush;
        CPen* pOldPen;

        if (m_bBrush)
                pOldBrush = pDC->SelectObject(&brush);
        else
                pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);

        if (m_bPen)
                pOldPen = pDC->SelectObject(&pen);
        else
                pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);

        CRect rect = GetRect();
        switch (m_nShape)
        {
        case rectangle:
                pDC->Rectangle(rect);
                break;

        case roundRectangle:
                pDC->RoundRect(rect, m_roundness);
                break;

        case ellipse:
                pDC->Ellipse(rect);
                break;

        case line:
                if (rect.top > rect.bottom)
                {
                        rect.top -= m_logpen.lopnWidth.y / 2;
                        rect.bottom += (m_logpen.lopnWidth.y + 1) / 2;
                }
                else
                {
                        rect.top += (m_logpen.lopnWidth.y + 1) / 2;
                        rect.bottom -= m_logpen.lopnWidth.y / 2;
                }

                if (rect.left > rect.right)
                {
                        rect.left -= m_logpen.lopnWidth.x / 2;
                        rect.right += (m_logpen.lopnWidth.x + 1) / 2;
                }
                else
                {
                        rect.left += (m_logpen.lopnWidth.x + 1) / 2;
                        rect.right -= m_logpen.lopnWidth.x / 2;
                }

                pDC->MoveTo(rect.TopLeft());
                pDC->LineTo(rect.BottomRight());
                break;
        }

        pDC->SelectObject(pOldBrush);
        pDC->SelectObject(pOldPen);
}


int CDrawRect::GetHandleCount()
{
        ASSERT_VALID(this);

        return m_nShape == line ? 2 :
                CDrawObj::GetHandleCount() + (m_nShape == roundRectangle);
}

// returns center of handle in logical coordinates
CPoint CDrawRect::GetHandle(int nHandle)
{
        ASSERT_VALID(this);

        if (m_nShape == line && nHandle == 2)
                nHandle = 5;
        else if (m_nShape == roundRectangle && nHandle == 9)
        {
                CRect rect = GetRect();
                rect.NormalizeRect();
                CPoint point = rect.BottomRight();
                point.x -= m_roundness.x / 2;
                point.y -= m_roundness.y / 2;
                return point;
        }

        return CDrawObj::GetHandle(nHandle);
}

HCURSOR CDrawRect::GetHandleCursor(int nHandle)
{
        ASSERT_VALID(this);

        if (m_nShape == line && nHandle == 2)
                nHandle = 5;
        else if (m_nShape == roundRectangle && nHandle == 9)
                return AfxGetApp()->LoadStandardCursor(IDC_SIZE);
        return CDrawObj::GetHandleCursor(nHandle);
}

// point is in logical coordinates
void CDrawRect::MoveHandleTo(int nHandle, CPoint point)
{
        ASSERT_VALID(this);

        if (m_nShape == line && nHandle == 2)
                nHandle = 5;
        else if (m_nShape == roundRectangle && nHandle == 9)
        {
                CRect rect = GetRect();
                rect.NormalizeRect();
                if (point.x > rect.right - 1)
                        point.x = rect.right - 1;
                else if (point.x < rect.left + rect.Width() / 2)
                        point.x = rect.left + rect.Width() / 2;
                if (point.y > rect.bottom - 1)
                        point.y = rect.bottom - 1;
                else if (point.y < rect.top + rect.Height() / 2)
                        point.y = rect.top + rect.Height() / 2;
                m_roundness.x = 2 * (rect.right - point.x);
                m_roundness.y = 2 * (rect.bottom - point.y);
                m_pDocument->SetModifiedFlag();

                UpDateViews();
                return;
        }

        CDrawObj::MoveHandleTo(nHandle, point);
}

// rect must be in logical coordinates
BOOL CDrawRect::Intersects(const CRect& rect)
{
        ASSERT_VALID(this);

        CRect rectT = rect;
        rectT.NormalizeRect();

        CRect fixed = GetRect();
        fixed.NormalizeRect();
        if ((rectT & fixed).IsRectEmpty())
                return FALSE;

        CRgn rgn;
        switch (m_nShape)
        {
        case rectangle:
                return TRUE;

        case roundRectangle:
                rgn.CreateRoundRectRgn(fixed.left, fixed.top, fixed.right, fixed.bottom,
                        m_roundness.x, m_roundness.y);
                break;

        case ellipse:
                rgn.CreateEllipticRgnIndirect(fixed);
                break;

        case line:
                {
                        int x = (m_logpen.lopnWidth.x + 5) / 2;
                        int y = (m_logpen.lopnWidth.y + 5) / 2;
                        POINT points[4];
                        points[0].x = fixed.left;
                        points[0].y = fixed.top;
                        points[1].x = fixed.left;
                        points[1].y = fixed.top;
                        points[2].x = fixed.right;
                        points[2].y = fixed.bottom;
                        points[3].x = fixed.right;
                        points[3].y = fixed.bottom;

                        if (fixed.left < fixed.right)
                        {
                                points[0].x -= x;
                                points[1].x += x;
                                points[2].x += x;
                                points[3].x -= x;
                        }
                        else
                        {
                                points[0].x += x;
                                points[1].x -= x;
                                points[2].x -= x;
                                points[3].x += x;
                        }

                        if (fixed.top < fixed.bottom)
                        {
                                points[0].y -= y;
                                points[1].y += y;
                                points[2].y += y;
                                points[3].y -= y;
                        }
                        else
                        {
                                points[0].y += y;
                                points[1].y -= y;
                                points[2].y -= y;
                                points[3].y += y;
                        }
                        rgn.CreatePolygonRgn(points, 4, ALTERNATE);
                }
                break;
        }
        return rgn.RectInRegion(fixed);
}

CDrawObj* CDrawRect::Clone(CDrawDoc* pDoc, CBaseView* pView)
{
        ASSERT_VALID(this);

        CDrawRect* pClone = new CDrawRect(GetRect());
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

////////////////////////////////////////////////////////////////////////////
// CDrawPoly

IMPLEMENT_SERIAL(CDrawPoly, CDrawObj, 0)

CDrawPoly::CDrawPoly()
{
        m_points = NULL;
        m_nPoints = 0;
        m_nAllocPoints = 0;
}

CDrawPoly::CDrawPoly(const CRect& position)
        : CDrawObj(position)
{
        m_points = NULL;
        m_nPoints = 0;
        m_nAllocPoints = 0;
        m_bPen = TRUE;
        m_bBrush = FALSE;
}

CDrawPoly::~CDrawPoly()
{
        if (m_points != NULL)
                delete m_points;
}

void CDrawPoly::Serialize( CArchive& ar )
{
        int i;
        CDrawObj::Serialize( ar );
        if( ar.IsStoring() )
        {
                ar << (WORD) m_nPoints;
                ar << (WORD) m_nAllocPoints;
                for (i = 0;i< m_nPoints; i++)
                        ar << m_points[i];
        }
        else
        {
                WORD wTemp;
                ar >> wTemp; m_nPoints = wTemp;
                ar >> wTemp; m_nAllocPoints = wTemp;
                m_points = NewPoints(m_nAllocPoints);
                for (i = 0;i < m_nPoints; i++)
                        ar >> m_points[i];
        }
}

void CDrawPoly::Draw(CDC* pDC)
{
        ASSERT_VALID(this);

        CBrush brush;
        if (!brush.CreateBrushIndirect(&m_logbrush))
                return;
        CPen pen;
        if (!pen.CreatePenIndirect(&m_logpen))
                return;

        CBrush* pOldBrush;
        CPen* pOldPen;

        if (m_bBrush)
                pOldBrush = pDC->SelectObject(&brush);
        else
                pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);

        if (m_bPen)
                pOldPen = pDC->SelectObject(&pen);
        else
                pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);

        pDC->Polygon(m_points, m_nPoints);

        pDC->SelectObject(pOldBrush);
        pDC->SelectObject(pOldPen);
}

// position must be in logical coordinates
void CDrawPoly::MoveTo(const CRect& position, BOOL NurSelbst)
{
        ASSERT_VALID(this);
        if (position == GetRect())
                return;

        UpDateViews();

        for (int i = 0; i < m_nPoints; i += 1)
        {
                m_points[i].x += position.left - GetRect().left;
                m_points[i].y += position.top - GetRect().top;
        }

        SetRect(position);

        UpDateViews();
        
        m_pDocument->SetModifiedFlag();
}

int CDrawPoly::GetHandleCount()
{
        return m_nPoints;
}

CPoint CDrawPoly::GetHandle(int nHandle)
{
        ASSERT_VALID(this);

        ASSERT(nHandle >= 1 && nHandle <= m_nPoints);
        return m_points[nHandle - 1];
}

HCURSOR CDrawPoly::GetHandleCursor(int )
{
        return AfxGetApp()->LoadStandardCursor(IDC_ARROW);
}

// point is in logical coordinates
void CDrawPoly::MoveHandleTo(int nHandle, CPoint point)
{
        ASSERT_VALID(this);
        ASSERT(nHandle >= 1 && nHandle <= m_nPoints);
        if (m_points[nHandle - 1] == point)
                return;

        m_points[nHandle - 1] = point;
        RecalcBounds(&m_ViewList);

        UpDateViews();
        m_pDocument->SetModifiedFlag();
}

// rect must be in logical coordinates
BOOL CDrawPoly::Intersects(const CRect& rect)
{
        ASSERT_VALID(this);
        CRgn rgn;
        rgn.CreatePolygonRgn(m_points, m_nPoints, ALTERNATE);
        return rgn.RectInRegion(rect);
}

CDrawObj* CDrawPoly::Clone(CDrawDoc* pDoc, CBaseView* pView)
{
        ASSERT_VALID(this);

        CDrawPoly* pClone = new CDrawPoly(GetRect());
        pClone->m_bPen = m_bPen;
        pClone->m_logpen = m_logpen;
        pClone->m_bBrush = m_bBrush;
        pClone->m_logbrush = m_logbrush;
        pClone->m_points = NewPoints(m_nAllocPoints);
        memcpy(pClone->m_points, m_points, sizeof(CPoint) * m_nPoints);
        pClone->m_nAllocPoints = m_nAllocPoints;
        pClone->m_nPoints = m_nPoints;
        ASSERT_VALID(pClone);

        if (pDoc != NULL)
                pDoc->Add(pClone, pView);

        ASSERT_VALID(pClone);
        return pClone;
}

// point is in logical coordinates
void CDrawPoly::AddPoint(const CPoint& point)
{
        ASSERT_VALID(this);
        if (m_nPoints == m_nAllocPoints)
        {
                CPoint* newPoints = NewPoints(m_nAllocPoints + 10);
                if (m_points != NULL)
                {
                        memcpy(newPoints, m_points, sizeof(CPoint) * m_nAllocPoints);
                        delete m_points;
                }
                m_points = newPoints;
                m_nAllocPoints += 10;
        }

        if (m_nPoints == 0 || m_points[m_nPoints - 1] != point)
        {
                m_points[m_nPoints++] = point;
                if (!RecalcBounds(&m_ViewList))
                {
                    UpDateViews();
                }
                m_pDocument->SetModifiedFlag();
        }
}

CPoint* CDrawPoly::NewPoints(int nPoints)
{
        return (CPoint*)new BYTE[nPoints * sizeof(CPoint)];
}

BOOL CDrawPoly::RecalcBounds(CObList* pView)
{
        ASSERT_VALID(this);

        if (m_nPoints == 0)
                return FALSE;

        CRect bounds(m_points[0], CSize(0, 0));
        for (int i = 1; i < m_nPoints; ++i)
        {
                if (m_points[i].x < bounds.left)
                        bounds.left = m_points[i].x;
                if (m_points[i].x > bounds.right)
                        bounds.right = m_points[i].x;
                if (m_points[i].y < bounds.top)
                        bounds.top = m_points[i].y;
                if (m_points[i].y > bounds.bottom)
                        bounds.bottom = m_points[i].y;
        }

        if (bounds == GetRect())
                return FALSE;

        if (pView == NULL)
                Invalidate();
        else
                UpDateViews();

        SetRect(bounds);

        if (pView == NULL)
                Invalidate();
        else
                UpDateViews();

        return TRUE;
}

////////////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CDrawOleObj, CDrawObj, 0)

BOOL CDrawOleObj::c_bShowItems = TRUE;

CDrawOleObj::CDrawOleObj() : m_extent(0,0)
{
        m_pClientItem = NULL;
}

CDrawOleObj::CDrawOleObj(const CRect& position)
        : CDrawObj(position), m_extent(0, 0)
{
        m_pClientItem = NULL;
}

void CDrawOleObj::Serialize( CArchive& ar )
{
        ASSERT_VALID(this);

        CDrawObj::Serialize(ar);

        if (ar.IsStoring())
        {
                ar << m_extent;
                ar << m_pClientItem;
        }
        else
        {
                ar >> m_extent;
                ar >> m_pClientItem;
                m_pClientItem->m_pDrawObj = this;
        }
}

CDrawObj* CDrawOleObj::Clone(CDrawDoc* pDoc, CBaseView* pView)
{
        ASSERT_VALID(this);

        AfxGetApp()->BeginWaitCursor();

        CDrawOleObj* pClone = NULL;
        CDrawItem* pItem = NULL;
        TRY
        {
                // perform a "deep copy" -- need to copy CDrawOleObj and the CDrawItem
                //  that it points to.
                CDrawOleObj* pClone = new CDrawOleObj(GetRect());
                CDrawItem* pItem = new CDrawItem(m_pDocument, pClone);
                if (!pItem->CreateCloneFrom(m_pClientItem))
                        AfxThrowMemoryException();

                pClone->m_pClientItem = pItem;
                pClone->m_bPen = m_bPen;
                pClone->m_logpen = m_logpen;
                pClone->m_bBrush = m_bBrush;
                pClone->m_logbrush = m_logbrush;
                ASSERT_VALID(pClone);

                if (pDoc != NULL)
                        pDoc->Add(pClone, pView);
        }
        CATCH_ALL(e)
        {
                pItem->Delete();
                pClone->m_pClientItem = NULL;
                delete pClone;
                AfxGetApp()->EndWaitCursor();

                THROW_LAST();
        }
        END_CATCH_ALL

        AfxGetApp()->EndWaitCursor();
        return pClone;
}

void CDrawOleObj::Draw(CDC* pDC)
{
        ASSERT_VALID(this);

        CDrawItem* pItem = m_pClientItem;
        if (pItem != NULL)
        {
                // draw the OLE item itself
                pItem->Draw(pDC, GetRect());

                // use a CRectTracker to draw the standard effects
                CRectTracker tracker;
                tracker.m_rect = GetRect();
                pDC->LPtoDP(tracker.m_rect);

                if (c_bShowItems)
                {
                        // put correct border depending on item type
                        if (pItem->GetType() == OT_LINK)
                                tracker.m_nStyle |= CRectTracker::dottedLine;
                        else
                                tracker.m_nStyle |= CRectTracker::solidLine;
                }

                // put hatching over the item if it is currently open
                if (pItem->GetItemState() == COleClientItem::openState ||
                        pItem->GetItemState() == COleClientItem::activeUIState)
                {
                        tracker.m_nStyle |= CRectTracker::hatchInside;
                }
                tracker.Draw(pDC);
        }
}

void CDrawOleObj::OnOpen(CBaseView* pView)
{
        AfxGetApp()->BeginWaitCursor();
        m_pClientItem->DoVerb(
                GetKeyState(VK_CONTROL) < 0 ? OLEIVERB_OPEN : OLEIVERB_PRIMARY,
                pView);
        AfxGetApp()->EndWaitCursor();
}

// position is in logical
void CDrawOleObj::MoveTo(const CRect& position, BOOL NurSelbst)
{
        ASSERT_VALID(this);

        if (position == GetRect())
                return;

        // call base class to update position
        CDrawObj::MoveTo(position);

        // update position of in-place editing session on position change
        if (m_pClientItem->IsInPlaceActive())
                m_pClientItem->SetItemRects();
}

/////////////////////////////////////////////////////////////////////////////

// Implementation von Node.

#include "stdafx.h"
#include "train.h"
#include "pageserv.h"
#include "drawdoc.h"
#include "basevw.h"
#include "drawobj.h"
#include "drawtool.h"
#include "mconnect.h"
#include "node.h"

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

IMPLEMENT_SERIAL(CDrawNode, CDrawRect, 0)

CDrawNode::CDrawNode()
{
}

CDrawNode::CDrawNode(const CRect& position)
   :CDrawRect(position)
{
    ASSERT_VALID(this);

    m_oldpos = position;

    Netz = Knoten;
}

CDrawNode::~CDrawNode()
{
    ASSERT_VALID(this);

    RemoveConnectionsAndReDraw(); // Mit Abmeldung...
}

void CDrawNode::KillConnections()
{
    RemoveConnections(m_pDocument);
}

CKante* CDrawNode::FindKanteZuKnoten(CDrawNode* pObj)
{
    SetActualConnection("CT_KnotenKante");

    CDrawObj* pStopperObj = (CDrawObj*) GetConnection();

    if (pStopperObj->Find(pObj)) return (CKante*) pStopperObj;

    CDrawObj* pSrc = (CDrawObj*) GetConnection();
    while (pSrc != pStopperObj)
    {
        if (pSrc->Find(pObj)) return (CKante*) pSrc;
        pSrc = (CDrawObj*) GetConnection();
    }

    return NULL;
}

void CDrawNode::Serialize(CArchive& ar)
{
    ASSERT_VALID(this);

    CDrawRect::Serialize(ar);
    if (ar.IsStoring())
    {
        ar << (WORD) m_nShape;
        ar << (WORD) Netz;
    }
    else
    {
        WORD wTemp;

        ar >> wTemp; m_nShape = (Shape)wTemp;
        ar >> wTemp; Netz = (NetzTyp)wTemp;
    }
}


void CDrawNode::Draw(CDC* pDC)
{
    ASSERT_VALID(this);
    ASSERT_VALID(m_pDocument);
//    ASSERT(m_pDocument->GetNetObjects()->GetCount() != 0);
    //ASSERT(m_pDocument->GetNetObjects()->Find(this) != NULL);

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

    pDC->Rectangle(rect);

    pDC->SelectObject(pOldBrush);
    pDC->SelectObject(pOldPen);
}

//  point is in logical coordinates
int CDrawNode::HitTest(CPoint point, CBaseView* pView, BOOL bSelected)
{
        ASSERT_VALID(this);
        ASSERT(pView != NULL);

        return 0;
}

// position is in logical
void CDrawNode::MoveTo(const CRect& position, BOOL NurSelbst)
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

        m_pDocument->SetModifiedFlag();

        // Der volgende Befehl bewegt alle angehängten Objekte
        // je nach MT - Option an die neue Position

        //if (pView->m_selection.Find(this) != NULL)
        MoveConnections(position, OldPos);
}


// point must be in logical
void CDrawNode::MoveHandleTo(int nHandle, CPoint point)
{
    ASSERT_VALID(this);

    CRect position = GetRect();
    MoveTo(position);
}

void CDrawNode::MeldeKanteAb(CKante* pKante)
{
    MeldeElementeAb("CT_KnotenKante", pKante);
}

HCURSOR CDrawNode::GetHandleCursor(int nHandle)
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

IMPLEMENT_SERIAL(CKante, CDrawRect, 0)

CKante::CKante()
{
    Netz = Kante;
    m_SelectedColor = RGB(255,0,0);
    m_NormalColor = RGB(0, 0, 0);
}

CKante::~CKante()
{
    if (SetActualConnection("CT_KanteKnoten"))
    {
        CDrawNode* node = (CDrawNode*) GetConnection();

        node->MeldeKanteAb(this);
        node = (CDrawNode*) GetConnection();
        node->MeldeKanteAb(this);
    }
}

void CKante::KillConnections()
{
    RemoveConnections(m_pDocument);
}

CKante::CKante(const CRect& position)
       :CDrawRect(position)
{
    ASSERT_VALID(this);

    //m_nShape = line;
    Netz = Kante;

    m_SelectedColor = RGB(255,0,0);
    m_NormalColor = m_logpen.lopnColor;
}

void CKante::Serialize(CArchive& ar)
{
    ASSERT_VALID(this);

    CDrawRect::Serialize(ar);

    if (ar.IsStoring())
    {
        ar << (WORD)Netz;
    }
    else
    {
        WORD wTemp;
        ar >> wTemp; Netz = (NetzTyp)wTemp;
    }
}

// returns center of handle in logical coordinates
CPoint CKante::GetHandle(int nHandle)
{
        ASSERT_VALID(this);

        if (nHandle == 2) nHandle = 5;

        return CDrawObj::GetHandle(nHandle);
}

// position is in logical
void CKante::MoveTo(const CRect& position, BOOL NurSelbst)
{
        ASSERT_VALID(this);

        if (position == GetRect())
                return;

        SetActualConnection("CT_KanteKnoten");

        CDrawNode* pNode_AB = (CDrawNode*) GetConnection();
        CDrawNode* pNode_BA = (CDrawNode*) GetConnection();

        if ((FindActiveView()->m_selection.Find(this) != NULL) &&
            (!NurSelbst))
            MoveConnections(position, GetRect());    
        

        CRect rect;
        CPoint pos1 = pNode_AB->GetPos(pNode_AB->GetRect());
        CPoint pos2 = pNode_BA->GetPos(pNode_BA->GetRect());

        rect.UnionRect(CRect (pos1, CSize(1, 1)),
                       CRect (pos2, CSize(1, 1)));

        if (FindActiveView()->m_selection.Find(this) == NULL)
            MoveConnections(rect, GetRect(), "CT_KanteKnoten");

        CRect r = GetRect();

        SetRect(InflatedRect(r));
        UpDateViews();
        SetRect(InflatedRect(rect));
        UpDateViews();        

        m_pDocument->SetModifiedFlag();
}


int CKante::GetHandleCount()
{
        ASSERT_VALID(this);

        return 2;
}

void CKante::MoveHandleTo(int nHandle, CPoint point)
{
    if (nHandle == 2)
        nHandle = 5;


    CDrawObj::MoveHandleTo(nHandle, point);    
}

void CKante::MeldeKnotenAb(CDrawNode* pNode)
{
    MeldeElementeAb("CT_KanteKnoten", this);
}

void CKante::ShowNormal()
{
    m_logpen.lopnColor = m_NormalColor;
    UpDateViews();
}

void CKante::ShowSelected()
{
    m_logpen.lopnColor = m_SelectedColor;
    UpDateViews();
}

void CKante::DrawTracker(CDC* pDC, TrackerState state)
{
        ASSERT_VALID(this);

        switch (state)
        {
        case normal:
                break;

        case selected:
        case active:
                {
                    CPoint handle = GetPos(GetRect());
                    pDC->PatBlt(handle.x - 3, handle.y - 3, 7, 7, DSTINVERT);
                }
                break;
        }
    
}

void CKante::Draw(CDC* pDC)
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

        ASSERT(SetActualConnection("CT_KanteKnoten"));
        
        CDrawNode* pNode_AB = (CDrawNode*) GetConnection();
        CDrawNode* pNode_BA = (CDrawNode*) GetConnection();

        //ASSERT(pNode_AB != pNode_BA);

        CPoint p1 = pNode_AB->GetPos(pNode_AB->GetRect());
        CPoint p2 = pNode_BA->GetPos(pNode_BA->GetRect());

        pDC->MoveTo(p1);
        pDC->LineTo(p2);

        pDC->SelectObject(pOldBrush);
        pDC->SelectObject(pOldPen);
}


    

// Implementation von Node.

#include "stdafx.h"
#include "drawcli.h"

#include "drawdoc.h"
#include "drawvw.h"
#include "drawobj.h"

#include "node.h"

#include "cntritem.h"
#include "rectdlg.h"

#include <io.h>
#include <errno.h>

#include <math.h>
#include <direct.h>
#include <limits.h>


IMPLEMENT_SERIAL(CDrawText, CDrawRect, 0)

CDrawText::CDrawText()
{
}

CDrawText::CDrawText(const CString& s)
   :CDrawRect(CRect position)
{
    ASSERT_VALID(this);

    // Calculate Position

    //m_oldpos = position;

    Netz = Knoten;
}

CDrawText::~CDrawText()
{
    POSITION pos;
    while (!m_connection.IsEmpty())
    {
        CConnect* pConn = (CConnect*)m_connection.GetHead();
        m_pDocument->Remove((CKante*)pConn->GetConnection());

        pos = m_pDocument->GetFirstViewPosition();
        while (pos != NULL)
                ((CDrawView*)m_pDocument->GetNextView(pos))->
                  Remove((CKante*)pConn->GetConnection());
                
        pos = m_pDocument->GetFirstViewPosition();
        while (pos != NULL)
                ((CDrawView*)m_pDocument->GetNextView(pos))->
                  InvalObj((CKante*)pConn->GetConnection());
        
        delete (CKante*)pConn->GetConnection();
    }
}

void CDrawText::KillConnections()
{
    m_connection.RemoveAll();
}

void CDrawText::Serialize(CArchive& ar)
{
    ASSERT_VALID(this);

    CDrawRect::Serialize(ar);
    if (ar.IsStoring())
    {
        ar << (WORD) m_nShape;
        ar << (WORD) Netz;

        POSITION pos = m_connection.GetHeadPosition();
        CConnect* pObj;

        while (pos != NULL)
        {
            pObj = (CConnect*)m_connection.GetNext(pos);
            pObj->SetConnection(pObj->GetConnection());
        }
        m_connection.Serialize(ar);
    }
    else
    {
        WORD wTemp;

        ar >> wTemp; m_nShape = (Shape)wTemp;
        ar >> wTemp; Netz = (NetzTyp)wTemp;

        m_connection.Serialize(ar);
    }
}


void CDrawText::Draw(CDC* pDC)
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

    CRect rect = m_position;

    pDC->Rectangle(rect);

    pDC->SelectObject(pOldBrush);
    pDC->SelectObject(pOldPen);
}

//  point is in logical coordinates
int CDrawText::HitTest(CPoint point, CDrawView* pView, BOOL bSelected)
{
        ASSERT_VALID(this);
        ASSERT(pView != NULL);

        return 0;
}

// position is in logical
void CDrawText::MoveTo(const CRect& position, CDrawView* pView)
{
        ASSERT_VALID(this);

        CPoint OldPos = GetPos(m_position);

        if (position == m_position)
                return;

        if (pView == NULL)
        {
                Invalidate();
                m_position = position;
                Invalidate();
        }
        else
        {
                pView->InvalObj(this);
                m_position = position;
                pView->InvalObj(this);

        }

        POSITION pos = m_connection.GetTailPosition();
        CConnect* pConnObj;
        CKante* pObj;
 
        while (pos != NULL)
        {
            pConnObj = (CConnect*)m_connection.GetPrev(pos);

            pObj = (CKante*)pConnObj->GetConnection();

            if (pObj != NULL)
              if (pObj->GetNumber() != pConnObj->GetConnectionNumber())
                pView->MessageBox("Gefundenes Objekt hat nicht die Nummer, wie ""GetNumber"".");

            if ((pObj != NULL) && (pView->m_selection.Find(pObj) == NULL))
            {
                if (pObj->Netz == Knoten)
                    ASSERT_VALID(NULL);
            
                int nDragHandle = pObj->HitTest(OldPos, pView, TRUE); 

                if (nDragHandle != 0)
                    pObj->MoveHandleTo(nDragHandle, GetPos(m_position), pView);
                else
                {
                    CPoint pos1 = GetPos(m_position);
                    CRect rect;
                    CDrawText* pN = (CDrawText*)pObj->node_AB->GetConnection();
                    if (pN != this)
                    {
                        CPoint pos2 = pN->GetPos(pN->m_position);

                        rect.UnionRect(CRect(pos1, CSize(1, 1)), CRect(pos2, CSize(1, 1)));
                        pObj->MoveTo(rect, pView);
                    }
                    else
                    {
                        pN = (CDrawText*)pObj->node_BA->GetConnection();
                        CPoint pos3 = pN->GetPos(pN->m_position);

                        rect.UnionRect(CRect(pos1, CSize(1, 1)), CRect(pos3, CSize(1, 1)));
                        pObj->MoveTo(rect, pView);
                    }
                }   
            }
        }
        m_pDocument->SetModifiedFlag();
}


// point must be in logical
void CDrawText::MoveHandleTo(int nHandle, CPoint point, CDrawView* pView)
{
    ASSERT_VALID(this);

    CRect position = m_position;
    MoveTo(position, pView);
}

void CDrawText::MeldeKanteAb(CKante* pKante)
{
    POSITION pos = m_connection.GetHeadPosition();

    while (pos != NULL)
    {
        POSITION pos1 = pos;
        CConnect* pConn = (CConnect*)m_connection.GetNext(pos);

        if ((CKante*)pConn->GetConnection() == pKante)
        {
            m_connection.RemoveAt(pos1);
            delete pConn;
            return;
        }
    }
}

HCURSOR CDrawText::GetHandleCursor(int nHandle)
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



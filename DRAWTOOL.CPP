// drawtool.cpp - Werkzeuge für die zu zeichnenden Objekte
//
// (c) Lothar Behrens

#include "stdafx.h"
#include <afxdlgs.h>
#include <string.h>
#include "train.h"
#include "pageserv.h"
#include "drawdoc.h"
#include "drawobj.h"
#include "basevw.h"
#include "pagevw.h"
#include "tabvw.h"
#include "persist.h"
#include "mainfrm.h"
#include "mconnect.h"
#include "node.h"
#include "dr_wnd.h"
#include "dataelem.h"
#include "tabobj.h"
#include "fahrplan.h"
#include "kundetab.h"
#include "drawtool.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDrawTool implementation

CPtrList CDrawTool::c_tools;

CSelectTool selectTool; // static
static CRectTool lineTool(line);
static CRectTool rectTool(rect);
static CRectTool roundRectTool(roundRect);
static CRectTool ellipseTool(ellipse);
static CPolyTool polyTool;

static CNodeTool bahnhofTool(bahnhof);
static CNodeTool connectTool(connect);

static CNodeTool textTool(text);
static CNodeTool adddataTool(adddata);
static CTabTool  tabelleTool(tabelle);
static CBenDefTool bendefTool(bendef); // Erzeuge neuen Träger
static CAddConnectionTool addconnectionTool(addconnection);
static CWindowTool windowTool(window);

static CMakeFahrplanTool makefahrplanTool(makefahrplan);
static CMakeNewKundenTabelleTool makekundentabelleTool(makekundentabelle);
static CMakeNewKundeTool makenewkundeTool(makenewkunde);
static CDeleteKundeTool deletekundeTool(deletekunde);

static CWagenTool wagenplatzierenTool(wagenplatzieren);
static CWagenTool addsitzgruppeTool(addsitzgruppe);
static CWagenTool deletesitzgruppeTool(deletesitzgruppe);
static CWagenTool addsitzTool(addsitz);
static CWagenTool deletesitzTool(deletesitz);

static CWagenBenutzenTool suchefreieplaetzeTool(suchefreieplaetze);
static CWagenBenutzenTool erstellereservierkaertchenTool(erstellereservierkaertchen);
static CWagenBenutzenTool besetzeplatzTool(besetzeplatz);
static CWagenBenutzenTool gebeplatzfreiTool(gebeplatzfrei);
static CWagenBenutzenTool besetzegruppeTool(besetzegruppe);
static CWagenBenutzenTool gebegruppefreiTool(gebegruppefrei);


CPoint CDrawTool::c_down;
UINT CDrawTool::c_nDownFlags;
CPoint CDrawTool::c_last;
DrawShape CDrawTool::c_drawShape = select;

CDrawTool::CDrawTool(DrawShape drawShape)
{
        m_drawShape = drawShape;
        c_tools.AddTail(this);
}

CDrawTool* CDrawTool::FindTool(DrawShape drawShape)
{
        POSITION pos = c_tools.GetHeadPosition();
        while (pos != NULL)
        {
                CDrawTool* pTool = (CDrawTool*)c_tools.GetNext(pos);
                if (pTool->m_drawShape == drawShape)
                        return pTool;
        }

        return NULL;
}

void CDrawTool::OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        // deactivate any in-place active item on this view!
        COleClientItem* pActiveItem = pView->GetDocument()->GetInPlaceActiveItem(pView);
        if (pActiveItem != NULL)
        {
                pActiveItem->Close();
                ASSERT(pView->GetDocument()->GetInPlaceActiveItem(pView) == NULL);
        }

        pView->SetCapture();
        c_nDownFlags = nFlags;
        c_down = point;
        c_last = point;
}

void CDrawTool::OnLButtonDblClk(CBaseView* , UINT , const CPoint& )
{
}

void CDrawTool::OnLButtonUp(CBaseView* pView, UINT , const CPoint& point)
{
        ReleaseCapture();

        if (point == c_down)
            if ((c_drawShape != connect) && (c_drawShape != makefahrplan))
                c_drawShape = select;
}

void CDrawTool::OnMouseMove(CBaseView* , UINT , const CPoint& point)
{
        c_last = point;
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void CDrawTool::OnCancel()
{
        c_drawShape = select;
}

////////////////////////////////////////////////////////////////////////////
// CResizeTool


SelectMode selectMode = none;
int nDragHandle;

CPoint lastPoint;

CSelectTool::CSelectTool()
        : CDrawTool(select)
{
}

void CSelectTool::OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        CPoint local = point;
        pView->ClientToDoc(local);

        CDrawObj* pObj;
        selectMode = none;

        // Check for resizing (only allowed on single selections)
        if (pView->m_selection.GetCount() == 1)
        {
                pObj = (CDrawObj*)pView->m_selection.GetHead();

                ASSERT(pObj->GetDoc() == pView->GetDocument());
                
                nDragHandle = pObj->HitTest(local, pView, TRUE);
                if (nDragHandle != 0)
                        selectMode = size;
        }

        // See if the click was on an object, select and start move if so
        if (selectMode == none)
        {
                pObj = pView->GetDocument()->ObjectAt(local, pView);

                if (pObj != NULL)
                {
                        selectMode = move;

                        if (!pView->IsSelected(pObj))
                                pView->Select(pObj, (nFlags & MK_SHIFT) != 0);

                        // Ctrl+Click clones the selection...
                        if ((nFlags & MK_CONTROL) != 0)
                                pView->CloneSelection();
                }
        }

        // Click on background, start a net-selection
        if (selectMode == none)
        {
                if ((nFlags & MK_SHIFT) == 0)
                        pView->Select(NULL);

                selectMode = netSelect;

                CClientDC dc(pView);
                CRect rect(point.x, point.y, point.x, point.y);
                rect.NormalizeRect();
                dc.DrawFocusRect(rect);
        }

        lastPoint = local;
        CDrawTool::OnLButtonDown(pView, nFlags, point);
}

void CSelectTool::OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if ((nFlags & MK_SHIFT) != 0)
        {
                // Shift+DblClk deselects object...
                CPoint local = point;
                pView->ClientToDoc(local);
                CDrawObj* pObj = pView->GetDocument()->ObjectAt(local, pView);
                if (pObj != NULL)
                        pView->Deselect(pObj);
        }
        else
        {
                // "Normal" DblClk opens properties, or OLE server...
                if (pView->m_selection.GetCount() == 1)
                        ((CDrawObj*)pView->m_selection.GetHead())->OnOpen(pView);
        }

        CDrawTool::OnLButtonDblClk(pView, nFlags, point);
}

void CSelectTool::OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if (pView->GetCapture() == pView)
        {
                if (selectMode == netSelect)
                {
                        CClientDC dc(pView);
                        CRect rect(c_down.x, c_down.y, c_last.x, c_last.y);
                        rect.NormalizeRect();
                        dc.DrawFocusRect(rect);

                        pView->SelectWithinRect(rect, TRUE);
                }
                else if (selectMode != none)
                {
                        pView->GetDocument()->UpdateAllViews(pView);
                }
        }

        CDrawTool::OnLButtonUp(pView, nFlags, point);
}

void CSelectTool::OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if (pView->GetCapture() != pView)
        {
                if (c_drawShape == select && pView->m_selection.GetCount() == 1)
                {
                        CDrawObj* pObj = (CDrawObj*)pView->m_selection.GetHead();
                        CPoint local = point;
                        pView->ClientToDoc(local);
                        int nHandle = pObj->HitTest(local, pView, TRUE);
                        if (nHandle != 0)
                        {
                                SetCursor(pObj->GetHandleCursor(nHandle));
                                return; // bypass CDrawTool
                        }
                }
                if (c_drawShape == select)
                        CDrawTool::OnMouseMove(pView, nFlags, point);
                return;
        }

        if (selectMode == netSelect)
        {
                CClientDC dc(pView);
                CRect rect(c_down.x, c_down.y, c_last.x, c_last.y);
                rect.NormalizeRect();
                dc.DrawFocusRect(rect);
                rect.SetRect(c_down.x, c_down.y, point.x, point.y);
                rect.NormalizeRect();
                dc.DrawFocusRect(rect);

                CDrawTool::OnMouseMove(pView, nFlags, point);
                return;
        }

        CPoint local = point;
        pView->ClientToDoc(local);
        CPoint delta = (CPoint)(local - lastPoint);

        POSITION pos = pView->m_selection.GetHeadPosition();
        while (pos != NULL)
        {
                CDrawObj* pObj = (CDrawObj*)pView->m_selection.GetNext(pos);

                CRect position = pObj->GetRect();

                if (selectMode == move)
                {
                    position += delta;
                    pObj->MoveTo(position);
                }
                else if (nDragHandle != 0)
                {
                        pObj->MoveHandleTo(nDragHandle, local);
                }
        }

        lastPoint = local;

        if (selectMode == size && c_drawShape == select)
        {
                c_last = point;
                SetCursor(((CDrawObj*)pView->m_selection.GetHead())->GetHandleCursor(nDragHandle));
                return; // bypass CDrawTool
        }

        c_last = point;

        if (c_drawShape == select)
                CDrawTool::OnMouseMove(pView, nFlags, point);
}


////////////////////////////////////////////////////////////////////////////
// CRectTool (does rectangles, round-rectangles, and ellipses)

CRectTool::CRectTool(DrawShape drawShape)
        : CDrawTool(drawShape)
{
}

void CRectTool::OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        CDrawTool::OnLButtonDown(pView, nFlags, point);

        CPoint local = point;
        pView->ClientToDoc(local);

        CDrawRect* pObj = new CDrawRect(CRect(local, CSize(0, 0)));
        switch (m_drawShape)
        {
        default:
                ASSERT(FALSE); // unsuported shape!


        /*case bahnhof:*/
        case rect:
                pObj->m_nShape = CDrawRect::rectangle;
                break;

        case roundRect:
                pObj->m_nShape = CDrawRect::roundRectangle;
                break;

        case ellipse:
                pObj->m_nShape = CDrawRect::ellipse;
                break;

        case line:
                pObj->m_nShape = CDrawRect::line;
                break;
        }
        pView->GetDocument()->Add(pObj, pView);
        pView->Select(pObj);

        selectMode = size;
        nDragHandle = 1;
        lastPoint = local;
}

void CRectTool::OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        CDrawTool::OnLButtonDblClk(pView, nFlags, point);
}

void CRectTool::OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if (point == c_down)
        {
                // Don't create empty objects...
                CDrawObj *pObj = (CDrawObj*)pView->m_selection.GetTail();
                pView->GetDocument()->Remove(pObj, pView);
                delete pObj;
                selectTool.OnLButtonDown(pView, nFlags, point); // try a select!
        }

        selectTool.OnLButtonUp(pView, nFlags, point);
}

void CRectTool::OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
        selectTool.OnMouseMove(pView, nFlags, point);
}


////////////////////////////////////////////////////////////////////////////
// CNodeTool 

CNodeTool::CNodeTool(DrawShape drawShape)
        : CRectTool(drawShape)
{
}

void CNodeTool::OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        CDrawNode* pObj;

        CPoint local = point;
        pView->ClientToDoc(local);

        if (m_drawShape == connect)
        {
            /* Wird auch sicher nur ein Node - Objekt selektiert ? */
            pObj = NULL;
            CObList* objects = pView->GetDocObjects();// Ersetzt GetDocument()->Get*Objects()
            CRect rect(local, CSize(1, 1));

            POSITION pos = objects->GetTailPosition();
            BOOL found = FALSE;
            
            while
                   (pos != NULL && !found)
            {
                    pObj = (CDrawNode*)objects->GetPrev(pos);
                    found = (pObj->Intersects(rect) &&
                             pObj->Netz == CDrawNode::Knoten);

                    if (!found) pObj = NULL;         
            }
             

            if ((pObj != NULL) && found)
                if (pObj->Netz == CDrawNode::Knoten)
                    selectTool.OnLButtonDown(pView, nFlags | MK_SHIFT, point);
                else
                    pObj = NULL;
        }
        else
            CDrawTool::OnLButtonDown(pView, nFlags, point);

        if (m_drawShape == text)
        {
            CRect rect(local, CSize(0, 0));
            rect.InflateRect(100, 30);

           
            CDataElem* pObj = new CDataElem(pView->GetDocument(), CString("Test - String"), CString(""));
            pObj->SetRect(rect);

            pView->GetDocument()->Add(pObj, pView);

            pView->Select(pObj);
                
            selectMode = move;
            nDragHandle = 0;
            lastPoint = local;
        }

        if (m_drawShape == adddata)
        {
            CRect rect(local, CSize(0, 0));
            rect.InflateRect(100, 30);

            CDataElem* pObj = new CDataElem(pView->GetDocument(), CString("AddData"), CString(""));
            pObj->SetRect(rect);

            pView->GetDocument()->Add(pObj, pView);

            pView->Select(pObj);
                
            selectMode = move;
            nDragHandle = 0;
            lastPoint = local;
        }


        if (m_drawShape == bahnhof)
        {
                CRect rect(local, CSize(0, 0));
                rect.InflateRect(10, 10);
                
                CDrawNode* pObj = new CDrawNode(rect);

                if (pObj->Netz != CDrawObj::Knoten)
                    pView->MessageBox("Knoten hat in Netz nicht Knoten stehen!");

                switch (m_drawShape)
                {
                default:
                        ASSERT(FALSE); // unsuported shape!

                case bahnhof:
                        pObj->m_nShape = CDrawNode::bahnhof;
                        break;
                }

                /* pObj in Obj - Liste einfügen und selektieren: */

                pView->GetDocument()->Add(pObj, pView); // Setzt m_pDocument

                pView->Select(pObj);
                
                selectMode = move;
                nDragHandle = 0;
                lastPoint = local;
        }
}

void CNodeTool::OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        CDrawTool::OnLButtonDblClk(pView, nFlags, point);
}

void CNodeTool::OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if (m_drawShape == connect) /* Connect oder Bahnhof ? */
        {
            /* Holen des Objektes, das erzeugt und selektiert wurde: */        

            if (pView->m_selection.GetCount() == 2)
            {
                CDrawNode* pObj1 = (CDrawNode*)pView->m_selection.GetHead();
                CDrawNode* pObj2 = (CDrawNode*)pView->m_selection.GetTail();

                pView->Deselect((CDrawNode*)pView->m_selection.GetTail());
                pView->Deselect((CDrawNode*)pView->m_selection.GetTail());

                if (pObj1->Netz != pObj2->Netz)
                    return;

                CPoint pos1 = pObj1->GetPos(pObj1->GetRect());
                CPoint pos2 = pObj2->GetPos(pObj2->GetRect());

                CRect destrect;

                destrect.UnionRect(CRect (pos1, CSize(1, 1)),
                                   CRect (pos2, CSize(1, 1)));

                CKante* pKante = new CKante(destrect);

                // Vorzeitig in Document einfügen (wegen m_pDocument)
                pView->GetDocument()->AddHead(pKante, pView);

                // Neue Verwaltung der Verbindungen

                pKante->SetConnectionTyp("CT_KanteKnoten");
                pKante->SetMoveTyp(MT_MoveRel);
                pKante->SetConnection(pObj1);
                pKante->SetConnection(pObj2);

                pObj1->SetConnectionTyp("CT_KnotenKante");
                pObj1->SetMoveTyp(MT_ChangeSize);
                pObj1->SetConnection(pKante);

                pObj2->SetConnectionTyp("CT_KnotenKante");
                pObj2->SetMoveTyp(MT_ChangeSize);
                pObj2->SetConnection(pKante);

                pView->Select(pKante);
                pView->Deselect(pKante);        
            }
            selectTool.OnLButtonUp(pView, nFlags, point);
        }
        else
        {  /* Bahnhof ... */
            selectTool.OnLButtonUp(pView, nFlags, point);       
        }        

        if (m_drawShape == text)
        {
            selectTool.OnLButtonUp(pView, nFlags, point);       
        }

        if (m_drawShape == adddata)
        {
            selectTool.OnLButtonUp(pView, nFlags, point);       
        }
}

void CNodeTool::OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point)
{
    if (m_drawShape == bahnhof)
    {
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
        selectTool.OnMouseMove(pView, nFlags, point);
    }
    if (m_drawShape == text)
    {
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
        selectTool.OnMouseMove(pView, nFlags, point);
    }
    if (m_drawShape == adddata)
    {
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
        selectTool.OnMouseMove(pView, nFlags, point);
    }
}


////////////////////////////////////////////////////////////////////////////
// CTabTool 

CTabTool::CTabTool(DrawShape drawShape)
        : CRectTool(drawShape)
{
}

void CTabTool::OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        CDrawNode* pObj;

        CPoint local = point;
        pView->ClientToDoc(local);

        CDrawTool::OnLButtonDown(pView, nFlags, point);

        if (m_drawShape == tabelle)
        {
                CTabelleObj* pObj = new CTabelleObj(local, 10, 10, pView->GetDocument());

                /* pObj in Obj - Liste einfügen und selektieren: */
        
                pView->GetDocument()->Add(pObj, pView);

                pView->Select(pObj);
                
                selectMode = move;
                nDragHandle = 0;
                lastPoint = local;
        }
}

void CTabTool::OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point)
{
    CDrawTool::OnLButtonDblClk(pView, nFlags, point);
}



void CTabTool::OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point)
{
    selectTool.OnLButtonUp(pView, nFlags, point);       
}

void CTabTool::OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point)
{
    if (m_drawShape == tabelle)
    {
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
        selectTool.OnMouseMove(pView, nFlags, point);
    }
}


////////////////////////////////////////////////////////////////////////////
// CPolyTool

CPolyTool::CPolyTool()
        : CDrawTool(poly)
{
        m_pDrawObj = NULL;
}

void CPolyTool::OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        CDrawTool::OnLButtonDown(pView, nFlags, point);

        CPoint local = point;
        pView->ClientToDoc(local);

        if (m_pDrawObj == NULL)
        {
                pView->SetCapture();

                m_pDrawObj = new CDrawPoly(CRect(local, CSize(0, 0)));
                pView->GetDocument()->Add(m_pDrawObj, pView);
                pView->Select(m_pDrawObj);
                m_pDrawObj->AddPoint(local);
        }
        else if (local == m_pDrawObj->m_points[0])
        {
                // Stop when the first point is repeated...
                ReleaseCapture();
                m_pDrawObj->m_nPoints -= 1;
                if (m_pDrawObj->m_nPoints < 2)
                {
                        delete m_pDrawObj;
                }
                else
                {
                        pView->InvalObj(m_pDrawObj);
                }
                m_pDrawObj = NULL;
                c_drawShape = select;
                return;
        }

        local.x += 1; // adjacent points can't be the same!
        m_pDrawObj->AddPoint(local);

        selectMode = size;
        nDragHandle = m_pDrawObj->GetHandleCount();
        lastPoint = local;
}

void CPolyTool::OnLButtonUp(CBaseView* , UINT , const CPoint& )
{
        // Don't release capture yet!
}

void CPolyTool::OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if (m_pDrawObj != NULL && (nFlags & MK_LBUTTON) != 0)
        {
                CPoint local = point;
                pView->ClientToDoc(local);
                m_pDrawObj->AddPoint(local);
                nDragHandle = m_pDrawObj->GetHandleCount();
                lastPoint = local;
                c_last = point;
                SetCursor(AfxGetApp()->LoadCursor(IDC_PENCIL));
        }
        else
        {
                SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
                selectTool.OnMouseMove(pView, nFlags, point);
        }
}

void CPolyTool::OnLButtonDblClk(CBaseView* pView, UINT , const CPoint& )
{
        ReleaseCapture();

        int nPoints = m_pDrawObj->m_nPoints;
        if (nPoints > 2 &&
                (m_pDrawObj->m_points[nPoints - 1] == m_pDrawObj->m_points[nPoints - 2] ||
                m_pDrawObj->m_points[nPoints - 1].x - 1 == m_pDrawObj->m_points[nPoints - 2].x &&
                m_pDrawObj->m_points[nPoints - 1].y == m_pDrawObj->m_points[nPoints - 2].y))

        {
                // Nuke the last point if it's the same as the next to last...
                m_pDrawObj->m_nPoints -= 1;
                pView->InvalObj(m_pDrawObj);
        }

        m_pDrawObj = NULL;
        c_drawShape = select;
}

void CPolyTool::OnCancel()
{
        CDrawTool::OnCancel();

        m_pDrawObj = NULL;
}


////////////////////////////////////////////////////////////////////////////
// CRectTool (does rectangles, round-rectangles, and ellipses)

CBenDefTool::CBenDefTool(DrawShape drawShape)
        : CDrawTool(drawShape)
{
}

void CBenDefTool::OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        CDrawTool::OnLButtonDown(pView, nFlags, point);

        CPoint local = point;
        pView->ClientToDoc(local);

        CDrawRect* pObj = new CDrawRect(CRect(local, CSize(0, 0)));

        if (pView->GetDocument()->FindObject(pObj->GetNumber()) != NULL)
            AfxMessageBox("Nummer schon vorhanden (DrawTool)");

        pView->GetDocument()->Add(pObj, pView);
        pView->Select(pObj);

        selectMode = size;
        nDragHandle = 1;
        lastPoint = local;
}

void CBenDefTool::OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        CDrawTool::OnLButtonDblClk(pView, nFlags, point);
}

void CBenDefTool::OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if (point == c_down)
        {
                // Don't create empty objects...
                CDrawObj *pObj = (CDrawObj*)pView->m_selection.GetTail();
                pView->GetDocument()->Remove(pObj, pView);
                delete pObj;
                selectTool.OnLButtonDown(pView, nFlags, point); // try a select!
        }

        selectTool.OnLButtonUp(pView, nFlags, point);
}

void CBenDefTool::OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
        selectTool.OnMouseMove(pView, nFlags, point);
}



/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// CAddConnectionTool 

CAddConnectionTool::CAddConnectionTool(DrawShape drawShape)
        : CRectTool(drawShape)
{
}

void CAddConnectionTool::OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        CDrawObj* pObj;

        CPoint local = point;
        pView->ClientToDoc(local);

        if (m_drawShape == addconnection)
        {
            /* Wird auch sicher nur ein Objekt selektiert ? */
            pObj = NULL;
            CObList* objects = pView->GetDocObjects();
            CRect rect(local, CSize(1, 1));

            POSITION pos = objects->GetTailPosition();
            BOOL found = FALSE;
            
            while
                   (pos != NULL && !found)
            {
                    pObj = (CDrawObj*)objects->GetPrev(pos);
                    found = (pObj->Intersects(rect));

                    if (!found) pObj = NULL;         
            }
             

            if ((pObj != NULL) && found)
                    selectTool.OnLButtonDown(pView, nFlags | MK_SHIFT, point);
        }
        else
            CDrawTool::OnLButtonDown(pView, nFlags, point);
}

void CAddConnectionTool::OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        CDrawTool::OnLButtonDblClk(pView, nFlags, point);
}

void CAddConnectionTool::OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if (m_drawShape == addconnection) 
        {
            /* Holen des Objektes, das erzeugt und selektiert wurde: */        

            if (pView->m_selection.GetCount() == 2)
            {

                CDrawObj* pObj1 = (CDrawObj*)pView->m_selection.GetHead();
                CDrawObj* pObj2 = (CDrawObj*)pView->m_selection.GetTail();

                pView->Deselect((CDrawObj*)pView->m_selection.GetTail());
                pView->Deselect((CDrawObj*)pView->m_selection.GetTail());

                // Neue Verwaltung der Verbindungen

                pObj1->SetConnectionTyp("CT_KnotenBahnhofname"); // CT_ServerClient
                pObj1->SetMoveTyp(MT_MoveRel);
                pObj1->SetConnection(pObj2);

                pObj2->SetConnectionTyp("CT_BahnhofnameKnoten"); // CT_ClientServer
                pObj2->SetMoveTyp(MT_OnlySelf);
                pObj2->SetConnection(pObj1);
            }
            selectTool.OnLButtonUp(pView, nFlags, point);
        }
        else
        {  /* Bahnhof ... */
            selectTool.OnLButtonUp(pView, nFlags, point);       
        }        

        if (m_drawShape == text)
        {
            selectTool.OnLButtonUp(pView, nFlags, point);       
        }

        if (m_drawShape == adddata)
        {
            selectTool.OnLButtonUp(pView, nFlags, point);       
        }
}

void CAddConnectionTool::OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point)
{
    if (m_drawShape == bahnhof)
    {
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
        selectTool.OnMouseMove(pView, nFlags, point);
    }
    if (m_drawShape == text)
    {
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
        selectTool.OnMouseMove(pView, nFlags, point);
    }
    if (m_drawShape == adddata)
    {
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
        selectTool.OnMouseMove(pView, nFlags, point);
    }
}



////////////////////////////////////////////////////////////////////////////
// CWindowTool (does windows...)

CWindowTool::CWindowTool(DrawShape drawShape)
        : CDrawTool(drawShape)
{
}

void CWindowTool::OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        CDrawTool::OnLButtonDown(pView, nFlags, point);

        CPoint local = point;
        pView->ClientToDoc(local);

        CDrawWindow* pObj = new CDrawWindow(CString(""), CString(""));
        switch (m_drawShape)
        {
        default:
                ASSERT(FALSE); // unsuported shape!


        case window:
                pObj->m_nWNDTyp = CDrawWindow::button;
                break;
        }
        pView->GetDocument()->Add(pObj, pView);
        pView->Select(pObj);

        selectMode = size;
        nDragHandle = 1;
        lastPoint = local;
}

void CWindowTool::OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        CDrawTool::OnLButtonDblClk(pView, nFlags, point);
}

void CWindowTool::OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if (point == c_down)
        {
                // Don't create empty objects...
                CDrawObj *pObj = (CDrawObj*)pView->m_selection.GetTail();
                pView->GetDocument()->Remove(pObj, pView);
                delete pObj;
                selectTool.OnLButtonDown(pView, nFlags, point); // try a select!
        }
        else
        {
            // Let the object generate the Window
        }

        selectTool.OnLButtonUp(pView, nFlags, point);
}

void CWindowTool::OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
        selectTool.OnMouseMove(pView, nFlags, point);
}

////////////////////////////////////////////////////////////////////////////
// CMakeFahrplanTool (does windows...)


CMakeFahrplanTool::CMakeFahrplanTool(DrawShape drawShape)
        : CDrawTool(drawShape)
{
}


void CMakeFahrplanTool::OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        CPoint local = point;
        pView->ClientToDoc(local);

        CDrawObj* pObj;
        selectMode = none;

        // See if the click was on an object, select and start move if so
        if (selectMode == none)
        {
                pObj = pView->GetDocument()->ObjectAt(local, pView);

                if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CDrawNode))))
                {
                        if (!pView->IsSelected(pObj))
                                pView->Select(pObj, nFlags | MK_SHIFT, TRUE);

                        CDrawDoc* pDoc = pView->GetDocument();

                        CFahrplanObj* pFahrplanObj;
                        CBaseView* pTabView;
                        if (pView->GetHiddenSelectedCount() == 1)
                        {
                            // ..., 2, 2 => Ein Element in der Tabelle !
                            pFahrplanObj = new CFahrplanObj(CPoint(0, 0), 0, 0, pView->GetDocument());

                            CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();

                            pTabView = (CBaseView*) pFrame->FindView(RUNTIME_CLASS(CTabView)); 
                            
                            pDoc->Add(pFahrplanObj, pTabView);
                            pTabView->Select(pFahrplanObj);
                            //pDoc->SelectFahrplan(pFahrplanObj);
                            pTabView->InvalObj(pFahrplanObj);
                        }

                        pFahrplanObj = (CFahrplanObj*) pTabView->m_selection.GetHead();

                        // Bahnhof in Fahrplan aufnehmen
                        // View holen, in der der Fahrplan ist, der
                        // aktiv ist:

                        // Wenn berhaupt einer aktiv ist...

                        if (!pFahrplanObj->AddHaltepunkt(pView->GetHiddenSelectedCount() - 1, pObj)) // pObj kann nur Trägerobjekt
                                                                // für Bahnhof sein !!
                        {
                            pFahrplanObj->ShowHiddenNormal();
                            pView->UnSelectAllHidden();
                            c_drawShape = select;
                        }
                        else
                        {
                            pFahrplanObj->ShowHiddenSelected();
                        }

                }
        }


        lastPoint = local;
        CDrawTool::OnLButtonDown(pView, nFlags, point);
}

void CMakeFahrplanTool::OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if ((nFlags & MK_SHIFT) != 0)
        {
                // Shift+DblClk deselects object...
                CPoint local = point;
                pView->ClientToDoc(local);
                CDrawObj* pObj = pView->GetDocument()->ObjectAt(local, pView);
                if (pObj != NULL)
                        pView->Deselect(pObj);
        }
        else
        {
                // "Normal" DblClk opens properties, or OLE server...
                if (pView->m_selection.GetCount() == 1)
                        ((CDrawObj*)pView->m_selection.GetHead())->OnOpen(pView);

                        // Wenn nicht nur ein Dialog zur verfgung steht.
                        //...->OnOpenPropertieMenu(pView);
        }

        CDrawTool::OnLButtonDblClk(pView, nFlags, point);
}

void CMakeFahrplanTool::OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if (pView->GetCapture() == pView)
        {
/*                if (selectMode == netSelect)
                {
                        CClientDC dc(pView);
                        CRect rect(c_down.x, c_down.y, c_last.x, c_last.y);
                        rect.NormalizeRect();
                        dc.DrawFocusRect(rect);

                        pView->SelectWithinRect(rect, TRUE);
                }
                else  */
                if (selectMode != none)
                {
                        pView->GetDocument()->UpdateAllViews(pView);
                }
        }

        CDrawTool::OnLButtonUp(pView, nFlags, point);
}

void CMakeFahrplanTool::OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if (pView->GetCapture() != pView)
        {
                if (c_drawShape == select && pView->m_selection.GetCount() == 1)
                {
                        CDrawObj* pObj = (CDrawObj*)pView->m_selection.GetHead();
                        CPoint local = point;
                        pView->ClientToDoc(local);
                        int nHandle = pObj->HitTest(local, pView, TRUE);
                        if (nHandle != 0)
                        {
                                SetCursor(pObj->GetHandleCursor(nHandle));
                                return; // bypass CDrawTool
                        }
                }
                if (c_drawShape == select)
                        CDrawTool::OnMouseMove(pView, nFlags, point);
                return;
        }

        if (selectMode == netSelect)
        {
                CClientDC dc(pView);
                CRect rect(c_down.x, c_down.y, c_last.x, c_last.y);
                rect.NormalizeRect();
                dc.DrawFocusRect(rect);
                rect.SetRect(c_down.x, c_down.y, point.x, point.y);
                rect.NormalizeRect();
                dc.DrawFocusRect(rect);

                CDrawTool::OnMouseMove(pView, nFlags, point);
                return;
        }

        CPoint local = point;
        pView->ClientToDoc(local);
        CPoint delta = (CPoint)(local - lastPoint);

        POSITION pos = pView->m_selection.GetHeadPosition();
        while (pos != NULL)
        {
                CDrawObj* pObj = (CDrawObj*)pView->m_selection.GetNext(pos);

                CRect position = pObj->GetRect();

                if (selectMode == move)
                {
                    position += delta;
                    pObj->MoveTo(position);
                }
                else if (nDragHandle != 0)
                {
                        pObj->MoveHandleTo(nDragHandle, local);
                }
        }

        lastPoint = local;

        if (selectMode == size && c_drawShape == select)
        {
                c_last = point;
                SetCursor(((CDrawObj*)pView->m_selection.GetHead())->GetHandleCursor(nDragHandle));
                return; // bypass CDrawTool
        }

        c_last = point;

        if (c_drawShape == select)
                CDrawTool::OnMouseMove(pView, nFlags, point);
}

////////////////////////////////////////////////////////////////////////////
// CMakeNewKundenTabelleTool (does windows...)


CMakeNewKundenTabelleTool::CMakeNewKundenTabelleTool(DrawShape drawShape)
        : CDrawTool(drawShape)
{
}


void CMakeNewKundenTabelleTool::OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        CPoint local = point;
        pView->ClientToDoc(local);

        selectMode = none;

        // See if the click was on an object, select and start move if so
        if (selectMode == none)
        {
            CKundenTabObj* pKunde = new CKundenTabObj(local, 0, 0, pView->GetDocument());
            pView->GetDocument()->Add(pKunde, pView);
            pView->Select(pKunde);

            pKunde->AddSpalte("String", "Name");
            pKunde->AddSpalte("String", "Netz");
            pKunde->AddSpalte("Datum" , "Datum");
            pKunde->AddSpalte("Zeit"  , "Zeit");
            pKunde->AddSpalte("String", "Von");
            pKunde->AddSpalte("String", "Nach");
            pKunde->AddSpalte("String", "Zug");

            pKunde->RecalcAllPositions();
        }

        
                
        selectMode = move;
        nDragHandle = 0;
        lastPoint = local;
        CDrawTool::OnLButtonDown(pView, nFlags, point);
}

void CMakeNewKundenTabelleTool::OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if ((nFlags & MK_SHIFT) != 0)
        {
                // Shift+DblClk deselects object...
                CPoint local = point;
                pView->ClientToDoc(local);
                CDrawObj* pObj = pView->GetDocument()->ObjectAt(local, pView);
                if (pObj != NULL)
                        pView->Deselect(pObj);
        }
        else
        {
                // "Normal" DblClk opens properties, or OLE server...
                if (pView->m_selection.GetCount() == 1)
                        ((CDrawObj*)pView->m_selection.GetHead())->OnOpen(pView);

                        // Wenn nicht nur ein Dialog zur verfgung steht.
                        //...->OnOpenPropertieMenu(pView);
        }

        CDrawTool::OnLButtonDblClk(pView, nFlags, point);
}

void CMakeNewKundenTabelleTool::OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if (pView->GetCapture() == pView)
        {
                if (selectMode != none)
                {
                        pView->GetDocument()->UpdateAllViews(pView);
                }
        }

        CDrawTool::OnLButtonUp(pView, nFlags, point);
}

void CMakeNewKundenTabelleTool::OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if (pView->GetCapture() != pView)
        {
                if (c_drawShape == select && pView->m_selection.GetCount() == 1)
                {
                        CDrawObj* pObj = (CDrawObj*)pView->m_selection.GetHead();
                        CPoint local = point;
                        pView->ClientToDoc(local);
                        int nHandle = pObj->HitTest(local, pView, TRUE);
                        if (nHandle != 0)
                        {
                                SetCursor(pObj->GetHandleCursor(nHandle));
                                return; // bypass CDrawTool
                        }
                }
                if (c_drawShape == select)
                        CDrawTool::OnMouseMove(pView, nFlags, point);
                return;
        }

        if (selectMode == netSelect)
        {
                CClientDC dc(pView);
                CRect rect(c_down.x, c_down.y, c_last.x, c_last.y);
                rect.NormalizeRect();
                dc.DrawFocusRect(rect);
                rect.SetRect(c_down.x, c_down.y, point.x, point.y);
                rect.NormalizeRect();
                dc.DrawFocusRect(rect);

                CDrawTool::OnMouseMove(pView, nFlags, point);
                return;
        }

        CPoint local = point;
        pView->ClientToDoc(local);
        CPoint delta = (CPoint)(local - lastPoint);

        POSITION pos = pView->m_selection.GetHeadPosition();
        while (pos != NULL)
        {
                CDrawObj* pObj = (CDrawObj*)pView->m_selection.GetNext(pos);

                CRect position = pObj->GetRect();

                if (selectMode == move)
                {
                    position += delta;
                    pObj->MoveTo(position);
                }
                else if (nDragHandle != 0)
                {
                        pObj->MoveHandleTo(nDragHandle, local);
                }
        }

        lastPoint = local;

        if (selectMode == size && c_drawShape == select)
        {
                c_last = point;
                SetCursor(((CDrawObj*)pView->m_selection.GetHead())->GetHandleCursor(nDragHandle));
                return; // bypass CDrawTool
        }

        c_last = point;

        if (c_drawShape == select)
                CDrawTool::OnMouseMove(pView, nFlags, point);
}

////////////////////////////////////////////////////////////////////////////
// CMakeNewKundeTool


CMakeNewKundeTool::CMakeNewKundeTool(DrawShape drawShape)
        : CDrawTool(drawShape)
{
}


void CMakeNewKundeTool::OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        CPoint local = point;
        pView->ClientToDoc(local);

        CDrawObj* pObj;
        selectMode = none;

        // See if the click was on an object, select and start move if so
        if (selectMode == none)
        {
                pObj = pView->GetDocument()->ObjectAt(local, pView);

                if (pObj == NULL)
                // Neue Tabelle anlegen
                {
                    makekundentabelleTool.OnLButtonDown(pView, nFlags, point);
                    pObj = pView->GetDocument()->ObjectAt(local, pView);
                }
                else

                //if (pObj != NULL)
                {
                    if (pObj->IsKindOf(RUNTIME_CLASS(CKundenTabObj)))
                    // Tabelle erwischt...
                    {
                        CKundenTabObj* pTabelle = (CKundenTabObj*) pObj;

                        pView->Select(pTabelle);
                        pTabelle->AddKunde("Lothar Behrens");
                        pTabelle->RecalcAllPositions();
                    }
                    else
                    // Client einer Tabelle erwischt ??
                    {
                        // Allgemeinbegriff------------------------------|
                        CKundenTabObj* pTabelle =
                                 (CKundenTabObj*) pObj->GetPropertyObj("CT_Spaltentyp_Kundentabelle", 1);

                        // pTabelle != NULL -> Element einer Tabelle erwischt
                        if (pTabelle != NULL)
                        {
                            pView->Select(pTabelle);
                            pTabelle->AddKunde("Wolfgang Behrens");
                            pTabelle->RecalcAllPositions();
                        }
                        else
                        {
                            AfxMessageBox("Kein Element der Tabelle!");
                        }
                    }
                }
        }


        lastPoint = local;
        CDrawTool::OnLButtonDown(pView, nFlags, point);
}

void CMakeNewKundeTool::OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if ((nFlags & MK_SHIFT) != 0)
        {
                // Shift+DblClk deselects object...
                CPoint local = point;
                pView->ClientToDoc(local);
                CDrawObj* pObj = pView->GetDocument()->ObjectAt(local, pView);
                if (pObj != NULL)
                        pView->Deselect(pObj);
        }
        else
        {
                // "Normal" DblClk opens properties, or OLE server...
                if (pView->m_selection.GetCount() == 1)
                        ((CDrawObj*)pView->m_selection.GetHead())->OnOpen(pView);

                        // Wenn nicht nur ein Dialog zur verfgung steht.
                        //...->OnOpenPropertieMenu(pView);
        }

        CDrawTool::OnLButtonDblClk(pView, nFlags, point);
}

void CMakeNewKundeTool::OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if (pView->GetCapture() == pView)
        {
                if (selectMode != none)
                {
                        pView->GetDocument()->UpdateAllViews(pView);
                }
        }

        CDrawTool::OnLButtonUp(pView, nFlags, point);
}

void CMakeNewKundeTool::OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if (pView->GetCapture() != pView)
        {
                if (c_drawShape == select && pView->m_selection.GetCount() == 1)
                {
                        CDrawObj* pObj = (CDrawObj*)pView->m_selection.GetHead();
                        CPoint local = point;
                        pView->ClientToDoc(local);
                        int nHandle = pObj->HitTest(local, pView, TRUE);
                        if (nHandle != 0)
                        {
                                SetCursor(pObj->GetHandleCursor(nHandle));
                                return; // bypass CDrawTool
                        }
                }
                if (c_drawShape == select)
                        CDrawTool::OnMouseMove(pView, nFlags, point);
                return;
        }

        if (selectMode == netSelect)
        {
                CClientDC dc(pView);
                CRect rect(c_down.x, c_down.y, c_last.x, c_last.y);
                rect.NormalizeRect();
                dc.DrawFocusRect(rect);
                rect.SetRect(c_down.x, c_down.y, point.x, point.y);
                rect.NormalizeRect();
                dc.DrawFocusRect(rect);

                CDrawTool::OnMouseMove(pView, nFlags, point);
                return;
        }

        CPoint local = point;
        pView->ClientToDoc(local);
        CPoint delta = (CPoint)(local - lastPoint);

        POSITION pos = pView->m_selection.GetHeadPosition();
        while (pos != NULL)
        {
                CDrawObj* pObj = (CDrawObj*)pView->m_selection.GetNext(pos);

                CRect position = pObj->GetRect();

                if (selectMode == move)
                {
                    position += delta;
                    pObj->MoveTo(position);
                }
                else if (nDragHandle != 0)
                {
                        pObj->MoveHandleTo(nDragHandle, local);
                }
        }

        lastPoint = local;

        if (selectMode == size && c_drawShape == select)
        {
                c_last = point;
                SetCursor(((CDrawObj*)pView->m_selection.GetHead())->GetHandleCursor(nDragHandle));
                return; // bypass CDrawTool
        }

        c_last = point;

        if (c_drawShape == select)
                CDrawTool::OnMouseMove(pView, nFlags, point);
}
////////////////////////////////////////////////////////////////////////////
// CDeleteKundeTool (does windows...)


CDeleteKundeTool::CDeleteKundeTool(DrawShape drawShape)
        : CDrawTool(drawShape)
{
}


void CDeleteKundeTool::OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        CPoint local = point;
        pView->ClientToDoc(local);

        CDrawObj* pObj;
        selectMode = none;

        // See if the click was on an object, select and start move if so
        if (selectMode == none)
        {
                pObj = pView->GetDocument()->ObjectAt(local, pView);

                if (pObj != NULL) // Objekt erwischt
                {
                    CKundenTabObj* pKunde = (CKundenTabObj*) pObj->GetPropertyObj("CT_Client_Tabelle", 1);
                    if (pKunde != NULL)
                    // Objekt ist Element einer Tabelle
                    {
                        pKunde->DeleteKunde(pObj); // Die Zeile, die pObj enthält
                    }
                    else
                    {
                        if (pObj->IsKindOf(RUNTIME_CLASS(CKundenTabObj)))
                        // Tabelle selbst erwischt
                        {
                            CKundenTabObj* pKunde = (CKundenTabObj*) pObj;
                            pKunde->DeleteKunde(); // Tabelle erfragt den zu löschenden Kunden
                        }
                    }
                }
        }

        lastPoint = local;
        CDrawTool::OnLButtonDown(pView, nFlags, point);
}

void CDeleteKundeTool::OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if ((nFlags & MK_SHIFT) != 0)
        {
                // Shift+DblClk deselects object...
                CPoint local = point;
                pView->ClientToDoc(local);
                CDrawObj* pObj = pView->GetDocument()->ObjectAt(local, pView);
                if (pObj != NULL)
                        pView->Deselect(pObj);
        }
        else
        {
                // "Normal" DblClk opens properties, or OLE server...
                if (pView->m_selection.GetCount() == 1)
                        ((CDrawObj*)pView->m_selection.GetHead())->OnOpen(pView);

                        // Wenn nicht nur ein Dialog zur verfgung steht.
                        //...->OnOpenPropertieMenu(pView);
        }

        CDrawTool::OnLButtonDblClk(pView, nFlags, point);
}

void CDeleteKundeTool::OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if (pView->GetCapture() == pView)
        {
/*                if (selectMode == netSelect)
                {
                        CClientDC dc(pView);
                        CRect rect(c_down.x, c_down.y, c_last.x, c_last.y);
                        rect.NormalizeRect();
                        dc.DrawFocusRect(rect);

                        pView->SelectWithinRect(rect, TRUE);
                }
                else  */
                if (selectMode != none)
                {
                        pView->GetDocument()->UpdateAllViews(pView);
                }
        }

        CDrawTool::OnLButtonUp(pView, nFlags, point);
}

void CDeleteKundeTool::OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if (pView->GetCapture() != pView)
        {
                if (c_drawShape == select && pView->m_selection.GetCount() == 1)
                {
                        CDrawObj* pObj = (CDrawObj*)pView->m_selection.GetHead();
                        CPoint local = point;
                        pView->ClientToDoc(local);
                        int nHandle = pObj->HitTest(local, pView, TRUE);
                        if (nHandle != 0)
                        {
                                SetCursor(pObj->GetHandleCursor(nHandle));
                                return; // bypass CDrawTool
                        }
                }
                if (c_drawShape == select)
                        CDrawTool::OnMouseMove(pView, nFlags, point);
                return;
        }

        if (selectMode == netSelect)
        {
                CClientDC dc(pView);
                CRect rect(c_down.x, c_down.y, c_last.x, c_last.y);
                rect.NormalizeRect();
                dc.DrawFocusRect(rect);
                rect.SetRect(c_down.x, c_down.y, point.x, point.y);
                rect.NormalizeRect();
                dc.DrawFocusRect(rect);

                CDrawTool::OnMouseMove(pView, nFlags, point);
                return;
        }

        CPoint local = point;
        pView->ClientToDoc(local);
        CPoint delta = (CPoint)(local - lastPoint);

        POSITION pos = pView->m_selection.GetHeadPosition();
        while (pos != NULL)
        {
                CDrawObj* pObj = (CDrawObj*)pView->m_selection.GetNext(pos);

                CRect position = pObj->GetRect();

                if (selectMode == move)
                {
                    position += delta;
                    pObj->MoveTo(position);
                }
                else if (nDragHandle != 0)
                {
                        pObj->MoveHandleTo(nDragHandle, local);
                }
        }

        lastPoint = local;

        if (selectMode == size && c_drawShape == select)
        {
                c_last = point;
                SetCursor(((CDrawObj*)pView->m_selection.GetHead())->GetHandleCursor(nDragHandle));
                return; // bypass CDrawTool
        }

        c_last = point;

        if (c_drawShape == select)
                CDrawTool::OnMouseMove(pView, nFlags, point);
}

////////////////////////////////////////////////////////////////////////////
// CWagenTool (does windows...)


CWagenTool::CWagenTool(DrawShape drawShape)
        : CDrawTool(drawShape)
{
}


void CWagenTool::OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        CPoint local = point;
        pView->ClientToDoc(local);

        CDrawObj* pObj;
        selectMode = none;

        // See if the click was on an object, select and start move if so
        if (selectMode == none)
        {
                pObj = pView->GetDocument()->ObjectAt(local, pView);

                if (pObj != NULL) // Objekt erwischt
                {
                }
        }

        lastPoint = local;
        CDrawTool::OnLButtonDown(pView, nFlags, point);
}

void CWagenTool::OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if ((nFlags & MK_SHIFT) != 0)
        {
                // Shift+DblClk deselects object...
                CPoint local = point;
                pView->ClientToDoc(local);
                CDrawObj* pObj = pView->GetDocument()->ObjectAt(local, pView);
                if (pObj != NULL)
                        pView->Deselect(pObj);
        }
        else
        {
                // "Normal" DblClk opens properties, or OLE server...
                if (pView->m_selection.GetCount() == 1)
                        ((CDrawObj*)pView->m_selection.GetHead())->OnOpen(pView);

                        // Wenn nicht nur ein Dialog zur verfgung steht.
                        //...->OnOpenPropertieMenu(pView);
        }

        CDrawTool::OnLButtonDblClk(pView, nFlags, point);
}

void CWagenTool::OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if (pView->GetCapture() == pView)
        {
/*                if (selectMode == netSelect)
                {
                        CClientDC dc(pView);
                        CRect rect(c_down.x, c_down.y, c_last.x, c_last.y);
                        rect.NormalizeRect();
                        dc.DrawFocusRect(rect);

                        pView->SelectWithinRect(rect, TRUE);
                }
                else  */
                if (selectMode != none)
                {
                        pView->GetDocument()->UpdateAllViews(pView);
                }
        }

        CDrawTool::OnLButtonUp(pView, nFlags, point);
}

void CWagenTool::OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if (pView->GetCapture() != pView)
        {
                if (c_drawShape == select && pView->m_selection.GetCount() == 1)
                {
                        CDrawObj* pObj = (CDrawObj*)pView->m_selection.GetHead();
                        CPoint local = point;
                        pView->ClientToDoc(local);
                        int nHandle = pObj->HitTest(local, pView, TRUE);
                        if (nHandle != 0)
                        {
                                SetCursor(pObj->GetHandleCursor(nHandle));
                                return; // bypass CDrawTool
                        }
                }
                if (c_drawShape == select)
                        CDrawTool::OnMouseMove(pView, nFlags, point);
                return;
        }

        if (selectMode == netSelect)
        {
                CClientDC dc(pView);
                CRect rect(c_down.x, c_down.y, c_last.x, c_last.y);
                rect.NormalizeRect();
                dc.DrawFocusRect(rect);
                rect.SetRect(c_down.x, c_down.y, point.x, point.y);
                rect.NormalizeRect();
                dc.DrawFocusRect(rect);

                CDrawTool::OnMouseMove(pView, nFlags, point);
                return;
        }

        CPoint local = point;
        pView->ClientToDoc(local);
        CPoint delta = (CPoint)(local - lastPoint);

        POSITION pos = pView->m_selection.GetHeadPosition();
        while (pos != NULL)
        {
                CDrawObj* pObj = (CDrawObj*)pView->m_selection.GetNext(pos);

                CRect position = pObj->GetRect();

                if (selectMode == move)
                {
                    position += delta;
                    pObj->MoveTo(position);
                }
                else if (nDragHandle != 0)
                {
                        pObj->MoveHandleTo(nDragHandle, local);
                }
        }

        lastPoint = local;

        if (selectMode == size && c_drawShape == select)
        {
                c_last = point;
                SetCursor(((CDrawObj*)pView->m_selection.GetHead())->GetHandleCursor(nDragHandle));
                return; // bypass CDrawTool
        }

        c_last = point;

        if (c_drawShape == select)
                CDrawTool::OnMouseMove(pView, nFlags, point);
}

////////////////////////////////////////////////////////////////////////////
// CWagenBenutzenTool (does windows...)


CWagenBenutzenTool::CWagenBenutzenTool(DrawShape drawShape)
        : CDrawTool(drawShape)
{
}


void CWagenBenutzenTool::OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        CPoint local = point;
        pView->ClientToDoc(local);

        CDrawObj* pObj;
        selectMode = none;

        // See if the click was on an object, select and start move if so
        if (selectMode == none)
        {
                pObj = pView->GetDocument()->ObjectAt(local, pView);

                if (pObj != NULL) // Objekt erwischt
                {
                }
        }

        lastPoint = local;
        CDrawTool::OnLButtonDown(pView, nFlags, point);
}

void CWagenBenutzenTool::OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if ((nFlags & MK_SHIFT) != 0)
        {
                // Shift+DblClk deselects object...
                CPoint local = point;
                pView->ClientToDoc(local);
                CDrawObj* pObj = pView->GetDocument()->ObjectAt(local, pView);
                if (pObj != NULL)
                        pView->Deselect(pObj);
        }
        else
        {
                // "Normal" DblClk opens properties, or OLE server...
                if (pView->m_selection.GetCount() == 1)
                        ((CDrawObj*)pView->m_selection.GetHead())->OnOpen(pView);

                        // Wenn nicht nur ein Dialog zur verfgung steht.
                        //...->OnOpenPropertieMenu(pView);
        }

        CDrawTool::OnLButtonDblClk(pView, nFlags, point);
}

void CWagenBenutzenTool::OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if (pView->GetCapture() == pView)
        {
/*                if (selectMode == netSelect)
                {
                        CClientDC dc(pView);
                        CRect rect(c_down.x, c_down.y, c_last.x, c_last.y);
                        rect.NormalizeRect();
                        dc.DrawFocusRect(rect);

                        pView->SelectWithinRect(rect, TRUE);
                }
                else  */
                if (selectMode != none)
                {
                        pView->GetDocument()->UpdateAllViews(pView);
                }
        }

        CDrawTool::OnLButtonUp(pView, nFlags, point);
}

void CWagenBenutzenTool::OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point)
{
        if (pView->GetCapture() != pView)
        {
                if (c_drawShape == select && pView->m_selection.GetCount() == 1)
                {
                        CDrawObj* pObj = (CDrawObj*)pView->m_selection.GetHead();
                        CPoint local = point;
                        pView->ClientToDoc(local);
                        int nHandle = pObj->HitTest(local, pView, TRUE);
                        if (nHandle != 0)
                        {
                                SetCursor(pObj->GetHandleCursor(nHandle));
                                return; // bypass CDrawTool
                        }
                }
                if (c_drawShape == select)
                        CDrawTool::OnMouseMove(pView, nFlags, point);
                return;
        }

        if (selectMode == netSelect)
        {
                CClientDC dc(pView);
                CRect rect(c_down.x, c_down.y, c_last.x, c_last.y);
                rect.NormalizeRect();
                dc.DrawFocusRect(rect);
                rect.SetRect(c_down.x, c_down.y, point.x, point.y);
                rect.NormalizeRect();
                dc.DrawFocusRect(rect);

                CDrawTool::OnMouseMove(pView, nFlags, point);
                return;
        }

        CPoint local = point;
        pView->ClientToDoc(local);
        CPoint delta = (CPoint)(local - lastPoint);

        POSITION pos = pView->m_selection.GetHeadPosition();
        while (pos != NULL)
        {
                CDrawObj* pObj = (CDrawObj*)pView->m_selection.GetNext(pos);

                CRect position = pObj->GetRect();

                if (selectMode == move)
                {
                    position += delta;
                    pObj->MoveTo(position);
                }
                else if (nDragHandle != 0)
                {
                        pObj->MoveHandleTo(nDragHandle, local);
                }
        }

        lastPoint = local;

        if (selectMode == size && c_drawShape == select)
        {
                c_last = point;
                SetCursor(((CDrawObj*)pView->m_selection.GetHead())->GetHandleCursor(nDragHandle));
                return; // bypass CDrawTool
        }

        c_last = point;

        if (c_drawShape == select)
                CDrawTool::OnMouseMove(pView, nFlags, point);
}


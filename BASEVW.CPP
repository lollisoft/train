// drawvw.cpp : implementation of the CBaseView class
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
//#include <afxpriv.h>

#include "train.h"
#include "pageserv.h"
#include "mconnect.h"
#include "drawdoc.h"
#include "drawobj.h"
#include "dataelem.h"
#include "tabobj.h"

#include "cntritem.h"
#include "persist.h"
#include "basevw.h"

#include "rectdlg.h"

#include "drawtool.h"
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// private clipboard format (list of Draw objects)
CLIPFORMAT CBaseView::m_cfDraw =
        (CLIPFORMAT)::RegisterClipboardFormat("MFC Train");

/////////////////////////////////////////////////////////////////////////////
// CBaseView

IMPLEMENT_DYNCREATE(CBaseView, CScrollView)

BEGIN_MESSAGE_MAP(CBaseView, CScrollView)
        //{{AFX_MSG_MAP(CBaseView)
        ON_COMMAND(ID_OLE_INSERT_NEW, OnInsertObject)
        ON_COMMAND(ID_CANCEL_EDIT, OnCancelEdit)

        // Aktionen durch die Maus:
        ON_WM_LBUTTONDOWN()
        ON_WM_LBUTTONUP()
        ON_WM_MOUSEMOVE()
        ON_WM_LBUTTONDBLCLK()

        ON_COMMAND(ID_DRAW_SELECT, OnDrawSelect)
        ON_UPDATE_COMMAND_UI(ID_DRAW_SELECT, OnUpdateDrawSelect)


        ON_UPDATE_COMMAND_UI(ID_OBJECT_MOVEBACK, OnUpdateSingleSelect)
        ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
        ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
        ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateAnySelect)
        ON_WM_SIZE()

        ON_COMMAND(ID_ZOOM, OnZoom)
        ON_COMMAND(ID_HUNDERTPROZENT, OnHundertProzent)
        ON_UPDATE_COMMAND_UI(ID_HUNDERTPROZENT, OnUpdateHundertProzent)

        ON_COMMAND(ID_VIEW_GRID, OnViewGrid)
        ON_UPDATE_COMMAND_UI(ID_VIEW_GRID, OnUpdateViewGrid)
        ON_WM_ERASEBKGND()

        ON_COMMAND(ID_OBJECT_FILLCOLOR, OnObjectFillColor)
        ON_COMMAND(ID_OBJECT_LINECOLOR, OnObjectLineColor)
        ON_COMMAND(ID_OBJECT_MOVEBACK, OnObjectMoveBack)
        ON_COMMAND(ID_OBJECT_MOVEFORWARD, OnObjectMoveForward)
        ON_COMMAND(ID_OBJECT_MOVETOBACK, OnObjectMoveToBack)
        ON_COMMAND(ID_OBJECT_MOVETOFRONT, OnObjectMoveToFront)

        ON_UPDATE_COMMAND_UI(ID_OBJECT_MOVEFORWARD, OnUpdateSingleSelect)
        ON_UPDATE_COMMAND_UI(ID_OBJECT_MOVETOBACK, OnUpdateSingleSelect)
        ON_UPDATE_COMMAND_UI(ID_OBJECT_MOVETOFRONT, OnUpdateSingleSelect)
 
        ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
        ON_COMMAND(ID_EDIT_CUT, OnEditCut)

        ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
        ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
        ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
        ON_UPDATE_COMMAND_UI(ID_VIEW_SHOWOBJECTS, OnUpdateViewShowObjects)
        ON_UPDATE_COMMAND_UI(ID_EDIT_PROPERTIES, OnUpdateEditProperties)
        ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)

        ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
        ON_WM_SETFOCUS()
        ON_COMMAND(ID_VIEW_SHOWOBJECTS, OnViewShowObjects)
        ON_COMMAND(ID_EDIT_PROPERTIES, OnEditProperties)
        ON_WM_DESTROY()
        //}}AFX_MSG_MAP
        // Standard printing commands
        ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
        ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBaseView construction/destruction

CBaseView::CBaseView()
{
        m_Zoom = 100;
        m_bHundertProzent = TRUE;
        m_bGrid = TRUE;
        m_gridColor = RGB(0, 0, 128);
        m_bActive = FALSE;
}

CBaseView::~CBaseView()
{
}
BOOL CBaseView::PreCreateWindow(CREATESTRUCT& cs)
{
        ASSERT_VALID(this);
        ASSERT(cs.style & WS_CHILD);
        if (cs.lpszClass == NULL)
                cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS);
        return TRUE;
}

void CBaseView::OnActivateView(BOOL bActivate, CView* pActiveView,
        CView* pDeactiveView)
{
    ASSERT_VALID(this);
        CView::OnActivateView(bActivate, pActiveView, pDeactiveView);


        // invalidate selections when active status changes
        if (m_bActive != bActivate)
        {
                if (bActivate)  // if becoming active update as if active
                {
                        m_bActive = bActivate;
                }

                if (!m_selection.IsEmpty())
                        OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);
                m_bActive = bActivate;
        }
}

/////////////////////////////////////////////////////////////////////////////
// CBaseView drawing

void CBaseView::InvalObj(CDrawObj* pObj)
{
    ASSERT_VALID(this);
        CRect rect = pObj->GetRect();
        DocToClient(rect);
        if (m_bActive && IsSelected(pObj))
        {
                rect.left -= 4;
                rect.top -= 5;
                rect.right += 5;
                rect.bottom += 4;
        }
        rect.InflateRect(1, 1); // handles CDrawOleObj objects

        InvalidateRect(rect, FALSE);
}

void CBaseView::OnUpdate(CView* , LPARAM lHint, CObject* pHint)
{
    ASSERT_VALID(this);
        switch (lHint)
        {
        case HINT_UPDATE_WINDOW:    // redraw entire window
                Invalidate(FALSE);
                break;

        case HINT_UPDATE_DRAWOBJ:   // a single object has changed
              /*  {
                        CObList* pList = ((CDrawObj*) pHint)->GetViewList();
                        if (pList->Find(this) == NULL) ((CDrawObj*) pHint)->SetView(this);*/
                        InvalObj((CDrawObj*)pHint);
                //}
                break;

        case HINT_UPDATE_SELECTION: // an entire selection has changed
                {
                        CObList* pList = pHint != NULL ? (CObList*)pHint : &m_selection;
                        POSITION pos = pList->GetHeadPosition();
                        while (pos != NULL)
                                InvalObj((CDrawObj*)pList->GetNext(pos));
                }
                break;

        case HINT_DELETE_SELECTION: // an entire selection has been removed
                if (pHint != &m_selection)
                {
                        CObList* pList = (CObList*)pHint;
                        POSITION pos = pList->GetHeadPosition();
                        while (pos != NULL)
                        {
                                CDrawObj* pObj = (CDrawObj*)pList->GetNext(pos);
                                InvalObj(pObj);
                                Remove(pObj);   // remove it from this view's selection
                        }
                }
                break;

        case HINT_UPDATE_OLE_ITEMS:
                {
                        POSITION pos = GetDocObjects()->GetHeadPosition();
                        while (pos != NULL)
                        {
                                CDrawObj* pObj = (CDrawObj*) GetDocObjects()->GetNext(pos);
                                if (pObj->IsKindOf(RUNTIME_CLASS(CDrawOleObj)))
                                        InvalObj(pObj);
                        }
                }
                break;

        default:
                ASSERT(FALSE);
                break;
        }
}

void CBaseView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
    ASSERT_VALID(this);
        CScrollView::OnPrepareDC(pDC, pInfo);

        // mapping mode is MM_ANISOTROPIC
        // these extents setup a mode similar to MM_LOENGLISH
        // MM_LOENGLISH is in .01 physical inches
        // these extents provide .01 logical inches

        pDC->SetMapMode(MM_ANISOTROPIC);
        pDC->SetViewportExt(pDC->GetDeviceCaps(LOGPIXELSX),
                pDC->GetDeviceCaps(LOGPIXELSY));
        if (m_bHundertProzent)
            pDC->SetWindowExt(100, -100);
        else
            pDC->SetWindowExt(m_Zoom, -m_Zoom);

        // set the origin of the coordinate system to the center of the page
        CPoint ptOrg;
        ptOrg.x = GetDocument()->GetSize().cx / 2;
        ptOrg.y = GetDocument()->GetSize().cy / 2;

        // ptOrg is in logical coordinates
        pDC->OffsetWindowOrg(-ptOrg.x,ptOrg.y);
}

BOOL CBaseView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll)
{
    ASSERT_VALID(this);
        // do the scroll
        if (!CScrollView::OnScrollBy(sizeScroll, bDoScroll))
                return FALSE;

        // update the position of any in-place active item
        if (bDoScroll)
        {
                UpdateActiveItem();
                UpdateWindow();
        }
        return TRUE;
}

void CBaseView::OnDraw(CDC* pDC)
{
    ASSERT_VALID(this);
        CDrawDoc* pDoc = GetDocument();
        ASSERT_VALID(pDoc);

        CDC dc;
        CDC* pDrawDC = pDC;
        CBitmap bitmap;
        CBitmap* pOldBitmap;

        // only paint the rect that needs repainting
        CRect client;
        pDC->GetClipBox(client);
        CRect rect = client;
        DocToClient(rect);

        if (!pDC->IsPrinting())
        {
                // draw to offscreen bitmap for fast looking repaints
                if (dc.CreateCompatibleDC(pDC))
                {
                        if (bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height()))
                        {
                                OnPrepareDC(&dc, NULL);
                                pDrawDC = &dc;

                                // offset origin more because bitmap is just piece of the whole drawing
                                dc.OffsetViewportOrg(-rect.left, -rect.top);
                                pOldBitmap = dc.SelectObject(&bitmap);
                                dc.SetBrushOrg(rect.left % 8, rect.top % 8);

                                // might as well clip to the same rectangle
                                dc.IntersectClipRect(client);
                        }
                }
        }

        // paint background
        CBrush brush;
        if (!brush.CreateSolidBrush(pDoc->GetPaperColor()))
                return;

        brush.UnrealizeObject();
        pDrawDC->FillRect(client, &brush);

        if (!pDC->IsPrinting() && m_bGrid)
                DrawGrid(pDrawDC);

        pDoc->Draw(pDrawDC, this);

        if (pDrawDC != pDC)
        {
                pDC->SetViewportOrg(0, 0);
                pDC->SetWindowOrg(0,0);
                pDC->SetMapMode(MM_TEXT);
                dc.SetViewportOrg(0, 0);
                dc.SetWindowOrg(0,0);
                dc.SetMapMode(MM_TEXT);
                pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(),
                        &dc, 0, 0, SRCCOPY);
                dc.SelectObject(pOldBitmap);
        }
}

void CBaseView::Remove(CDrawObj* pObj)
{
    ASSERT_VALID(this);
        POSITION pos = m_selection.Find(pObj);
        if (pos != NULL)
                m_selection.RemoveAt(pos);
}

void CBaseView::PasteNative(COleDataObject& dataObject)
{
    ASSERT_VALID(this);
        // get file refering to clipboard data
        CFile* pFile = dataObject.GetFileData(m_cfDraw);
        if (pFile == NULL)
                return;

        // connect the file to the archive
        CArchive ar(pFile, CArchive::load);
        TRY
        {
                ar.m_pDocument = GetDocument(); // set back-pointer in archive

                // read the selection
                m_selection.Serialize(ar);
        }
        CATCH_ALL(e)
        {
                ar.Close();
                delete pFile;
                THROW_LAST();
        }
        END_CATCH_ALL

        ar.Close();
        delete pFile;
}

void CBaseView::PasteEmbedded(COleDataObject& dataObject)
{
    ASSERT_VALID(this);
        BeginWaitCursor();

        // paste embedded
        CDrawOleObj* pObj = new CDrawOleObj(GetInitialPosition());
        ASSERT_VALID(pObj);
        CDrawItem* pItem = new CDrawItem(GetDocument(), pObj);
        ASSERT_VALID(pItem);
        pObj->m_pClientItem = pItem;

        TRY
        {
                if (!pItem->CreateFromData(&dataObject) &&
                        !pItem->CreateStaticFromData(&dataObject))
                {
                        AfxThrowMemoryException();      // any exception will do
                }

                // add the object to the document
                GetDocument()->Add(pObj, this);
                m_selection.AddTail(pObj);

                // try to get initial presentation data
                pItem->UpdateLink();
                pItem->UpdateExtent();
        }
        CATCH_ALL(e)
        {
                // clean up item
                pItem->Delete();
                pObj->m_pClientItem = NULL;
                GetDocument()->Remove(pObj, this);
                delete pObj;

                AfxMessageBox(IDP_FAILED_TO_CREATE);
        }
        END_CATCH_ALL

        EndWaitCursor();
}

void CBaseView::DrawGrid(CDC* pDC)
{
    ASSERT_VALID(this);
        CDrawDoc* pDoc = GetDocument();

        COLORREF oldBkColor = pDC->SetBkColor(pDoc->GetPaperColor());

        CRect rect;
        rect.left = -pDoc->GetSize().cx / 2;
        rect.top = -pDoc->GetSize().cy / 2;
        rect.right = rect.left + pDoc->GetSize().cx;
        rect.bottom = rect.top + pDoc->GetSize().cy;

        // Center lines
        CPen penDash;
        penDash.CreatePen(PS_DASH, 1, m_gridColor);
        CPen* pOldPen = pDC->SelectObject(&penDash);

        pDC->MoveTo(0, rect.top);
        pDC->LineTo(0, rect.bottom);
        pDC->MoveTo(rect.left, 0);
        pDC->LineTo(rect.right, 0);

        // Major unit lines
        CPen penDot;
        penDot.CreatePen(PS_DOT, 1, m_gridColor);
        pDC->SelectObject(&penDot);

        for (int x = rect.left / 100 * 100; x < rect.right; x += 100)
        {
                if (x != 0)
                {
                        pDC->MoveTo(x, rect.top);
                        pDC->LineTo(x, rect.bottom);
                }
        }

        for (int y = rect.top / 100 * 100; y < rect.bottom; y += 100)
        {
                if (y != 0)
                {
                        pDC->MoveTo(rect.left, y);
                        pDC->LineTo(rect.right, y);
                }
        }

        // Outlines
        CPen penSolid;
        penSolid.CreatePen(PS_SOLID, 1, m_gridColor);
        pDC->SelectObject(&penSolid);
        pDC->MoveTo(rect.left, rect.top);
        pDC->LineTo(rect.right, rect.top);
        pDC->LineTo(rect.right, rect.bottom);
        pDC->LineTo(rect.left, rect.bottom);
        pDC->LineTo(rect.left, rect.top);

        pDC->SelectObject(pOldPen);
        pDC->SetBkColor(oldBkColor);
}

void CBaseView::SetObjectsToView()
// View an Objekte anmelden.
{
    ASSERT_VALID(this);
    CObList* list = GetDocObjects();

    POSITION pos = list->GetHeadPosition();

    while (pos != NULL)
    {
        ((CDrawObj*) list->GetNext(pos))->SetView(this);
    }
}

void CBaseView::OnInitialUpdate()
{
    ASSERT_VALID(this);
    CView::OnInitialUpdate();
    CSize size = GetDocument()->GetSize();
    CClientDC dc(NULL);
    size.cx = MulDiv(size.cx, dc.GetDeviceCaps(LOGPIXELSX), 100);
    size.cy = MulDiv(size.cy, dc.GetDeviceCaps(LOGPIXELSY), 100);
    SetScrollSizes(MM_TEXT, size);

    SetObjectsToView();
}

void CBaseView::SetPageSize(CSize size)
{
    ASSERT_VALID(this);
        CClientDC dc(NULL);
        size.cx = MulDiv(size.cx, dc.GetDeviceCaps(LOGPIXELSX), m_Zoom);
        size.cy = MulDiv(size.cy, dc.GetDeviceCaps(LOGPIXELSY), m_Zoom);
        SetScrollSizes(MM_TEXT, size);
        GetDocument()->UpdateAllViews(NULL, HINT_UPDATE_WINDOW, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CBaseView printing

BOOL CBaseView::OnPreparePrinting(CPrintInfo* pInfo)
{
    ASSERT_VALID(this);
        // default preparation
        return DoPreparePrinting(pInfo);
}

void CBaseView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
    ASSERT_VALID(this);
        CScrollView::OnBeginPrinting(pDC,pInfo);

        // check page size -- user could have gone into print setup
        // from print dialog and changed paper or orientation
        GetDocument()->ComputePageSize();
}

void CBaseView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    ASSERT_VALID(this);
        // TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// OLE Client support and commands

BOOL CBaseView::IsSelected(const CObject* pDocItem) const
{
    ASSERT_VALID(this);
        CDrawObj* pDrawObj = (CDrawObj*)pDocItem;
        if (pDocItem->IsKindOf(RUNTIME_CLASS(CDrawItem)))
                pDrawObj = ((CDrawItem*)pDocItem)->m_pDrawObj;
        return m_selection.Find(pDrawObj) != NULL;
}

void CBaseView::OnInsertObject()
{
    ASSERT_VALID(this);
        // Invoke the standard Insert Object dialog box to obtain information
        //  for new CDrawItem object.
        COleInsertDialog dlg;
        if (dlg.DoModal() != IDOK)
                return;

        BeginWaitCursor();

        // First create the C++ object
        CDrawOleObj* pObj = new CDrawOleObj(GetInitialPosition());
        ASSERT_VALID(pObj);
        CDrawItem* pItem = new CDrawItem(GetDocument(), pObj);
        ASSERT_VALID(pItem);
        pObj->m_pClientItem = pItem;

        // Now create the OLE object/item
        TRY
        {
                if (!dlg.CreateItem(pObj->m_pClientItem))
                        AfxThrowMemoryException();

                // add the object to the document
                GetDocument()->Add(pObj, this);

                // try to get initial presentation data
                pItem->UpdateLink();
                pItem->UpdateExtent();

                // if insert new object -- initially show the object
                if (dlg.GetSelectionType() == COleInsertDialog::createNewItem)
                        pItem->DoVerb(OLEIVERB_SHOW, this);
        }
        CATCH_ALL(e)
        {
                // clean up item
                pItem->Delete();
                pObj->m_pClientItem = NULL;
                GetDocument()->Remove(pObj, this);
                delete pObj;

                AfxMessageBox(IDP_FAILED_TO_CREATE);
        }
        END_CATCH_ALL

        EndWaitCursor();
}

// The following command handler provides the standard keyboard
//  user interface to cancel an in-place editing session.
void CBaseView::OnCancelEdit()
{
    ASSERT_VALID(this);
        // deactivate any in-place active item on this view!
        COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
        if (pActiveItem != NULL)
        {
                // if we found one, deactivate it
                pActiveItem->Close();
        }
        ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);

        // escape also brings us back into select mode
        ReleaseCapture();

        CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
        if (pTool != NULL)
                pTool->OnCancel();

        CDrawTool::c_drawShape = select;
}

void CBaseView::OnSetFocus(CWnd* pOldWnd)
{
    ASSERT_VALID(this);
        COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
        if (pActiveItem != NULL &&
                pActiveItem->GetItemState() == COleClientItem::activeUIState)
        {
                // need to set focus to this item if it is in the same view
                CWnd* pWnd = pActiveItem->GetInPlaceWindow();
                if (pWnd != NULL)
                {
                        pWnd->SetFocus();
                        return;
                }
        }

        CScrollView::OnSetFocus(pOldWnd);
}

CRect CBaseView::GetInitialPosition()
{
    ASSERT_VALID(this);
        CRect rect(10, 10, 10, 10);
        ClientToDoc(rect);
        return rect;
}

void CBaseView::ClientToDoc(CPoint& point)
{
    ASSERT_VALID(this);
        CClientDC dc(this);
        OnPrepareDC(&dc, NULL);
        dc.DPtoLP(&point);
}

void CBaseView::ClientToDoc(CRect& rect)
{
    ASSERT_VALID(this);
        CClientDC dc(this);
        OnPrepareDC(&dc, NULL);
        dc.DPtoLP(rect);
        ASSERT(rect.left <= rect.right);
        ASSERT(rect.bottom <= rect.top);
}

void CBaseView::DocToClient(CPoint& point)
{
    ASSERT_VALID(this);
        CClientDC dc(this);
        OnPrepareDC(&dc, NULL);
        dc.LPtoDP(&point);
}

void CBaseView::DocToClient(CRect& rect)
{
    ASSERT_VALID(this);
        CClientDC dc(this);
        OnPrepareDC(&dc, NULL);
        dc.LPtoDP(rect);
        rect.NormalizeRect();
}

CObList* CBaseView::GetDocObjects()
{
    ASSERT(FALSE);
    return NULL;
}


// PageView bezieht sich hier nur auf GetDocObjects (eine Seite)
CObList* CBaseView::GetPage()
{
    return GetDocObjects();
}

CString CBaseView::GetActualPage()
{
    // Nur eine Seite, damit CDrawObj.FindView funktioniert
    CString S = CString("1");
    return S;
}

void CBaseView::UnSelectAllHidden()
{
    m_hide_selection.RemoveAll();
}

void CBaseView::Select(CDrawObj* pObj, BOOL bAdd, BOOL bHidden)
{
        if (!bAdd)
        {
                OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);
                m_selection.RemoveAll();
        }

        if (pObj == NULL || IsSelected(pObj))
                return;

        if (bHidden)
        {
            m_hide_selection.AddTail(pObj);
        }
        else
        {
            m_selection.AddTail(pObj);
            InvalObj(pObj);
        }
}

// rect is in device coordinates
void CBaseView::SelectWithinRect(CRect rect, BOOL bAdd)
{
        if (!bAdd)
                Select(NULL);

        ClientToDoc(rect);

        CObList* pObList = GetDocObjects();
        POSITION posObj = pObList->GetHeadPosition();
        while (posObj != NULL)
        {
                CDrawObj* pObj = (CDrawObj*)pObList->GetNext(posObj);
                if (pObj->Intersects(rect))
                        Select(pObj, TRUE);
        }
}

void CBaseView::Deselect(CDrawObj* pObj)
{
        POSITION pos = m_selection.Find(pObj);
        if (pos != NULL)
        {
                InvalObj(pObj);
                m_selection.RemoveAt(pos);
        }
}

void CBaseView::CloneSelection()
{
        POSITION pos = m_selection.GetHeadPosition();
        while (pos != NULL)
        {
                CDrawObj* pObj = (CDrawObj*)m_selection.GetNext(pos);
                pObj->Clone(pObj->GetDoc());
                        // copies object and adds it to the document
        }
}

void CBaseView::UpdateActiveItem()
{
        ASSERT(GetDocument() != NULL);
        COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
        if (pActiveItem != NULL &&
                pActiveItem->GetItemState() == COleClientItem::activeUIState)
        {
                // this will update the item rectangles by calling
                //  OnGetPosRect & OnGetClipRect.
                pActiveItem->SetItemRects();
        }
}

/////////////////////////////////////////////////////////////////////////////
// CBaseView message handlers

void CBaseView::OnLButtonDown(UINT nFlags, CPoint point)
{
        if (!m_bActive)
                return;
        CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
        if (pTool != NULL)
        {
            Moves = OMoves = 0;
            
            pTool->OnLButtonDown(this, nFlags, point);
        }
}

void CBaseView::OnLButtonUp(UINT nFlags, CPoint point)
{
        if (!m_bActive)
                return;
        CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
        if (pTool != NULL)
                pTool->OnLButtonUp(this, nFlags, point);

        char text[100];
        sprintf(text, "Moves %d, Objectmoves %d.", Moves, OMoves);

        SetStatusText(CString(text));
}

void CBaseView::SetStatusText(CString S)
{
        CMainFrame* w = (CMainFrame*) AfxGetMainWnd();
        CStatusBar* St = ((CStatusBar*) w->GetDlgItem(AFX_IDW_STATUS_BAR));
        St->SetPaneText(1, S);
}

void CBaseView::OnMouseMove(UINT nFlags, CPoint point)
{
        if (!m_bActive)
                return;
        CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
        if (pTool != NULL)
        {
            Moves++;
            pTool->OnMouseMove(this, nFlags, point);
        }
}

void CBaseView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
        if (!m_bActive)
                return;
        CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
        if (pTool != NULL)
                pTool->OnLButtonDblClk(this, nFlags, point);
}

void CBaseView::OnDestroy()
{
        CScrollView::OnDestroy();

        // deactivate the inplace active item on this view
        COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
        if (pActiveItem != NULL && pActiveItem->GetActiveView() == this)
        {
                pActiveItem->Deactivate();
                ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
        }
}

void CBaseView::OnDrawSelect()
{
        CDrawTool::c_drawShape = select;
}

void CBaseView::OnUpdateDrawSelect(CCmdUI* pCmdUI)
{
        pCmdUI->SetRadio(CDrawTool::c_drawShape == select);
}

void CBaseView::OnUpdateSingleSelect(CCmdUI* pCmdUI)
{
        pCmdUI->Enable(m_selection.GetCount() == 1);
}

void CBaseView::OnEditSelectAll()
{
        CObList* pObList = GetDocObjects();
        POSITION pos = pObList->GetHeadPosition();
        
        while (pos != NULL)
                Select((CDrawObj*)pObList->GetNext(pos), TRUE);
}

void CBaseView::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{
        pCmdUI->Enable(GetDocObjects()->GetCount() != 0);
}

void CBaseView::OnEditClear()
{
        CObList* DeleteList = new CObList;
        CObList* ProblemList = new CObList;
        
        POSITION pos = m_selection.GetHeadPosition();
        while (pos != NULL)
        {
            CDrawObj* pObj = (CDrawObj*) m_selection.GetNext(pos);

            char buf[100];

            if (!pObj->SearchDeleteAbleObjects(DeleteList, ProblemList))
            {
                sprintf(buf, "Objekt kann nicht gelöscht werden!\n Objekte zu löschen: %d\n Objekte mit Problemen: %d",
                        DeleteList->GetCount(), ProblemList->GetCount());
                delete DeleteList;
                delete ProblemList;
                AfxMessageBox(buf);
                return;
            }
            else
            {
                m_selection.RemoveAll();

                pos = DeleteList->GetHeadPosition();

                while (pos != NULL)
                {
                    m_selection.AddTail(DeleteList->GetNext(pos));
                }

                if (m_selection.Find(pObj) == NULL) m_selection.AddTail(pObj);
                
                GetDocument()->UpdateAllViews(NULL, HINT_DELETE_SELECTION, &m_selection);
                OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);

                // now remove the selection from the document (nur Kanten)
                pos = DeleteList->GetHeadPosition();
                while (pos != NULL)
                {
                    CDrawObj* pObj = (CDrawObj*)DeleteList->GetNext(pos);
                    ASSERT_VALID(pObj);
                    pObj->CutConnections();
                }

                pos = DeleteList->GetHeadPosition();
                while (pos != NULL)
                {
                    CDrawObj* pObj = (CDrawObj*)DeleteList->GetNext(pos);
                    ASSERT_VALID(pObj);
                    GetDocument()->Remove(pObj, this);
                    //delete pObj;
                }

                delete DeleteList;
                delete ProblemList;
                return;
            }
        }
        
    
        // update all the views before the selection goes away
        GetDocument()->UpdateAllViews(NULL, HINT_DELETE_SELECTION, &m_selection);
        OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);

        // now remove the selection from the document (nur Kanten)
        pos = m_selection.GetHeadPosition();
        while (pos != NULL)
        {
                CDrawObj* pObj = (CDrawObj*)m_selection.GetNext(pos);

                if (pObj->Netz == CDrawObj::Kante)
                {
                    GetDocument()->Remove(pObj, this);
                    delete pObj;
                }
        }

        pos = m_selection.GetHeadPosition();
        while (pos != NULL)
        {
            CDrawObj* pObj = (CDrawObj*)m_selection.GetNext(pos);

            if (pObj->Netz == CDrawObj::Kante)
            {
                POSITION pos1 = m_selection.Find(pObj);
                m_selection.RemoveAt(pos1);
            }
        }

        // now remove the selection from the document
        pos = m_selection.GetHeadPosition();
        while (pos != NULL)
        {
                CDrawObj* pObj = (CDrawObj*)m_selection.GetNext(pos);
                GetDocument()->Remove(pObj, this);
                delete pObj;
        }
        //GetDocument()->m_deletedobjects.RemoveAll();
        m_selection.RemoveAll();
}

void CBaseView::OnUpdateAnySelect(CCmdUI* pCmdUI)
{
        pCmdUI->Enable(!m_selection.IsEmpty());
}

void CBaseView::OnSize(UINT nType, int cx, int cy)
{
        CScrollView::OnSize(nType, cx, cy);
        UpdateActiveItem();
}

void CBaseView::UpDateScroller()
{
        CClientDC dc(this);
        OnPrepareDC(&dc, NULL);

        CSize size = GetDocument()->GetSize();
        if (m_bHundertProzent)
        {
            size.cx = MulDiv(size.cx, dc.GetDeviceCaps(LOGPIXELSX), 100);
            size.cy = MulDiv(size.cy, dc.GetDeviceCaps(LOGPIXELSY), 100);
        }
        else
        {
            size.cx = MulDiv(size.cx, dc.GetDeviceCaps(LOGPIXELSX), m_Zoom);
            size.cy = MulDiv(size.cy, dc.GetDeviceCaps(LOGPIXELSY), m_Zoom);
        }    
        SetScrollSizes(MM_TEXT, size);
}

void CBaseView::OnZoom()
{
        ASSERT_VALID(this);

        CZoomDlg dlg(this);
        dlg.m_bHundertProzent = m_bHundertProzent;
        dlg.m_Prozent = m_Zoom;


        if (dlg.DoModal() != IDOK)
                return;

        m_bHundertProzent = dlg.m_bHundertProzent;
        m_Zoom = dlg.m_Prozent;

//        CClientDC dc(this);
//        OnPrepareDC(&dc, NULL);

        UpDateScroller();
        
        Invalidate(FALSE);
}

void CBaseView::OnHundertProzent()
{
    m_bHundertProzent = !m_bHundertProzent;
//    CClientDC dc(this);
//    OnPrepareDC(&dc, NULL);

    UpDateScroller();

    Invalidate(FALSE);
}

void CBaseView::OnUpdateHundertProzent(CCmdUI* pCmdUI)
{
        pCmdUI->SetCheck(m_bHundertProzent);
}

void CBaseView::OnViewGrid()
{
        m_bGrid = !m_bGrid;
        Invalidate(FALSE);
}

void CBaseView::OnUpdateViewGrid(CCmdUI* pCmdUI)
{
        pCmdUI->SetCheck(m_bGrid);
}

BOOL CBaseView::OnEraseBkgnd(CDC* pDC)
{
        return TRUE;
}

void CBaseView::OnObjectFillColor()
{
        CColorDialog dlg;
        if (dlg.DoModal() != IDOK)
                return;

        COLORREF color = dlg.GetColor();

        POSITION pos = m_selection.GetHeadPosition();
        while (pos != NULL)
        {
                CDrawObj* pObj = (CDrawObj*)m_selection.GetNext(pos);
                pObj->SetFillColor(color);
        }
}

void CBaseView::OnObjectLineColor()
{
        CColorDialog dlg;
        if (dlg.DoModal() != IDOK)
                return;

        COLORREF color = dlg.GetColor();

        POSITION pos = m_selection.GetHeadPosition();
        while (pos != NULL)
        {
                CDrawObj* pObj = (CDrawObj*)m_selection.GetNext(pos);
                pObj->SetLineColor(color);
        }
}

void CBaseView::OnObjectMoveBack()
{/*
        CDrawDoc* pDoc = GetDocument();
        CDrawObj* pObj = (CDrawObj*)m_selection.GetHead();
        CObList* pObjects = pDoc->GetObjects();
        POSITION pos = pObjects->Find(pObj);
        ASSERT(pos != NULL);
        if (pos != pObjects->GetHeadPosition())
        {
                POSITION posPrev = pos;
                pObjects->GetPrev(posPrev);
                pObjects->RemoveAt(pos);
                pObjects->InsertBefore(posPrev, pObj);
                InvalObj(pObj);
        }*/
}

void CBaseView::OnObjectMoveForward()
{/*
        CDrawDoc* pDoc = GetDocument();
        CDrawObj* pObj = (CDrawObj*)m_selection.GetHead();
        CObList* pObjects = pDoc->GetObjects();
        POSITION pos = pObjects->Find(pObj);
        ASSERT(pos != NULL);
        if (pos != pObjects->GetTailPosition())
        {
                POSITION posNext = pos;
                pObjects->GetNext(posNext);
                pObjects->RemoveAt(pos);
                pObjects->InsertAfter(posNext, pObj);
                InvalObj(pObj);
        }*/
}

void CBaseView::OnObjectMoveToBack()
{/*
        CDrawDoc* pDoc = GetDocument();
        CDrawObj* pObj = (CDrawObj*)m_selection.GetHead();
        CObList* pObjects = pDoc->GetObjects();
        POSITION pos = pObjects->Find(pObj);
        ASSERT(pos != NULL);
        pObjects->RemoveAt(pos);
        pObjects->AddHead(pObj);
        InvalObj(pObj);*/
}

void CBaseView::OnObjectMoveToFront()
{/*
        CDrawDoc* pDoc = GetDocument();
        CDrawObj* pObj = (CDrawObj*)m_selection.GetHead();
        CObList* pObjects = pDoc->GetObjects();
        POSITION pos = pObjects->Find(pObj);
        ASSERT(pos != NULL);
        pObjects->RemoveAt(pos);
        pObjects->AddTail(pObj);
        InvalObj(pObj);*/
}

void CBaseView::OnEditCopy()
{
        ASSERT_VALID(this);
        ASSERT(m_cfDraw != NULL);

        // Create a shared file and associate a CArchive with it
        CSharedFile file;
        CArchive ar(&file, CArchive::store);

        // Serialize selected objects to the archive   !!!
        //
        // KillConnections wird nur beim Löschen benötigt.
        // Es muß eine exakte Kopie (Layer) erstellt werden,
        // die dann quer verbunden wird, sowie neu nummeriert
        // wird.

        CObList CopyList;

        POSITION pos = m_selection.GetHeadPosition();

        while (pos != NULL)
        {
            CDrawObj* pObj = (CDrawObj*) m_selection.GetNext(pos);

            // Kopieren, um Nummer zu verändern.
            
            CDrawObj* pObjCopy = pObj->Clone(NULL);

            pObjCopy->SetDoc(GetDocument());

            // Querverbindung herstellen,
            // damit später Connections wieder aufgebaut werden können.

            pObj->SetConnectionTyp("CT_CopyServer");
            pObj->SetMoveTyp(MT_MoveRel);
            pObj->SetConnection(pObjCopy);

            pObjCopy->SetConnectionTyp("CT_CopyClient");
            pObjCopy->SetMoveTyp(MT_OnlySelf);
            pObjCopy->SetConnection(pObj);

            ASSERT_VALID(pObjCopy->GetDoc());
            ASSERT_VALID(pObj->GetDoc());
            //ASSERT(pObj->IsKindOf(RUNTIME_CLASS(CDataElem)));         
            CopyList.AddTail(pObjCopy);
        }
        CopyList.Serialize(ar); // Nicht m_selection, sondern CopyList !!!
        ar.Close();

        COleDataSource* pDataSource = NULL;

        CDrawOleObj* pDrawOle = (CDrawOleObj*)m_selection.GetHead();
        TRY
        {
                pDataSource = new COleDataSource;
                // put on local format instead of or in addation to
                pDataSource->CacheGlobalData(m_cfDraw, file.Detach());

                // if only one item and it is a COleClientItem then also
                // paste in that format
                if (m_selection.GetCount() == 1 &&
                        pDrawOle->IsKindOf(RUNTIME_CLASS(CDrawOleObj)))
                {
                        pDrawOle->m_pClientItem->GetClipboardData(pDataSource, FALSE);
                }
                pDataSource->SetClipboard();
        }
        CATCH_ALL(e)
        {
                delete pDataSource;
                THROW_LAST();
        }
        END_CATCH_ALL
}

void CBaseView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
        pCmdUI->Enable(!m_selection.IsEmpty());
}

void CBaseView::OnEditCut()
{
        OnEditCopy();
        OnEditClear();
}

void CBaseView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
        pCmdUI->Enable(!m_selection.IsEmpty());
}

void CBaseView::OnEditPaste()
{
        COleDataObject dataObject;
        dataObject.AttachClipboard();

        // invalidate current selection since it will be deselected
        OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);
        m_selection.RemoveAll();
        if (dataObject.IsDataAvailable(m_cfDraw))
        {
                PasteNative(dataObject);

                // now add all items in m_selection to document
                POSITION pos = m_selection.GetHeadPosition();
                while (pos != NULL)
                        GetDocument()->Add((CDrawObj*)m_selection.GetNext(pos), this);
        }
        else
                PasteEmbedded(dataObject);

        GetDocument()->SetModifiedFlag();

        // invalidate new pasted stuff
        GetDocument()->UpdateAllViews(NULL, HINT_UPDATE_SELECTION, &m_selection);
}

void CBaseView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
        // determine if private or standard OLE formats are on the clipboard
        COleDataObject dataObject;
        BOOL bEnable = dataObject.AttachClipboard() &&
                (dataObject.IsDataAvailable(m_cfDraw) ||
                 COleClientItem::CanCreateFromData(&dataObject));

        // enable command based on availability
        pCmdUI->Enable(bEnable);
}

void CBaseView::OnFilePrint()
{
        CScrollView::OnFilePrint();
        GetDocument()->ComputePageSize();
}

void CBaseView::OnViewShowObjects()
{
        CDrawOleObj::c_bShowItems = !CDrawOleObj::c_bShowItems;
        GetDocument()->UpdateAllViews(NULL, HINT_UPDATE_OLE_ITEMS, NULL);
}

void CBaseView::OnUpdateViewShowObjects(CCmdUI* pCmdUI)
{
        pCmdUI->SetCheck(CDrawOleObj::c_bShowItems);
}

void CBaseView::OnEditProperties()
{
        if (m_selection.GetCount() == 1 && CDrawTool::c_drawShape == select)
        {
                CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
                ASSERT(pTool != NULL);
                pTool->OnLButtonDblClk(this, 0, CPoint(0, 0));
        }
}

void CBaseView::OnUpdateEditProperties(CCmdUI* pCmdUI)
{
        pCmdUI->Enable(m_selection.GetCount() == 1 &&
                CDrawTool::c_drawShape == select);
}

/////////////////////////////////////////////////////////////////////////////
// CBaseView diagnostics

#ifdef _DEBUG
void CBaseView::AssertValid() const
{
        CScrollView::AssertValid();
}

void CBaseView::Dump(CDumpContext& dc) const
{
        CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

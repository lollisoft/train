// drawvw.cpp : implementation of the CFahrplanView class
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
#include <afxpriv.h>

#include "drawcli.h"

#include "drawdoc.h"
#include "drawobj.h"
#include "cntritem.h"
#include "fahrpvw.h"

#include "drawtool.h"
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// private clipboard format (list of Draw objects)
CLIPFORMAT CFahrplanView::m_cfDraw =
        (CLIPFORMAT)::RegisterClipboardFormat("MFC Draw Sample");

/////////////////////////////////////////////////////////////////////////////
// CFahrplanView

IMPLEMENT_DYNCREATE(CFahrplanView, CScrollView)

BEGIN_MESSAGE_MAP(CFahrplanView, CScrollView)
        //{{AFX_MSG_MAP(CFahrplanView)
        ON_COMMAND(ID_OLE_INSERT_NEW, OnInsertObject)
        ON_COMMAND(ID_CANCEL_EDIT, OnCancelEdit)
        ON_WM_LBUTTONDOWN()
        ON_WM_LBUTTONUP()
        ON_WM_MOUSEMOVE()
        ON_WM_LBUTTONDBLCLK()
        ON_COMMAND(ID_DRAW_SELECT, OnDrawSelect)
        ON_COMMAND(ID_DRAW_ROUNDRECT, OnDrawRoundRect)
        ON_COMMAND(ID_DRAW_RECT, OnDrawRect)
        ON_COMMAND(ID_DRAW_LINE, OnDrawLine)
        ON_COMMAND(ID_DRAW_ELLIPSE, OnDrawEllipse)

        ON_COMMAND(ID_DRAW_BAHNHOF, OnDrawBahnhof)
        ON_UPDATE_COMMAND_UI(ID_DRAW_BAHNHOF, OnUpdateDrawBahnhof)
        ON_COMMAND(ID_CONNECT_BAHNHOF, OnConnectBahnhof)
        ON_UPDATE_COMMAND_UI(ID_CONNECT_BAHNHOF, OnUpdateConnectBahnhof)



        ON_UPDATE_COMMAND_UI(ID_DRAW_ELLIPSE, OnUpdateDrawEllipse)
        ON_UPDATE_COMMAND_UI(ID_DRAW_LINE, OnUpdateDrawLine)
        ON_UPDATE_COMMAND_UI(ID_DRAW_RECT, OnUpdateDrawRect)
        ON_UPDATE_COMMAND_UI(ID_DRAW_ROUNDRECT, OnUpdateDrawRoundRect)
        ON_UPDATE_COMMAND_UI(ID_DRAW_SELECT, OnUpdateDrawSelect)
        ON_UPDATE_COMMAND_UI(ID_OBJECT_MOVEBACK, OnUpdateSingleSelect)
        ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
        ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
        ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateAnySelect)
        ON_COMMAND(ID_DRAW_POLYGON, OnDrawPolygon)
        ON_UPDATE_COMMAND_UI(ID_DRAW_POLYGON, OnUpdateDrawPolygon)
        ON_WM_SIZE()
        ON_COMMAND(ID_VIEW_GRID, OnViewGrid)
        ON_UPDATE_COMMAND_UI(ID_VIEW_GRID, OnUpdateViewGrid)
        ON_WM_ERASEBKGND()
        ON_COMMAND(ID_OBJECT_FILLCOLOR, OnObjectFillColor)
        ON_COMMAND(ID_OBJECT_LINECOLOR, OnObjectLineColor)
        ON_COMMAND(ID_OBJECT_MOVEBACK, OnObjectMoveBack)
        ON_COMMAND(ID_OBJECT_MOVEFORWARD, OnObjectMoveForward)
        ON_COMMAND(ID_OBJECT_MOVETOBACK, OnObjectMoveToBack)
        ON_COMMAND(ID_OBJECT_MOVETOFRONT, OnObjectMoveToFront)
        ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
        ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
        ON_COMMAND(ID_EDIT_CUT, OnEditCut)
        ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
        ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
        ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
        ON_WM_SETFOCUS()
        ON_COMMAND(ID_VIEW_SHOWOBJECTS, OnViewShowObjects)
        ON_UPDATE_COMMAND_UI(ID_VIEW_SHOWOBJECTS, OnUpdateViewShowObjects)
        ON_COMMAND(ID_EDIT_PROPERTIES, OnEditProperties)
        ON_UPDATE_COMMAND_UI(ID_EDIT_PROPERTIES, OnUpdateEditProperties)
        ON_WM_DESTROY()
        ON_UPDATE_COMMAND_UI(ID_OBJECT_MOVEFORWARD, OnUpdateSingleSelect)
        ON_UPDATE_COMMAND_UI(ID_OBJECT_MOVETOBACK, OnUpdateSingleSelect)
        ON_UPDATE_COMMAND_UI(ID_OBJECT_MOVETOFRONT, OnUpdateSingleSelect)
        ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
        //}}AFX_MSG_MAP
        // Standard printing commands
        ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
        ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFahrplanView construction/destruction

CFahrplanView::CFahrplanView()
{
        m_bGrid = TRUE;
        m_gridColor = RGB(0, 0, 128);
        m_bActive = FALSE;
}

CFahrplanView::~CFahrplanView()
{
}

BOOL CFahrplanView::PreCreateWindow(CREATESTRUCT& cs)
{
        ASSERT(cs.style & WS_CHILD);
        if (cs.lpszClass == NULL)
                cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS);
        return TRUE;
}

void CFahrplanView::OnActivateView(BOOL bActivate, CView* pActiveView,
        CView* pDeactiveView)
{
        CView::OnActivateView(bActivate, pActiveView, pDeactiveView);

        // invalidate selections when active status changes
        if (m_bActive != bActivate)
        {
                if (bActivate)  // if becoming active update as if active
                        m_bActive = bActivate;
                if (!m_selection.IsEmpty())
                        OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);
                m_bActive = bActivate;
        }
}

/////////////////////////////////////////////////////////////////////////////
// CFahrplanView drawing

void CFahrplanView::InvalObj(CDrawObj* pObj)
{
        CRect rect = pObj->m_position;
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

void CFahrplanView::OnUpdate(CView* , LPARAM lHint, CObject* pHint)
{
        switch (lHint)
        {
        case HINT_UPDATE_WINDOW:    // redraw entire window
                
                Invalidate(FALSE);
                break;

        case HINT_UPDATE_DRAWOBJ:   // a single object has changed
                InvalObj((CDrawObj*)pHint);
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
                        CDrawDoc* pDoc = GetDocument();
                        POSITION pos = pDoc->GetObjects()->GetHeadPosition();
                        while (pos != NULL)
                        {
                                CDrawObj* pObj = (CDrawObj*)pDoc->GetObjects()->GetNext(pos);
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

void CFahrplanView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
        CScrollView::OnPrepareDC(pDC, pInfo);

        // mapping mode is MM_ANISOTROPIC
        // these extents setup a mode similar to MM_LOENGLISH
        // MM_LOENGLISH is in .01 physical inches
        // these extents provide .01 logical inches

        pDC->SetMapMode(MM_ANISOTROPIC);
        pDC->SetViewportExt(pDC->GetDeviceCaps(LOGPIXELSX),
                pDC->GetDeviceCaps(LOGPIXELSY));
        pDC->SetWindowExt(100, -100);

        // set the origin of the coordinate system to the center of the page
        CPoint ptOrg;
        ptOrg.x = GetDocument()->GetSize().cx / 2;
        ptOrg.y = GetDocument()->GetSize().cy / 2;

        // ptOrg is in logical coordinates
        pDC->OffsetWindowOrg(-ptOrg.x,ptOrg.y);
}

BOOL CFahrplanView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll)
{
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

void CFahrplanView::OnDraw(CDC* pDC)
{
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

void CFahrplanView::Remove(CDrawObj* pObj)
{
        POSITION pos = m_selection.Find(pObj);
        if (pos != NULL)
                m_selection.RemoveAt(pos);
}

void CFahrplanView::PasteNative(COleDataObject& dataObject)
{
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

void CFahrplanView::PasteEmbedded(COleDataObject& dataObject)
{
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
                GetDocument()->Add(pObj);
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
                GetDocument()->Remove(pObj);
                delete pObj;

                AfxMessageBox(IDP_FAILED_TO_CREATE);
        }
        END_CATCH_ALL

        EndWaitCursor();
}

void CFahrplanView::DrawGrid(CDC* pDC)
{
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

void CFahrplanView::OnInitialUpdate()
{
        CView::OnInitialUpdate();
        CSize size = GetDocument()->GetSize();
        CClientDC dc(NULL);
        size.cx = MulDiv(size.cx, dc.GetDeviceCaps(LOGPIXELSX), 100);
        size.cy = MulDiv(size.cy, dc.GetDeviceCaps(LOGPIXELSY), 100);
        SetScrollSizes(MM_TEXT, size);
}

void CFahrplanView::SetPageSize(CSize size)
{
        CClientDC dc(NULL);
        size.cx = MulDiv(size.cx, dc.GetDeviceCaps(LOGPIXELSX), 100);
        size.cy = MulDiv(size.cy, dc.GetDeviceCaps(LOGPIXELSY), 100);
        SetScrollSizes(MM_TEXT, size);
        GetDocument()->UpdateAllViews(NULL, HINT_UPDATE_WINDOW, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CFahrplanView printing

BOOL CFahrplanView::OnPreparePrinting(CPrintInfo* pInfo)
{
        // default preparation
        return DoPreparePrinting(pInfo);
}

void CFahrplanView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
        CScrollView::OnBeginPrinting(pDC,pInfo);

        // check page size -- user could have gone into print setup
        // from print dialog and changed paper or orientation
        GetDocument()->ComputePageSize();
}

void CFahrplanView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
        // TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// OLE Client support and commands

BOOL CFahrplanView::IsSelected(const CObject* pDocItem) const
{
        CDrawObj* pDrawObj = (CDrawObj*)pDocItem;
        if (pDocItem->IsKindOf(RUNTIME_CLASS(CDrawItem)))
                pDrawObj = ((CDrawItem*)pDocItem)->m_pDrawObj;
        return m_selection.Find(pDrawObj) != NULL;
}

void CFahrplanView::OnInsertObject()
{
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
                GetDocument()->Add(pObj);

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
                GetDocument()->Remove(pObj);
                delete pObj;

                AfxMessageBox(IDP_FAILED_TO_CREATE);
        }
        END_CATCH_ALL

        EndWaitCursor();
}

// The following command handler provides the standard keyboard
//  user interface to cancel an in-place editing session.
void CFahrplanView::OnCancelEdit()
{
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

void CFahrplanView::OnSetFocus(CWnd* pOldWnd)
{
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

CRect CFahrplanView::GetInitialPosition()
{
        CRect rect(10, 10, 10, 10);
        ClientToDoc(rect);
        return rect;
}

void CFahrplanView::ClientToDoc(CPoint& point)
{
        CClientDC dc(this);
        OnPrepareDC(&dc, NULL);
        dc.DPtoLP(&point);
}

void CFahrplanView::ClientToDoc(CRect& rect)
{
        CClientDC dc(this);
        OnPrepareDC(&dc, NULL);
        dc.DPtoLP(rect);
        ASSERT(rect.left <= rect.right);
        ASSERT(rect.bottom <= rect.top);
}

void CFahrplanView::DocToClient(CPoint& point)
{
        CClientDC dc(this);
        OnPrepareDC(&dc, NULL);
        dc.LPtoDP(&point);
}

void CFahrplanView::DocToClient(CRect& rect)
{
        CClientDC dc(this);
        OnPrepareDC(&dc, NULL);
        dc.LPtoDP(rect);
        rect.NormalizeRect();
}

void CFahrplanView::Select(CDrawObj* pObj, BOOL bAdd)
{
        if (!bAdd)
        {
                OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);
                m_selection.RemoveAll();
        }

        if (pObj == NULL || IsSelected(pObj))
                return;

        m_selection.AddTail(pObj);
        InvalObj(pObj);
}

// rect is in device coordinates
void CFahrplanView::SelectWithinRect(CRect rect, BOOL bAdd)
{
        if (!bAdd)
                Select(NULL);

        ClientToDoc(rect);

        CObList* pObList = GetDocument()->GetObjects();
        POSITION posObj = pObList->GetHeadPosition();
        while (posObj != NULL)
        {
                CDrawObj* pObj = (CDrawObj*)pObList->GetNext(posObj);
                if (pObj->Intersects(rect))
                        Select(pObj, TRUE);
        }
}

void CFahrplanView::Deselect(CDrawObj* pObj)
{
        POSITION pos = m_selection.Find(pObj);
        if (pos != NULL)
        {
                InvalObj(pObj);
                m_selection.RemoveAt(pos);
        }
}

void CFahrplanView::CloneSelection()
{
        POSITION pos = m_selection.GetHeadPosition();
        while (pos != NULL)
        {
                CDrawObj* pObj = (CDrawObj*)m_selection.GetNext(pos);
                pObj->Clone(pObj->m_pDocument);
                        // copies object and adds it to the document
        }
}

void CFahrplanView::UpdateActiveItem()
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
// CFahrplanView message handlers

void CFahrplanView::OnLButtonDown(UINT nFlags, CPoint point)
{
        if (!m_bActive)
                return;
        CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
        if (pTool != NULL)
                pTool->OnLButtonDown(this, nFlags, point);
}

void CFahrplanView::OnLButtonUp(UINT nFlags, CPoint point)
{
        if (!m_bActive)
                return;
        CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
        if (pTool != NULL)
                pTool->OnLButtonUp(this, nFlags, point);
}

void CFahrplanView::OnMouseMove(UINT nFlags, CPoint point)
{
        if (!m_bActive)
                return;
        CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
        if (pTool != NULL)
                pTool->OnMouseMove(this, nFlags, point);
}

void CFahrplanView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
        if (!m_bActive)
                return;
        CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
        if (pTool != NULL)
                pTool->OnLButtonDblClk(this, nFlags, point);
}

void CFahrplanView::OnDestroy()
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

void CFahrplanView::OnDrawSelect()
{
        CDrawTool::c_drawShape = select;
}

void CFahrplanView::OnDrawRoundRect()
{
        CDrawTool::c_drawShape = roundRect;
}

void CFahrplanView::OnDrawRect()
{
        CDrawTool::c_drawShape = rect;
}

void CFahrplanView::OnDrawLine()
{
        CDrawTool::c_drawShape = line;
}

void CFahrplanView::OnDrawEllipse()
{
        CDrawTool::c_drawShape = ellipse;
}

void CFahrplanView::OnDrawPolygon()
{
        CDrawTool::c_drawShape = poly;
}

void CFahrplanView::OnUpdateDrawEllipse(CCmdUI* pCmdUI)
{
        pCmdUI->SetRadio(CDrawTool::c_drawShape == ellipse);
}

void CFahrplanView::OnUpdateDrawLine(CCmdUI* pCmdUI)
{
        pCmdUI->SetRadio(CDrawTool::c_drawShape == line);
}

void CFahrplanView::OnUpdateDrawRect(CCmdUI* pCmdUI)
{
        pCmdUI->SetRadio(CDrawTool::c_drawShape == rect);
}

void CFahrplanView::OnUpdateDrawRoundRect(CCmdUI* pCmdUI)
{
        pCmdUI->SetRadio(CDrawTool::c_drawShape == roundRect);
}

void CFahrplanView::OnUpdateDrawSelect(CCmdUI* pCmdUI)
{
        pCmdUI->SetRadio(CDrawTool::c_drawShape == select);
}

void CFahrplanView::OnDrawBahnhof()
{
        CDrawTool::c_drawShape = bahnhof;
}

void CFahrplanView::OnUpdateDrawBahnhof(CCmdUI* pCmdUI)
{
        pCmdUI->SetRadio(CDrawTool::c_drawShape == bahnhof);
}

void CFahrplanView::OnConnectBahnhof()
{
    while (m_selection.GetCount() != 0)
    {
        Deselect((CDrawObj*)m_selection.GetHead());
    }
    CDrawTool::c_drawShape = connect;
}

void CFahrplanView::OnUpdateConnectBahnhof(CCmdUI* pCmdUI)
{
        pCmdUI->SetRadio(CDrawTool::c_drawShape == connect);
}



void CFahrplanView::OnUpdateSingleSelect(CCmdUI* pCmdUI)
{
        pCmdUI->Enable(m_selection.GetCount() == 1);
}

void CFahrplanView::OnEditSelectAll()
{
        CObList* pObList = GetDocument()->GetObjects();
        POSITION pos = pObList->GetHeadPosition();
        
        while (pos != NULL)
                Select((CDrawObj*)pObList->GetNext(pos), TRUE);
}

void CFahrplanView::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{
        pCmdUI->Enable(GetDocument()->GetObjects()->GetCount() != 0);
}

void CFahrplanView::OnEditClear()
{
        // update all the views before the selection goes away
        GetDocument()->UpdateAllViews(NULL, HINT_DELETE_SELECTION, &m_selection);
        OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);

        // now remove the selection from the document (nur Kanten)
        POSITION pos = m_selection.GetHeadPosition();
        while (pos != NULL)
        {
                CDrawObj* pObj = (CDrawObj*)m_selection.GetNext(pos);

                if (pObj->Netz == CDrawObj::Kante)
                {
                    GetDocument()->Remove(pObj);
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
                GetDocument()->Remove(pObj);
                delete pObj;
        }


        m_selection.RemoveAll();
}

void CFahrplanView::OnUpdateAnySelect(CCmdUI* pCmdUI)
{
        pCmdUI->Enable(!m_selection.IsEmpty());
}

void CFahrplanView::OnUpdateDrawPolygon(CCmdUI* pCmdUI)
{
        pCmdUI->SetRadio(CDrawTool::c_drawShape == poly);
}

void CFahrplanView::OnSize(UINT nType, int cx, int cy)
{
        CScrollView::OnSize(nType, cx, cy);
        UpdateActiveItem();
}

void CFahrplanView::OnViewGrid()
{
        m_bGrid = !m_bGrid;
        Invalidate(FALSE);
}

void CFahrplanView::OnUpdateViewGrid(CCmdUI* pCmdUI)
{
        pCmdUI->SetCheck(m_bGrid);
}

BOOL CFahrplanView::OnEraseBkgnd(CDC* pDC)
{
        return TRUE;
}

void CFahrplanView::OnObjectFillColor()
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

void CFahrplanView::OnObjectLineColor()
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

void CFahrplanView::OnObjectMoveBack()
{
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
        }
}

void CFahrplanView::OnObjectMoveForward()
{
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
        }
}

void CFahrplanView::OnObjectMoveToBack()
{
        CDrawDoc* pDoc = GetDocument();
        CDrawObj* pObj = (CDrawObj*)m_selection.GetHead();
        CObList* pObjects = pDoc->GetObjects();
        POSITION pos = pObjects->Find(pObj);
        ASSERT(pos != NULL);
        pObjects->RemoveAt(pos);
        pObjects->AddHead(pObj);
        InvalObj(pObj);
}

void CFahrplanView::OnObjectMoveToFront()
{
        CDrawDoc* pDoc = GetDocument();
        CDrawObj* pObj = (CDrawObj*)m_selection.GetHead();
        CObList* pObjects = pDoc->GetObjects();
        POSITION pos = pObjects->Find(pObj);
        ASSERT(pos != NULL);
        pObjects->RemoveAt(pos);
        pObjects->AddTail(pObj);
        InvalObj(pObj);
}

void CFahrplanView::OnEditCopy()
{
        ASSERT_VALID(this);
        ASSERT(m_cfDraw != NULL);

        // Create a shared file and associate a CArchive with it
        CSharedFile file;
        CArchive ar(&file, CArchive::store);

        // Serialize selected objects to the archive
        m_selection.Serialize(ar);
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

void CFahrplanView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
        pCmdUI->Enable(!m_selection.IsEmpty());
}

void CFahrplanView::OnEditCut()
{
        OnEditCopy();
        OnEditClear();
}

void CFahrplanView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
        pCmdUI->Enable(!m_selection.IsEmpty());
}

void CFahrplanView::OnEditPaste()
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
                        GetDocument()->Add((CDrawObj*)m_selection.GetNext(pos));
        }
        else
                PasteEmbedded(dataObject);

        GetDocument()->SetModifiedFlag();

        // invalidate new pasted stuff
        GetDocument()->UpdateAllViews(NULL, HINT_UPDATE_SELECTION, &m_selection);
}

void CFahrplanView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
        // determine if private or standard OLE formats are on the clipboard
        COleDataObject dataObject;
        BOOL bEnable = dataObject.AttachClipboard() &&
                (dataObject.IsDataAvailable(m_cfDraw) ||
                 COleClientItem::CanCreateFromData(&dataObject));

        // enable command based on availability
        pCmdUI->Enable(bEnable);
}

void CFahrplanView::OnFilePrint()
{
        CScrollView::OnFilePrint();
        GetDocument()->ComputePageSize();
}

void CFahrplanView::OnViewShowObjects()
{
        CDrawOleObj::c_bShowItems = !CDrawOleObj::c_bShowItems;
        GetDocument()->UpdateAllViews(NULL, HINT_UPDATE_OLE_ITEMS, NULL);
}

void CFahrplanView::OnUpdateViewShowObjects(CCmdUI* pCmdUI)
{
        pCmdUI->SetCheck(CDrawOleObj::c_bShowItems);
}

void CFahrplanView::OnEditProperties()
{
        if (m_selection.GetCount() == 1 && CDrawTool::c_drawShape == select)
        {
                CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
                ASSERT(pTool != NULL);
                pTool->OnLButtonDblClk(this, 0, CPoint(0, 0));
        }
}

void CFahrplanView::OnUpdateEditProperties(CCmdUI* pCmdUI)
{
        pCmdUI->Enable(m_selection.GetCount() == 1 &&
                CDrawTool::c_drawShape == select);
}

/////////////////////////////////////////////////////////////////////////////
// CFahrplanView diagnostics

#ifdef _DEBUG
void CFahrplanView::AssertValid() const
{
        CScrollView::AssertValid();
}

void CFahrplanView::Dump(CDumpContext& dc) const
{
        CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

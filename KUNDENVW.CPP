// drawvw.cpp : implementation of the CKundenView class
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

#include "train.h"
#include "pageserv.h"
#include "drawdoc.h"
#include "drawobj.h"
#include "tabobj.h"
#include "cntritem.h"
#include "basevw.h"
#include "pagevw.h"
#include "kundenvw.h"

#include "drawtool.h"
#include "persist.h"
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKundenView

IMPLEMENT_DYNCREATE(CKundenView, CPageView)


BEGIN_MESSAGE_MAP(CKundenView, CPageView)
        ON_COMMAND(ID_NEU_KUNDE, OnNeuKunde)
        ON_UPDATE_COMMAND_UI(ID_NEU_KUNDE, OnUpdateNeuKunde)
        ON_COMMAND(ID_DELETE_KUNDE, OnDeleteKunde)
        ON_UPDATE_COMMAND_UI(ID_DELETE_KUNDE, OnUpdateDeleteKunde)
        
/*        ON_COMMAND(ID_SEITE_UP, OnKundeUp)
        ON_UPDATE_COMMAND_UI(ID_SEITE_UP, OnUpdateKundeUp)
        ON_COMMAND(ID_SEITE_DN, OnKundeDn)
        ON_UPDATE_COMMAND_UI(ID_SEITE_DN, OnUpdateKundeDn)
        ON_COMMAND(ID_SEITE_HOME, OnKundeHome)
        ON_UPDATE_COMMAND_UI(ID_SEITE_HOME, OnUpdateKundeHome)
        ON_COMMAND(ID_SEITE_END, OnKundeEnd)
        ON_UPDATE_COMMAND_UI(ID_SEITE_END, OnUpdateKundeEnd)
*/
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKundenView construction/destruction

CKundenView::CKundenView()
{
        m_bGrid = TRUE;
        m_gridColor = RGB(128, 0, 0);
        m_bActive = FALSE;
}

CKundenView::~CKundenView()
{
}

BOOL CKundenView::PreCreateWindow(CREATESTRUCT& cs)
{
     return CBaseView::PreCreateWindow(cs);
}

void CKundenView::OnActivateView(BOOL bActivate, CView* pActiveView,
        CView* pDeactiveView)
{
    if (m_bActive != bActivate)
    {
        CMainFrame* V = (CMainFrame*) AfxGetMainWnd();

        if (V != NULL) V->OnViewBar(IDW_KUNDEN);
    }
    
    CBaseView::OnActivateView(bActivate, pActiveView, pDeactiveView);
}




void CKundenView::OnNeuKunde()
{
    CDrawTool::c_drawShape = makenewkunde;
}
void CKundenView::OnUpdateNeuKunde(CCmdUI* pCmdUI)
{
    pCmdUI->SetRadio(CDrawTool::c_drawShape == makenewkunde);
}
void CKundenView::OnDeleteKunde()
{
    CDrawTool::c_drawShape = deletekunde;
}
void CKundenView::OnUpdateDeleteKunde(CCmdUI* pCmdUI)
{
    pCmdUI->SetRadio(CDrawTool::c_drawShape == deletekunde);
}
/*
void CKundenView::OnKundeUp()
{
    int i = GetDocument()->GetActualKunden();
    GetDocument()->SetActualKunden(i + 1);
}
void CKundenView::OnUpdateKundeUp(CCmdUI* pCmdUI)
{
    int i = GetDocument()->GetKundenPages()->GetCount();
    pCmdUI->Enable((i - 1 > GetDocument()->GetActualKunden()) && (i != 0));
}
void CKundenView::OnKundeDn()
{
    int i = GetDocument()->GetActualKunden();
    GetDocument()->SetActualKunden(i - 1);
}
void CKundenView::OnUpdateKundeDn(CCmdUI* pCmdUI)
{
    int i = GetDocument()->GetActualKunden();
    pCmdUI->Enable(i > 0);
}
void CKundenView::OnKundeHome()
{
    GetDocument()->SetActualKunden(0);
}
void CKundenView::OnUpdateKundeHome(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(GetDocument()->GetActualKunden() > 0);
}
void CKundenView::OnKundeEnd()
{
    int i = GetDocument()->GetKundenPages()->GetCount();
    GetDocument()->SetActualKunden(i - 1);
}
void CKundenView::OnUpdateKundeEnd(CCmdUI* pCmdUI)
{
    int i = GetDocument()->GetKundenPages()->GetCount();
    pCmdUI->Enable(GetDocument()->GetActualKunden() < i - 1);
}

*/

/////////////////////////////////////////////////////////////////////////////
// CKundenView drawing



CObList* CKundenView::GetDocObjects()
{
    return GetPage();
}


void CKundenView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
        CBaseView::OnPrepareDC(pDC, pInfo);
}

BOOL CKundenView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll)
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

void CKundenView::OnInitialUpdate()
{
    SetupPageServer("Kunden");
    CBaseView::OnInitialUpdate();
}

void CKundenView::SetPageSize(CSize size)
{
        CClientDC dc(NULL);
        size.cx = MulDiv(size.cx, dc.GetDeviceCaps(LOGPIXELSX), 100);
        size.cy = MulDiv(size.cy, dc.GetDeviceCaps(LOGPIXELSY), 100);
        SetScrollSizes(MM_TEXT, size);
        GetDocument()->UpdateAllViews(NULL, HINT_UPDATE_WINDOW, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CKundenView printing

BOOL CKundenView::OnPreparePrinting(CPrintInfo* pInfo)
{
        // default preparation
        return DoPreparePrinting(pInfo);
}

void CKundenView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
        CScrollView::OnBeginPrinting(pDC,pInfo);

        // check page size -- user could have gone into print setup
        // from print dialog and changed paper or orientation
        GetDocument()->ComputePageSize();
}

void CKundenView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
        // TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// OLE Client support and commands

BOOL CKundenView::IsSelected(const CObject* pDocItem) const
{
        CDrawObj* pDrawObj = (CDrawObj*)pDocItem;
        if (pDocItem->IsKindOf(RUNTIME_CLASS(CDrawItem)))
                pDrawObj = ((CDrawItem*)pDocItem)->m_pDrawObj;
        return m_selection.Find(pDrawObj) != NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CKundenView diagnostics

#ifdef _DEBUG
void CKundenView::AssertValid() const
{
        CScrollView::AssertValid();
}

void CKundenView::Dump(CDumpContext& dc) const
{
        CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

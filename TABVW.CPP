// drawvw.cpp : implementation of the CTabView class
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

// Die CTabView stellt einen Fahrplan aus einer Liste von Fahrplähnen
// dar, welcher auch einen Rückfahrt - Fahrplan haben kann.
// Weiter kann mann noch Beschreibungen hinzufügen etc.

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
#include "tabvw.h"

#include "drawtool.h"
#include "persist.h"
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabView

IMPLEMENT_DYNCREATE(CTabView, CPageView)


BEGIN_MESSAGE_MAP(CTabView, CPageView)
        ON_COMMAND(ID_DRAW_TABELLE, OnDrawTabelle)
        ON_UPDATE_COMMAND_UI(ID_DRAW_TABELLE, OnUpdateDrawTabelle)

        ON_COMMAND(ID_ADD_FAHRT, OnAddFahrt)
        ON_UPDATE_COMMAND_UI(ID_ADD_FAHRT, OnUpdateAddFahrt)
        ON_COMMAND(ID_ADD_REVERS_FAHRPLAN, OnAddReversFahrplan)
        ON_UPDATE_COMMAND_UI(ID_ADD_REVERS_FAHRPLAN, OnUpdateAddReversFahrplan)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabView construction/destruction

CTabView::CTabView()
{
        m_bGrid = TRUE;
        m_gridColor = RGB(128, 0, 0);
        m_bActive = FALSE;
}

CTabView::~CTabView()
{
}

BOOL CTabView::PreCreateWindow(CREATESTRUCT& cs)
{
     return CBaseView::PreCreateWindow(cs);
}

void CTabView::OnActivateView(BOOL bActivate, CView* pActiveView,
        CView* pDeactiveView)
{
    if (m_bActive != bActivate)
    {
        CMainFrame* V = (CMainFrame*) AfxGetMainWnd();

        if (V != NULL) V->OnViewBar(IDW_FAHRPLAN);
    }
    
    CBaseView::OnActivateView(bActivate, pActiveView, pDeactiveView);
}



void CTabView::OnAddReversFahrplan()
{
}

void CTabView::OnUpdateAddReversFahrplan(CCmdUI* pCmdUI)
{
}

void CTabView::OnAddFahrt()
{
}

void CTabView::OnUpdateAddFahrt(CCmdUI* pCmdUI)
{
}

void CTabView::OnDrawTabelle()
{
        CDrawTool::c_drawShape = tabelle;
}

void CTabView::OnUpdateDrawTabelle(CCmdUI* pCmdUI)
{
        pCmdUI->SetRadio(CDrawTool::c_drawShape == tabelle);
}


/////////////////////////////////////////////////////////////////////////////
// CTabView drawing



CObList* CTabView::GetDocObjects()
{
    return GetPage();
}


void CTabView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
        CBaseView::OnPrepareDC(pDC, pInfo);
}

BOOL CTabView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll)
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

void CTabView::OnInitialUpdate()
{
    SetupPageServer("Tabellenansicht");
    CBaseView::OnInitialUpdate();
}

void CTabView::SetPageSize(CSize size)
{
        CClientDC dc(NULL);
        size.cx = MulDiv(size.cx, dc.GetDeviceCaps(LOGPIXELSX), 100);
        size.cy = MulDiv(size.cy, dc.GetDeviceCaps(LOGPIXELSY), 100);
        SetScrollSizes(MM_TEXT, size);
        GetDocument()->UpdateAllViews(NULL, HINT_UPDATE_WINDOW, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CTabView printing

BOOL CTabView::OnPreparePrinting(CPrintInfo* pInfo)
{
        // default preparation
        return DoPreparePrinting(pInfo);
}

void CTabView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
        CScrollView::OnBeginPrinting(pDC,pInfo);

        // check page size -- user could have gone into print setup
        // from print dialog and changed paper or orientation
        GetDocument()->ComputePageSize();
}

void CTabView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
        // TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// OLE Client support and commands

BOOL CTabView::IsSelected(const CObject* pDocItem) const
{
        CDrawObj* pDrawObj = (CDrawObj*)pDocItem;
        if (pDocItem->IsKindOf(RUNTIME_CLASS(CDrawItem)))
                pDrawObj = ((CDrawItem*)pDocItem)->m_pDrawObj;
        return m_selection.Find(pDrawObj) != NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CTabView diagnostics

#ifdef _DEBUG
void CTabView::AssertValid() const
{
        CScrollView::AssertValid();
}

void CTabView::Dump(CDumpContext& dc) const
{
        CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

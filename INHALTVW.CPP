// drawvw.cpp : implementation of the CInhaltView class
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
#include "cntritem.h"
#include "basevw.h"
#include "pagevw.h"
#include "inhaltvw.h"

#include "drawtool.h"
#include "persist.h"
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// private clipboard format (list of Draw objects)
/////////////////////////////////////////////////////////////////////////////
// CInhaltView

IMPLEMENT_DYNCREATE(CInhaltView, CPageView)

BEGIN_MESSAGE_MAP(CInhaltView, CPageView)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInhaltView construction/destruction

CInhaltView::CInhaltView()
{
        m_bGrid = TRUE;
        m_gridColor = RGB(0, 0, 0);
        m_bActive = FALSE;
}

CInhaltView::~CInhaltView()
{
}

BOOL CInhaltView::PreCreateWindow(CREATESTRUCT& cs)
{
     return CBaseView::PreCreateWindow(cs);
}

void CInhaltView::OnActivateView(BOOL bActivate, CView* pActiveView,
        CView* pDeactiveView)
{
    if (m_bActive != bActivate)
    {
        CMainFrame* V = (CMainFrame*) AfxGetMainWnd();

        if (V != NULL) V->OnViewBar(IDW_BENDEF);
    }
    
    CBaseView::OnActivateView(bActivate, pActiveView, pDeactiveView);
}
/////////////////////////////////////////////////////////////////////////////
// CInhaltView drawing


CObList* CInhaltView::GetDocObjects()
{
    return GetPage();
}


void CInhaltView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
        CBaseView::OnPrepareDC(pDC, pInfo);
}

void CInhaltView::OnInitialUpdate()
{
    SetupPageServer("Inhaltsverzeichnis");
    CBaseView::OnInitialUpdate();
}


/////////////////////////////////////////////////////////////////////////////
// CInhaltView printing

BOOL CInhaltView::OnPreparePrinting(CPrintInfo* pInfo)
{
        // default preparation
        return DoPreparePrinting(pInfo);
}

void CInhaltView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
        CScrollView::OnBeginPrinting(pDC,pInfo);

        // check page size -- user could have gone into print setup
        // from print dialog and changed paper or orientation
        GetDocument()->ComputePageSize();
}

void CInhaltView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
        // TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// OLE Client support and commands

BOOL CInhaltView::IsSelected(const CObject* pDocItem) const
{
        CDrawObj* pDrawObj = (CDrawObj*)pDocItem;
        if (pDocItem->IsKindOf(RUNTIME_CLASS(CDrawItem)))
                pDrawObj = ((CDrawItem*)pDocItem)->m_pDrawObj;
        return m_selection.Find(pDrawObj) != NULL;
}



/////////////////////////////////////////////////////////////////////////////
// CInhaltView message handlers


/////////////////////////////////////////////////////////////////////////////
// CInhaltView diagnostics

#ifdef _DEBUG
void CInhaltView::AssertValid() const
{
        CScrollView::AssertValid();
}

void CInhaltView::Dump(CDumpContext& dc) const
{
        CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

// drawvw.cpp : implementation of the CWagenView class
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
#include "wagenvw.h"

#include "drawtool.h"
#include "persist.h"
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWagenView

IMPLEMENT_DYNCREATE(CWagenView, CPageView)

BEGIN_MESSAGE_MAP(CWagenView, CPageView)
        // Netz - Eigenschaften: 

        ON_COMMAND(ID_DRAW_ADD_WAGEN, OnDrawAddWagen)
        ON_UPDATE_COMMAND_UI(ID_DRAW_ADD_WAGEN, OnUpdateDrawAddWagen)        

        ON_COMMAND(ID_DRAW_ADD_SITZGRUPPE, OnDrawAddSitzgruppe)
        ON_UPDATE_COMMAND_UI(ID_DRAW_ADD_SITZGRUPPE, OnUpdateDrawAddSitzgruppe)

        ON_COMMAND(ID_DRAW_ADD_SITZ, OnDrawAddSitz)
        ON_UPDATE_COMMAND_UI(ID_DRAW_ADD_SITZ, OnUpdateDrawAddSitz)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWagenView construction/destruction

CWagenView::CWagenView()
{
        m_bGrid = TRUE;
        m_gridColor = RGB(0, 0, 128);
        m_bActive = FALSE;
}

CWagenView::~CWagenView()
{
}

BOOL CWagenView::PreCreateWindow(CREATESTRUCT& cs)
{
     return CBaseView::PreCreateWindow(cs);
}

void CWagenView::OnActivateView(BOOL bActivate, CView* pActiveView,
        CView* pDeactiveView)
{
    if (m_bActive != bActivate)
    {
        CMainFrame* V = (CMainFrame*) AfxGetMainWnd();

        if (V != NULL) V->OnViewBar(IDW_EDIT);
    }
    
    CBaseView::OnActivateView(bActivate, pActiveView, pDeactiveView);
}
/////////////////////////////////////////////////////////////////////////////
// CWagenView drawing


CObList* CWagenView::GetDocObjects()
{
    return GetPage();
}


void CWagenView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
        CBaseView::OnPrepareDC(pDC, pInfo);
}

void CWagenView::OnInitialUpdate()
{
    SetupPageServer("Streckennetz");
    CBaseView::OnInitialUpdate();
}


/////////////////////////////////////////////////////////////////////////////
// CWagenView printing

BOOL CWagenView::OnPreparePrinting(CPrintInfo* pInfo)
{
        // default preparation
        return DoPreparePrinting(pInfo);
}

void CWagenView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
        CScrollView::OnBeginPrinting(pDC,pInfo);

        // check page size -- user could have gone into print setup
        // from print dialog and changed paper or orientation
        GetDocument()->ComputePageSize();
}

void CWagenView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
        // TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// OLE Client support and commands

BOOL CWagenView::IsSelected(const CObject* pDocItem) const
{
        CDrawObj* pDrawObj = (CDrawObj*)pDocItem;
        if (pDocItem->IsKindOf(RUNTIME_CLASS(CDrawItem)))
                pDrawObj = ((CDrawItem*)pDocItem)->m_pDrawObj;
        return m_selection.Find(pDrawObj) != NULL;
}



/////////////////////////////////////////////////////////////////////////////
// CWagenView message handlers

void CWagenView::OnDrawAddWagen()
{
//    if (m_selection.GetCount() == 1)
        CDrawTool::c_drawShape = addwagen;
}

void CWagenView::OnUpdateDrawAddWagen(CCmdUI* pCmdUI)
{
    pCmdUI->SetRadio(CDrawTool::c_drawShape == addwagen);    
}

void CWagenView::OnDrawAddSitzgruppe()
{
//    if (m_selection.GetCount() == 1)
        CDrawTool::c_drawShape = addsitzgruppe;
}

void CWagenView::OnUpdateDrawAddSitzgruppe(CCmdUI* pCmdUI)
{
    pCmdUI->SetRadio(CDrawTool::c_drawShape == addsitzgruppe);    
}

void CWagenView::OnDrawAddSitz()
{
//    if (m_selection.GetCount() == 1)
        CDrawTool::c_drawShape = addsitz;
}

void CWagenView::OnUpdateDrawAddSitz(CCmdUI* pCmdUI)
{
    pCmdUI->SetRadio(CDrawTool::c_drawShape == addsitz);    
}

/////////////////////////////////////////////////////////////////////////////
// CWagenView diagnostics

#ifdef _DEBUG
void CWagenView::AssertValid() const
{
        CScrollView::AssertValid();
}

void CWagenView::Dump(CDumpContext& dc) const
{
        CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

// drawvw.cpp : implementation of the CNetUse class
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

#include "drawdoc.h"
#include "drawobj.h"
#include "cntritem.h"
#include "basevw.h"
#include "mapview.h"
#include "net_use.h"

#include "drawtool.h"
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNetUse

IMPLEMENT_DYNCREATE(CNetUse, CBaseView)

BEGIN_MESSAGE_MAP(CNetUse, CBaseView)
        ON_COMMAND(ID_GET_FAHRKARTE, OnChooseFahrkarte)
        ON_UPDATE_COMMAND_UI(ID_GET_FAHRKARTE, OnUpdateChooseFahrkarte)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetUse construction/destruction

CNetUse::CNetUse()
{
        m_bGrid = TRUE;
        m_gridColor = RGB(0, 0, 128);
        m_bActive = FALSE;
}

CNetUse::~CNetUse()
{
}

BOOL CNetUse::PreCreateWindow(CREATESTRUCT& cs)
{
     return CBaseView::PreCreateWindow(cs);
}

void CNetUse::OnActivateView(BOOL bActivate, CView* pActiveView,
        CView* pDeactiveView)
{
    if (m_bActive != bActivate)
    {
        CMainFrame* V = (CMainFrame*) AfxGetMainWnd();

        if (V != NULL) V->OnViewBar(IDW_USE);
    }
    
    CBaseView::OnActivateView(bActivate, pActiveView, pDeactiveView);
}
/////////////////////////////////////////////////////////////////////////////
// CNetUse drawing


CObList* CNetUse::GetDocObjects()
{
    return GetDocument()->GetNetObjects();
}


void CNetUse::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
        CBaseView::OnPrepareDC(pDC, pInfo);
}

void CNetUse::OnInitialUpdate()
{
    CBaseView::OnInitialUpdate();
}


/////////////////////////////////////////////////////////////////////////////
// CNetUse printing

BOOL CNetUse::OnPreparePrinting(CPrintInfo* pInfo)
{
        // default preparation
        return DoPreparePrinting(pInfo);
}

void CNetUse::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
        CScrollView::OnBeginPrinting(pDC,pInfo);

        // check page size -- user could have gone into print setup
        // from print dialog and changed paper or orientation
        GetDocument()->ComputePageSize();
}

void CNetUse::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
        // TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// OLE Client support and commands

BOOL CNetUse::IsSelected(const CObject* pDocItem) const
{
        CDrawObj* pDrawObj = (CDrawObj*)pDocItem;
        if (pDocItem->IsKindOf(RUNTIME_CLASS(CDrawItem)))
                pDrawObj = ((CDrawItem*)pDocItem)->m_pDrawObj;
        return m_selection.Find(pDrawObj) != NULL;
}

void CNetUse::OnMouseMove(UINT nFlags, CPoint point)
{
   if (!m_bActive)
        return;
}


/////////////////////////////////////////////////////////////////////////////
// CNetUse message handlers

void CNetUse::OnChooseFahrkarte()
{
        CDrawTool::c_drawShape = choosefahrkarte;
}

void CNetUse::OnUpdateChooseFahrkarte(CCmdUI* pCmdUI)
{
        pCmdUI->SetRadio(CDrawTool::c_drawShape == choosefahrkarte);
}





/////////////////////////////////////////////////////////////////////////////
// CNetUse diagnostics

#ifdef _DEBUG
void CNetUse::AssertValid() const
{
        CScrollView::AssertValid();
}

void CNetUse::Dump(CDumpContext& dc) const
{
        CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

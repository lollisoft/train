// drawvw.cpp : implementation of the CNetEd class
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
#include "net_ed.h"

#include "drawtool.h"
#include "persist.h"
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNetEd

IMPLEMENT_DYNCREATE(CNetEd, CPageView)

BEGIN_MESSAGE_MAP(CNetEd, CPageView)
        // Netz - Eigenschaften: 
        ON_COMMAND(ID_DRAW_BAHNHOF, OnDrawBahnhof)
        ON_COMMAND(ID_CONNECT_BAHNHOF, OnConnectBahnhof)

        ON_COMMAND(ID_DRAW_TEXT, OnDrawText)
        ON_COMMAND(ID_ADD_DATA_OBJ, OnAddDataObj)

        ON_COMMAND(ID_DRAW_ADD_ELEMENT, OnDrawAddElement)
        ON_UPDATE_COMMAND_UI(ID_DRAW_ADD_ELEMENT, OnUpdateDrawAddElement)        

        ON_COMMAND(ID_DRAW_MAKE_FAHRPLAN, OnMakeFahrplan)
        ON_UPDATE_COMMAND_UI(ID_DRAW_MAKE_FAHRPLAN, OnUpdateMakeFahrplan)

        ON_UPDATE_COMMAND_UI(ID_DRAW_BAHNHOF, OnUpdateDrawBahnhof)
        ON_UPDATE_COMMAND_UI(ID_CONNECT_BAHNHOF, OnUpdateConnectBahnhof)
        ON_UPDATE_COMMAND_UI(ID_DRAW_TEXT, OnUpdateDrawText)
        ON_UPDATE_COMMAND_UI(ID_ADD_DATA_OBJ, OnUpdateAddDataObj)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetEd construction/destruction

CNetEd::CNetEd()
{
        m_bGrid = TRUE;
        m_gridColor = RGB(0, 0, 128);
        m_bActive = FALSE;
}

CNetEd::~CNetEd()
{
}

BOOL CNetEd::PreCreateWindow(CREATESTRUCT& cs)
{
     return CBaseView::PreCreateWindow(cs);
}

void CNetEd::OnActivateView(BOOL bActivate, CView* pActiveView,
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
// CNetEd drawing


CObList* CNetEd::GetDocObjects()
{
    return GetPage();
}


void CNetEd::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
        CBaseView::OnPrepareDC(pDC, pInfo);
}

void CNetEd::OnInitialUpdate()
{
    SetupPageServer("Streckennetz");
    CBaseView::OnInitialUpdate();
}


/////////////////////////////////////////////////////////////////////////////
// CNetEd printing

BOOL CNetEd::OnPreparePrinting(CPrintInfo* pInfo)
{
        // default preparation
        return DoPreparePrinting(pInfo);
}

void CNetEd::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
        CScrollView::OnBeginPrinting(pDC,pInfo);

        // check page size -- user could have gone into print setup
        // from print dialog and changed paper or orientation
        GetDocument()->ComputePageSize();
}

void CNetEd::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
        // TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// OLE Client support and commands

BOOL CNetEd::IsSelected(const CObject* pDocItem) const
{
        CDrawObj* pDrawObj = (CDrawObj*)pDocItem;
        if (pDocItem->IsKindOf(RUNTIME_CLASS(CDrawItem)))
                pDrawObj = ((CDrawItem*)pDocItem)->m_pDrawObj;
        return m_selection.Find(pDrawObj) != NULL;
}



/////////////////////////////////////////////////////////////////////////////
// CNetEd message handlers

void CNetEd::OnDrawText()
{
        CDrawTool::c_drawShape = text;
}

void CNetEd::OnUpdateDrawText(CCmdUI* pCmdUI)
{
        pCmdUI->SetRadio(CDrawTool::c_drawShape == text);
}

void CNetEd::OnAddDataObj()
{
        CDrawTool::c_drawShape = adddata;
}

void CNetEd::OnUpdateAddDataObj(CCmdUI* pCmdUI)
{
        pCmdUI->SetRadio(CDrawTool::c_drawShape == adddata);
}


void CNetEd::OnDrawBahnhof()
{
        CDrawTool::c_drawShape = bahnhof;
}

void CNetEd::OnUpdateDrawBahnhof(CCmdUI* pCmdUI)
{
        pCmdUI->SetRadio(CDrawTool::c_drawShape == bahnhof);
}

void CNetEd::OnConnectBahnhof()
{
    while (m_selection.GetCount() != 0)
    {
        Deselect((CDrawObj*)m_selection.GetHead());
    }
    CDrawTool::c_drawShape = connect;
}

void CNetEd::OnUpdateConnectBahnhof(CCmdUI* pCmdUI)
{
        pCmdUI->SetRadio(CDrawTool::c_drawShape == connect);
}

void CNetEd::OnDrawAddElement()
{
//    if (m_selection.GetCount() == 1)
        CDrawTool::c_drawShape = addconnection;
}

void CNetEd::OnUpdateDrawAddElement(CCmdUI* pCmdUI)
{
    pCmdUI->SetRadio(CDrawTool::c_drawShape == addconnection);    
}

void CNetEd::OnMakeFahrplan()
{
    CDrawTool::c_drawShape = makefahrplan;
}

void CNetEd::OnUpdateMakeFahrplan(CCmdUI* pCmdUI)
{
    pCmdUI->SetRadio(CDrawTool::c_drawShape == makefahrplan);    
}


/////////////////////////////////////////////////////////////////////////////
// CNetEd diagnostics

#ifdef _DEBUG
void CNetEd::AssertValid() const
{
        CScrollView::AssertValid();
}

void CNetEd::Dump(CDumpContext& dc) const
{
        CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

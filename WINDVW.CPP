// drawvw.cpp : implementation of the CWndView class
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
#include "windvw.h" // Server für CWnd

#include "drawtool.h"
#include "persist.h"
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWndView

IMPLEMENT_DYNCREATE(CWndView, CBaseView)

BEGIN_MESSAGE_MAP(CWndView, CBaseView)

        // Window
        ON_COMMAND(ID_CREATE_WINDOW, OnCreateWindow)
        ON_UPDATE_COMMAND_UI(ID_CREATE_WINDOW, OnUpdateCreateWindow)
        // Netz - Eigenschaften: 
        ON_COMMAND(ID_DRAW_BAHNHOF, OnDrawBahnhof)
        ON_COMMAND(ID_CONNECT_BAHNHOF, OnConnectBahnhof)

        ON_COMMAND(ID_DRAW_TEXT, OnDrawText)
        ON_COMMAND(ID_ADD_DATA_OBJ, OnAddDataObj)

        ON_COMMAND(ID_DRAW_ADD_ELEMENT, OnDrawAddElement)
        ON_UPDATE_COMMAND_UI(ID_DRAW_ADD_ELEMENT, OnUpdateDrawAddElement)        

        ON_UPDATE_COMMAND_UI(ID_DRAW_BAHNHOF, OnUpdateDrawBahnhof)
        ON_UPDATE_COMMAND_UI(ID_CONNECT_BAHNHOF, OnUpdateConnectBahnhof)
        ON_UPDATE_COMMAND_UI(ID_DRAW_TEXT, OnUpdateDrawText)
        ON_UPDATE_COMMAND_UI(ID_ADD_DATA_OBJ, OnUpdateAddDataObj)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWndView construction/destruction

CWndView::CWndView()
{
        m_bGrid = TRUE;
        m_gridColor = RGB(0, 0, 128);
        m_bActive = FALSE;
}

CWndView::~CWndView()
{
}

BOOL CWndView::PreCreateWindow(CREATESTRUCT& cs)
{
     return CBaseView::PreCreateWindow(cs);
}

void CWndView::OnActivateView(BOOL bActivate, CView* pActiveView,
        CView* pDeactiveView)
{
    if (m_bActive != bActivate)
    {
        CMainFrame* V = (CMainFrame*) AfxGetMainWnd();

        if (V != NULL) V->OnViewBar(IDW_WINDOW);
    }
    
    CBaseView::OnActivateView(bActivate, pActiveView, pDeactiveView);
}
/////////////////////////////////////////////////////////////////////////////
// CWndView drawing


CObList* CWndView::GetDocObjects()
{
    return GetPage();
}


void CWndView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
        CBaseView::OnPrepareDC(pDC, pInfo);
}

void CWndView::OnInitialUpdate()
{
    SetupPageServer("Dialogdesigner");
    CBaseView::OnInitialUpdate();
}


/////////////////////////////////////////////////////////////////////////////
// CWndView printing

BOOL CWndView::OnPreparePrinting(CPrintInfo* pInfo)
{
        // default preparation
        return DoPreparePrinting(pInfo);
}

void CWndView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
        CScrollView::OnBeginPrinting(pDC,pInfo);

        // check page size -- user could have gone into print setup
        // from print dialog and changed paper or orientation
        GetDocument()->ComputePageSize();
}

void CWndView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
        // TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// OLE Client support and commands

BOOL CWndView::IsSelected(const CObject* pDocItem) const
{
        CDrawObj* pDrawObj = (CDrawObj*)pDocItem;
        if (pDocItem->IsKindOf(RUNTIME_CLASS(CDrawItem)))
                pDrawObj = ((CDrawItem*)pDocItem)->m_pDrawObj;
        return m_selection.Find(pDrawObj) != NULL;
}



/////////////////////////////////////////////////////////////////////////////
// CWndView message handlers

void CWndView::OnDrawText()
{
        CDrawTool::c_drawShape = text;
}

void CWndView::OnUpdateDrawText(CCmdUI* pCmdUI)
{
        pCmdUI->SetRadio(CDrawTool::c_drawShape == text);
}

void CWndView::OnCreateWindow()
{
        CDrawTool::c_drawShape = window;
}

void CWndView::OnUpdateCreateWindow(CCmdUI* pCmdUI)
{
        pCmdUI->SetRadio(CDrawTool::c_drawShape == window);
}


void CWndView::OnAddDataObj()
{
        CDrawTool::c_drawShape = adddata;
}

void CWndView::OnUpdateAddDataObj(CCmdUI* pCmdUI)
{
        pCmdUI->SetRadio(CDrawTool::c_drawShape == adddata);
}


void CWndView::OnDrawBahnhof()
{
        CDrawTool::c_drawShape = bahnhof;
}

void CWndView::OnUpdateDrawBahnhof(CCmdUI* pCmdUI)
{
        pCmdUI->SetRadio(CDrawTool::c_drawShape == bahnhof);
}

void CWndView::OnConnectBahnhof()
{
    while (m_selection.GetCount() != 0)
    {
        Deselect((CDrawObj*)m_selection.GetHead());
    }
    CDrawTool::c_drawShape = connect;
}

void CWndView::OnUpdateConnectBahnhof(CCmdUI* pCmdUI)
{
        pCmdUI->SetRadio(CDrawTool::c_drawShape == connect);
}

void CWndView::OnDrawAddElement()
{
    if (m_selection.GetCount() == 1)
        CDrawTool::c_drawShape = addconnection;
}

void CWndView::OnUpdateDrawAddElement(CCmdUI* pCmdUI)
{
    pCmdUI->SetRadio(CDrawTool::c_drawShape == addconnection);    
}



/////////////////////////////////////////////////////////////////////////////
// CWndView diagnostics

#ifdef _DEBUG
void CWndView::AssertValid() const
{
        CScrollView::AssertValid();
}

void CWndView::Dump(CDumpContext& dc) const
{
        CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

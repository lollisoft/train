// drawvw.cpp : implementation of the CBenDefView class
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
#include "bendefvw.h"

#include "drawtool.h"
#include "persist.h"
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// private clipboard format (list of Draw objects)
/////////////////////////////////////////////////////////////////////////////
// CBenDefView

IMPLEMENT_DYNCREATE(CBenDefView, CPageView)

BEGIN_MESSAGE_MAP(CBenDefView, CPageView)
    ON_COMMAND(ID_DRAW_NEU_ELEMENT, OnDrawNeuElement)
    ON_COMMAND(ID_DRAW_ADD_ELEMENT, OnDrawAddElement)
    ON_COMMAND(ID_DRAW_ATTRIBUTE, OnDrawAttribute)
    ON_COMMAND(ID_ADD_BENDEFOBJ, OnAddNewBenDefObj)
    
    ON_UPDATE_COMMAND_UI(ID_ADD_BENDEFOBJ, OnUpdateAddNewBenDefObj)
    ON_UPDATE_COMMAND_UI(ID_DRAW_NEU_ELEMENT, OnUpdateDrawNeuElement)
    ON_UPDATE_COMMAND_UI(ID_DRAW_ADD_ELEMENT, OnUpdateDrawAddElement)
    ON_UPDATE_COMMAND_UI(ID_DRAW_ATTRIBUTE, OnUpdateDrawAttribute)

        ON_COMMAND(ID_DRAW_ROUNDRECT, OnDrawRoundRect)
        ON_COMMAND(ID_DRAW_RECT, OnDrawRect)
        ON_COMMAND(ID_DRAW_LINE, OnDrawLine)
        ON_COMMAND(ID_DRAW_CIRCLE, OnDrawCircle)
        ON_COMMAND(ID_DRAW_POLYGON, OnDrawPolygon)
        ON_UPDATE_COMMAND_UI(ID_DRAW_POLYGON, OnUpdateDrawPolygon)
        ON_UPDATE_COMMAND_UI(ID_DRAW_CIRCLE, OnUpdateDrawCircle)
        ON_UPDATE_COMMAND_UI(ID_DRAW_LINE, OnUpdateDrawLine)
        ON_UPDATE_COMMAND_UI(ID_DRAW_RECT, OnUpdateDrawRect)
        ON_UPDATE_COMMAND_UI(ID_DRAW_ROUNDRECT, OnUpdateDrawRoundRect)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBenDefView construction/destruction

CBenDefView::CBenDefView()
{
        m_bGrid = TRUE;
        m_gridColor = RGB(0, 0, 0);
        m_bActive = FALSE;
}

CBenDefView::~CBenDefView()
{
}

BOOL CBenDefView::PreCreateWindow(CREATESTRUCT& cs)
{
     return CBaseView::PreCreateWindow(cs);
}

void CBenDefView::OnActivateView(BOOL bActivate, CView* pActiveView,
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
// CBenDefView drawing


CObList* CBenDefView::GetDocObjects()
{
    //return GetDocument()->...(GetActualPage());
    return GetPage();
}


void CBenDefView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
        CBaseView::OnPrepareDC(pDC, pInfo);
}

void CBenDefView::OnInitialUpdate()
{
    SetupPageServer("Benutzerdefinierte Objekte");
    CBaseView::OnInitialUpdate();
}


/////////////////////////////////////////////////////////////////////////////
// CBenDefView printing

BOOL CBenDefView::OnPreparePrinting(CPrintInfo* pInfo)
{
        // default preparation
        return DoPreparePrinting(pInfo);
}

void CBenDefView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
        CScrollView::OnBeginPrinting(pDC,pInfo);

        // check page size -- user could have gone into print setup
        // from print dialog and changed paper or orientation
        GetDocument()->ComputePageSize();
}

void CBenDefView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
        // TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// OLE Client support and commands

BOOL CBenDefView::IsSelected(const CObject* pDocItem) const
{
        CDrawObj* pDrawObj = (CDrawObj*)pDocItem;
        if (pDocItem->IsKindOf(RUNTIME_CLASS(CDrawItem)))
                pDrawObj = ((CDrawItem*)pDocItem)->m_pDrawObj;
        return m_selection.Find(pDrawObj) != NULL;
}



/////////////////////////////////////////////////////////////////////////////
// CBenDefView message handlers

void CBenDefView::OnAddNewBenDefObj() // z. B. Neuer Bahnhof´s Typ
{
    CDrawTool::c_drawShape = addnewbendefobj;
}

void CBenDefView::OnDrawNeuElement()
{
    CDrawTool::c_drawShape = bendef;
}

void CBenDefView::OnDrawAddElement()
{
    if (m_selection.GetCount() == 1)
        CDrawTool::c_drawShape = addconnection;
}

void CBenDefView::OnDrawAttribute()
{
}

void CBenDefView::OnUpdateAddNewBenDefObj(CCmdUI* pCmdUI)
{
    pCmdUI->SetRadio(CDrawTool::c_drawShape == addnewbendefobj);
}


void CBenDefView::OnUpdateDrawNeuElement(CCmdUI* pCmdUI)
{
    pCmdUI->SetRadio(CDrawTool::c_drawShape == bendef);
}

void CBenDefView::OnUpdateDrawAddElement(CCmdUI* pCmdUI)
{
    pCmdUI->SetRadio(CDrawTool::c_drawShape == addconnection);    
}

void CBenDefView::OnUpdateDrawAttribute(CCmdUI* pCmdUI)
{
}

void CBenDefView::OnDrawRoundRect()
{
        CDrawTool::c_drawShape = roundRect;
}

void CBenDefView::OnDrawRect()
{
        CDrawTool::c_drawShape = rect;
}

void CBenDefView::OnDrawLine()
{
        CDrawTool::c_drawShape = line;
}

void CBenDefView::OnDrawCircle()
{
        CDrawTool::c_drawShape = ellipse;
}

void CBenDefView::OnDrawPolygon()
{
        CDrawTool::c_drawShape = poly;
}

void CBenDefView::OnUpdateDrawCircle(CCmdUI* pCmdUI)
{
        pCmdUI->SetRadio(CDrawTool::c_drawShape == ellipse);
}

void CBenDefView::OnUpdateDrawLine(CCmdUI* pCmdUI)
{
        pCmdUI->SetRadio(CDrawTool::c_drawShape == line);
}

void CBenDefView::OnUpdateDrawRect(CCmdUI* pCmdUI)
{
        pCmdUI->SetRadio(CDrawTool::c_drawShape == rect);
}

void CBenDefView::OnUpdateDrawRoundRect(CCmdUI* pCmdUI)
{
        pCmdUI->SetRadio(CDrawTool::c_drawShape == roundRect);
}

void CBenDefView::OnUpdateDrawPolygon(CCmdUI* pCmdUI)
{
        pCmdUI->SetRadio(CDrawTool::c_drawShape == poly);
}


/////////////////////////////////////////////////////////////////////////////
// CBenDefView diagnostics

#ifdef _DEBUG
void CBenDefView::AssertValid() const
{
        CScrollView::AssertValid();
}

void CBenDefView::Dump(CDumpContext& dc) const
{
        CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

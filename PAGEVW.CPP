// drawvw.cpp : implementation of the CPageView class
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

#include "drawtool.h"
#include "persist.h"
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageView

IMPLEMENT_DYNCREATE(CPageView, CBaseView)


BEGIN_MESSAGE_MAP(CPageView, CBaseView)
        ON_COMMAND(ID_NEU_SEITE, OnNeuSeite)
        ON_UPDATE_COMMAND_UI(ID_NEU_SEITE, OnUpdateNeuSeite)
        ON_COMMAND(ID_DELETE_SEITE, OnDeleteSeite)
        ON_UPDATE_COMMAND_UI(ID_DELETE_SEITE, OnUpdateDeleteSeite)
        
        ON_COMMAND(ID_SEITE_UP, OnSeiteUp)
        ON_UPDATE_COMMAND_UI(ID_SEITE_UP, OnUpdateSeiteUp)
        ON_COMMAND(ID_SEITE_DN, OnSeiteDn)
        ON_UPDATE_COMMAND_UI(ID_SEITE_DN, OnUpdateSeiteDn)
        ON_COMMAND(ID_SEITE_HOME, OnSeiteHome)
        ON_UPDATE_COMMAND_UI(ID_SEITE_HOME, OnUpdateSeiteHome)
        ON_COMMAND(ID_SEITE_END, OnSeiteEnd)
        ON_UPDATE_COMMAND_UI(ID_SEITE_END, OnUpdateSeiteEnd)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageView construction/destruction

CPageView::CPageView()
{
        m_bGrid = TRUE;
        m_gridColor = RGB(128, 0, 0);
        m_bActive = FALSE;
}

CPageView::~CPageView()
{
}

BOOL CPageView::PreCreateWindow(CREATESTRUCT& cs)
{
     return CBaseView::PreCreateWindow(cs);
}

void CPageView::OnActivateView(BOOL bActivate, CView* pActiveView,
        CView* pDeactiveView)
{
    if (m_bActive != bActivate)
    {
        CMainFrame* V = (CMainFrame*) AfxGetMainWnd();

        // Hier k”nnte eine Toolbar speziell zur Seite gew„hlt werden
        if (V != NULL) V->OnViewBar(IDW_KUNDEN);
    }
    
    CBaseView::OnActivateView(bActivate, pActiveView, pDeactiveView);
}

CObList* CPageView::GetPage()
{
    return GetPageServer()->GetPage(GetActualPage());
}

CString CPageView::GetActualPfad()
{
    return m_ViewPfad +
            m_ActualPfad;
}

CString CPageView::GetActualPage()
{
    return m_ViewPfad +
            m_ActualPfad +
            m_ActualPage;
}

CPageServer* CPageView::GetPageServer()
{
    GetDocument()->GetPageServer()->SetPage(m_ActualPfad);
    return GetDocument()->GetPageServer();
}

void CPageView::ShowPageOverview()
{
    // Wechselt zur Inhaltsseite und l”scht den alten Graf.
    // Zeichnet den Neuen.
    GetDocument()->GetPageServer()->ShowOverview(m_ActualPage);
}

void CPageView::OnNeuSeite()
{
    GetDocument()->AddNewPage(this);
    GetDocument()->MakeChangesRegardingMe(this);
    Invalidate(FALSE);
}
void CPageView::OnUpdateNeuSeite(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(GetDocument()->IsAnyPageLeft(this));
}
void CPageView::OnDeleteSeite()
{
    GetDocument()->DeletePage(this);
    GetDocument()->MakeChangesRegardingMe(this);
    Invalidate(FALSE);
}
void CPageView::OnUpdateDeleteSeite(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(GetDocument()->ExsistAnyPage(this));
}

void CPageView::OnSeiteUp()
{
    GetDocument()->GetNextPage(this);
    GetDocument()->MakeChangesRegardingMe(this);
    Invalidate(FALSE);
}
void CPageView::OnUpdateSeiteUp(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(GetDocument()->IsNextPageAvailable(this));
}
void CPageView::OnSeiteDn()
{
    GetDocument()->GetPrevPage(this);
    GetDocument()->MakeChangesRegardingMe(this);
    Invalidate(FALSE);
}
void CPageView::OnUpdateSeiteDn(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(GetDocument()->IsPrevPageAvailable(this));
}
void CPageView::OnSeiteHome()
{
    GetDocument()->GetFirstPage(this);
    GetDocument()->MakeChangesRegardingMe(this);
    Invalidate(FALSE);
}
void CPageView::OnUpdateSeiteHome(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(GetDocument()->IsPrevPageAvailable(this));
}
void CPageView::OnSeiteEnd()
{
    GetDocument()->GetLastPage(this);
    GetDocument()->MakeChangesRegardingMe(this);
    Invalidate(FALSE);
}
void CPageView::OnUpdateSeiteEnd(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(GetDocument()->IsNextPageAvailable(this));
}



/////////////////////////////////////////////////////////////////////////////
// CPageView drawing



CObList* CPageView::GetDocObjects()
{
    return GetPage();
}


void CPageView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
        CBaseView::OnPrepareDC(pDC, pInfo);
}

BOOL CPageView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll)
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

void CPageView::SetViewPfad(CString Pfad)
{
    m_ViewPfad = Pfad;
}

void CPageView::SetThisViewName(CString Name)
{
    m_ThisViewName = Name;
}

void CPageView::SetActualPage(CString Page)
{
    m_ActualPage = Page;
}

void CPageView::SetActualPfad(CString Pfad)
{
    m_ActualPfad = Pfad;
}



void CPageView::SetupPageServer(CString Name)
{
    CPageServer* pServer = GetDocument()->GetPageServer();

    CString ViewName = pServer->CheckForAName(Name); // Darf keine Directoryspezifischen Zeichen enthalten.

    if (ViewName.IsEmpty())
    {
        AfxMessageBox("SetupPageServer..Name ist nicht korrekt!");
        ASSERT(FALSE);
    }
    SetThisViewName(ViewName);
    

    // Die View muß wissen, wo Ihre Daten sind. D.h.,
    // Sie speichert den Pfad zu Ihren Daten.
    //
    // Dieser Pfad wird später von Objekten in der View erfragt.

    SetViewPfad(CString("/Views")); // Datenbereich für Views
    
    // Übergabeparameter "this" für Aktualisierungen (Achtung: Pointer!)
    // InitDirToView muß eine Seite einrichten, damit später gleich
    // mit der aktuellen Seite gearbeitet werden kann.

    
    if (!pServer->ExsistAnyPage(GetViewPfad()))
        pServer->InitDirToView(this, GetThisViewName());

  
    SetActualPfad("/" + GetThisViewName());
    
}

void CPageView::OnInitialUpdate()
{
    CBaseView::OnInitialUpdate();
}

void CPageView::SetPageSize(CSize size)
{
        CClientDC dc(NULL);
        size.cx = MulDiv(size.cx, dc.GetDeviceCaps(LOGPIXELSX), 100);
        size.cy = MulDiv(size.cy, dc.GetDeviceCaps(LOGPIXELSY), 100);
        SetScrollSizes(MM_TEXT, size);
        GetDocument()->UpdateAllViews(NULL, HINT_UPDATE_WINDOW, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CPageView printing

BOOL CPageView::OnPreparePrinting(CPrintInfo* pInfo)
{
        // default preparation
        return DoPreparePrinting(pInfo);
}

void CPageView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
        CScrollView::OnBeginPrinting(pDC,pInfo);

        // check page size -- user could have gone into print setup
        // from print dialog and changed paper or orientation
        GetDocument()->ComputePageSize();
}

void CPageView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
        // TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// OLE Client support and commands

/*BOOL CPageView::IsSelected(const CObject* pDocItem) const
{
        CDrawObj* pDrawObj = (CDrawObj*)pDocItem;
        if (pDocItem->IsKindOf(RUNTIME_CLASS(CDrawItem)))
                pDrawObj = ((CDrawItem*)pDocItem)->m_pDrawObj;
        return m_selection.Find(pDrawObj) != NULL;
}*/

/////////////////////////////////////////////////////////////////////////////
// CPageView diagnostics

#ifdef _DEBUG
void CPageView::AssertValid() const
{
        CScrollView::AssertValid();
}

void CPageView::Dump(CDumpContext& dc) const
{
        CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

// ordview.cpp : implementation of the CMainView class
//

#include "stdafx.h"
#include "train.h"
#include "pageserv.h"
#include "Drawdoc.h"
#include "Basevw.h"
#include "mainview.h"
//
//include children tab views here for each tab type
//
//#include "Net_Use.h"
#include "ordervw.h"
#include "editvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define BASE    CMapView
#undef THIS
#define THIS    CMainView
/////////////////////////////////////////////////////////////////////////////
// CMainView

IMPLEMENT_DYNCREATE(CMainView, CMapView)

BEGIN_MESSAGE_MAP(CMainView, CMapView)
        //{{AFX_MSG_MAP(CMainView)
        //}}AFX_MSG_MAP
        // Standard printing commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainView construction/destruction

THIS::THIS()
{
        // TODO: add construction code here
}

THIS::~THIS()
{
}


/////////////////////////////////////////////////////////////////////////////
// CMainView diagnostics

#ifdef _DEBUG
void THIS::AssertValid() const
{
        BASE::AssertValid();
}

void THIS::Dump(CDumpContext& dc) const
{
        CView::Dump(dc);
}

CDrawDoc* THIS::GetDocument() // non-debug version is inline
{
        ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDrawDoc)));
        return (CDrawDoc*) m_pDocument;
}

#endif //_DEBUG
void THIS::OnInitialUpdate()
{
        COrderView* pOrderView = (COrderView*) addTabView(RUNTIME_CLASS(COrderView),GetDocument(),
                "&Options");
        ASSERT(pOrderView != NULL);
        pOrderView->m_pMapView = this;

/*        CNetUse* pDrawView = (CNetUse*) addTabView(RUNTIME_CLASS(CNetUse),GetDocument(),
                "&Netz",TRUE,FALSE);
        ASSERT(pDrawView != NULL);
        pDrawView->m_pMapView = this;
*/
        CEditVw* pEditView = (CEditVw*) addTabView(RUNTIME_CLASS(CEditVw),GetDocument(),
                "&Edit",TRUE,FALSE);
        ASSERT(pEditView != NULL);
        pEditView->m_pMapView = this;

        BASE::OnInitialUpdate();
}


// editvw.cpp : implementation of the CEditVw class
//

#include "stdafx.h"
#include "train.h"
#include "pageserv.h"
#include "drawdoc.h"
#include "mapview.h"
#include "editvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditVw
#define BASE CEditView
#undef THIS
#define THIS CEditVw

IMPLEMENT_DYNCREATE(CEditVw,CEditView)

BEGIN_MESSAGE_MAP(CEditVw,CEditView)
        //{{AFX_MSG_MAP(CEditVw)
        ON_WM_SYSCOMMAND()
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditVw construction/destruction

THIS::THIS()
{
}

THIS::~THIS()
{
}

#ifdef _DEBUG
CDrawDoc* THIS::GetDocument() // non-debug version is inline
{
        ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDrawDoc)));
        return (CDrawDoc*) m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEditVw message handlers
void THIS::OnSysCommand(UINT nChar,LONG lParam)
{
        if(m_pMapView->doSysCommand(nChar,lParam))
                return;
        BASE::OnSysCommand(nChar,lParam);
}
BOOL THIS::PreCreateWindow(CREATESTRUCT& cs)
{
        BOOL temp = BASE::PreCreateWindow(cs);
        cs.style =      cs.style |
        WS_HSCROLL | WS_VSCROLL |
        ES_AUTOHSCROLL | ES_AUTOVSCROLL |
        ES_MULTILINE | ES_NOHIDESEL;
        return temp;
}

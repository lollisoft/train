// ordervw.cpp : implementation file
//

#include "stdafx.h"
#include "train.h"
#include "ctl3d.h"
#include "ordervw.h"
#include "resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COrderView
#define BASE    CFormView
#undef THIS
#define THIS    COrderView
IMPLEMENT_DYNCREATE(COrderView, CFormView)

THIS::THIS()
        : CFormView(COrderView::IDD)
{
        //{{AFX_DATA_INIT(COrderView)
        m_tabHeight = 25;
        m_tabMargin = 7;
        m_enable = TRUE;
        //}}AFX_DATA_INIT
}

THIS::~THIS()
{
}

void THIS::DoDataExchange(CDataExchange* pDX)
{
        CFormView::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(COrderView)
        DDX_Control(pDX, IDD_TAB_STYLE, m_tabStyle);
        DDX_Control(pDX, IDD_TAB_LOOK, m_tabLook);
        DDX_Text(pDX, IDC_TAB_HEIGHT, m_tabHeight);
        DDV_MinMaxUInt(pDX, m_tabHeight, 5, 100);
        DDX_Text(pDX, IDC_TAB_MARGIN, m_tabMargin);
        DDV_MinMaxUInt(pDX, m_tabMargin, 2, 100);
        DDX_Check(pDX, IDC_ENABLE, m_enable);
        //}}AFX_DATA_MAP
}

void THIS::OnInitialUpdate()
{
        BASE::OnInitialUpdate();

        Ctl3dSubclassDlg (m_hWnd, CTL3D_ALL);
        m_tabLook.SetCurSel(0);
        m_tabStyle.SetCurSel(0);
}
BEGIN_MESSAGE_MAP(COrderView, CFormView)
        //{{AFX_MSG_MAP(COrderView)
        ON_COMMAND(ID_HELP_ORDERS,OnTestOrder)
        ON_WM_SYSCOMMAND()
        ON_CBN_SELCHANGE(IDD_TAB_LOOK, OnSelchangeTabLook)
        ON_CBN_SELCHANGE(IDD_TAB_STYLE, OnSelchangeTabStyle)
        ON_WM_SIZE()
        ON_BN_CLICKED(ID_APPLY, OnApply)
        ON_WM_KILLFOCUS()
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COrderView message handlers

void THIS::OnTestOrder()
{
        AfxMessageBox("Handled in OrderView");
}

void THIS::OnSysCommand(UINT nChar,LONG lParam)
{
        if(m_pMapView->doSysCommand(nChar,lParam))
                return;
        BASE::OnSysCommand(nChar,lParam);
}

void THIS::OnSelchangeTabLook()
{
        int sel = m_tabLook.GetCurSel();
        m_pMapView->setLAF((sel == 0) ? LAF_MSWORD:LAF_CHICAGO);
}

void THIS::OnSelchangeTabStyle()
{
        int sel = m_tabStyle.GetCurSel();
        switch(sel)
        {
                case 0:
                        m_pMapView->setTabPosition(TABSONTOP);
                        break;
                case 1:
                        m_pMapView->setTabPosition(TABSONLEFT);
                        break;
                case 2:
                        m_pMapView->setTabPosition(TABSONLEFTBOT);
                        break;
                case 3:
                        m_pMapView->setTabPosition(TABSONRIGHT);
                        break;
                case 4:
                        m_pMapView->setTabPosition(TABSONRIGHTBOT);
                        break;
        }
}

void THIS::OnSize(UINT nType, int cx, int cy)
{
        CFormView::OnSize(nType, cx, cy);

        SetSize(ID_FRAME,cx-10,cy-10);  
//      setPosition(ID_APPLY,x,y);
//      setPosition(ID_UPDATE,x,y);
        
}
void THIS::SetPosition(UINT nID,int x,int y)
{
        CWnd* wnd = GetDlgItem(nID);
        if(!wnd)
                return;
        wnd->SetWindowPos(NULL,x,y,0,0,SWP_NOZORDER|SWP_NOSIZE);
}
void THIS::SetSize(UINT nID,int width,int height)
{
        CWnd* wnd = GetDlgItem(nID);
        if(!wnd)
                return;
        wnd->SetWindowPos(NULL,0,0,width,height,SWP_NOZORDER|SWP_NOMOVE);
}
CRect THIS::GetRect(UINT nID)
{
        CRect rect;
        CWnd* wnd = GetDlgItem(nID);
        wnd->GetWindowRect(&rect);
        wnd->ScreenToClient(&rect);
//      CPoint pt = rect;
        return rect;
}
void THIS::OnApply()
{
        UpdateData(TRUE);
        m_pMapView->setMargin(m_tabMargin);
        m_pMapView->setTabHeight(m_tabHeight);
        m_pMapView->enableView(1,m_enable);
}

void COrderView::OnKillFocus(CWnd* pNewWnd)
{
        CFormView::OnKillFocus(pNewWnd);

}

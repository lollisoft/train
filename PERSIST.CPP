#include "stdafx.h"
#include "persist.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////
// CMDIPersistentFrameWnd

const CRect NEAR CMDIPersistentFrameWnd::rectDefault(10, 10, 800, 600);  // static

IMPLEMENT_DYNAMIC(CMDIPersistentFrameWnd, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMDIPersistentFrameWnd, CMDIFrameWnd)
    //{{AFX_MSG_MAP(CMDIPersistentFrameWnd)
    ON_WM_DESTROY()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////
CMDIPersistentFrameWnd::CMDIPersistentFrameWnd() :
    m_profileHeading("Window size"), m_profileRect("Rect"),
    m_profileIcon("icon"), m_profileMax("max"),
    m_profileTool("tool"), m_profileStatus("status")
{
    m_bFirstTime = TRUE;

}

/////////////////////////////////////////////////////////////////////
CMDIPersistentFrameWnd::~CMDIPersistentFrameWnd()
{
}

/////////////////////////////////////////////////////////////////////
void CMDIPersistentFrameWnd::OnDestroy()
{
    CString text, temp;
    CWnd*   pBar;
    BOOL    bIconic, bMaximized;

    WINDOWPLACEMENT wndpl;
    wndpl.length = sizeof(WINDOWPLACEMENT);
    // gets current window position and iconized/maximized status
    BOOL bRet = GetWindowPlacement(&wndpl);
    if (wndpl.showCmd == SW_SHOWNORMAL) {
      bIconic = FALSE;
      bMaximized = FALSE;
    }
    else if (wndpl.showCmd == SW_SHOWMAXIMIZED) {
      bIconic = FALSE;
      bMaximized = TRUE;
    } 
    else if (wndpl.showCmd == SW_SHOWMINIMIZED) {
      bIconic = TRUE;
      if (wndpl.flags) {
        bMaximized = TRUE;
      }
      else {
        bMaximized = FALSE;
      }
    }
    wsprintf(text.GetBuffer(20), "%04d %04d %04d %04d",
        wndpl.rcNormalPosition.left, wndpl.rcNormalPosition.top,
        wndpl.rcNormalPosition.right, wndpl.rcNormalPosition.bottom);

    text.ReleaseBuffer();
    AfxGetApp()->WriteProfileString(m_profileHeading,
                                    m_profileRect, text);
    AfxGetApp()->WriteProfileInt(m_profileHeading,
                                 m_profileIcon, bIconic);
    AfxGetApp()->WriteProfileInt(m_profileHeading,
                                 m_profileMax, bMaximized);
    if (pBar = GetDescendantWindow(AFX_IDW_TOOLBAR)) {
        AfxGetApp()->WriteProfileInt(m_profileHeading, m_profileTool,
            (pBar->GetStyle() & WS_VISIBLE) != 0L);
    }
    if (pBar = GetDescendantWindow(AFX_IDW_STATUS_BAR)) {
        AfxGetApp()->WriteProfileInt(m_profileHeading,
             m_profileStatus, (pBar->GetStyle() & WS_VISIBLE) != 0L);
    }
    CFrameWnd::OnDestroy();
}

LPCSTR CMDIPersistentFrameWnd::GetIconWndClass(DWORD dwDefaultStyle, UINT nIDResource)
{
        ASSERT_VALID_IDR(nIDResource);

/*        HINSTANCE hInst = AfxFindResourceHandle(
                MAKEINTRESOURCE(nIDResource), RT_GROUP_ICON);
        HICON hIcon = ::LoadIcon(hInst, MAKEINTRESOURCE(nIDResource));*/

        HICON hIcon = AfxGetApp()->LoadIcon(nIDResource);
        
        if (hIcon != NULL)
        {
                CREATESTRUCT cs;
                memset(&cs, 0, sizeof(CREATESTRUCT));
                cs.style = dwDefaultStyle;
                PreCreateWindow(cs);
                        // will fill lpszClassName with default WNDCLASS name
                        // ignore instance handle from PreCreateWindow.

                WNDCLASS wndcls;
                if (cs.lpszClass != NULL &&
                        GetClassInfo(AfxGetInstanceHandle(), cs.lpszClass, &wndcls) &&
                        wndcls.hIcon != hIcon)
                {
                        // register a very similar WNDCLASS
                        CString S = AfxRegisterWndClass(wndcls.style,
                                    wndcls.hCursor, wndcls.hbrBackground, hIcon);
                        return NULL;
                }
        }
        return NULL;        // just use the default
}


BOOL CMDIPersistentFrameWnd::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle,
        CWnd* pParentWnd, CCreateContext* pContext)
{
        // only do this once
        ASSERT_VALID_IDR(nIDResource);
        ASSERT(m_nIDHelp == 0 || m_nIDHelp == nIDResource);

        m_nIDHelp = nIDResource;    // ID for help context (+HID_BASE_RESOURCE)

        CString strFullString;
        if (strFullString.LoadString(nIDResource))
                AfxExtractSubString(m_strTitle, strFullString, 0);    // first sub-string

        if (!Create(GetIconWndClass(dwDefaultStyle, nIDResource),
          m_strTitle, dwDefaultStyle, rectDefault,
          pParentWnd, MAKEINTRESOURCE(nIDResource), 0L, pContext))
                return FALSE;   // will self destruct on failure normally

        // save the default menu handle
        ASSERT(m_hWnd != NULL);
        m_hMenuDefault = ::GetMenu(m_hWnd);

        // load accelerator resource
        LoadAccelTable(MAKEINTRESOURCE(nIDResource));

        if (pContext == NULL)   // send initial update
                SendMessageToDescendants(WM_INITIALUPDATE, 0, 0, TRUE, TRUE);

        return TRUE;
}


/////////////////////////////////////////////////////////////////////
void CMDIPersistentFrameWnd::ActivateFrame(int nCmdShow)
{
    CWnd*           pBar;
    CString         text;
    BOOL            bIconic, bMaximized, bTool, bStatus;
    UINT            flags;
    WINDOWPLACEMENT wndpl;
    CRect           rect;
 
    if (m_bFirstTime) {
      m_bFirstTime = FALSE;
      text = AfxGetApp()->GetProfileString(m_profileHeading,
                                           m_profileRect);
      if (!text.IsEmpty()) {
        // can't use sscanf in a DLL
        rect.left = atoi((const char*) text);
        rect.top = atoi((const char*) text + 5);
        rect.right = atoi((const char*) text + 10);
          rect.bottom = atoi((const char*) text + 15);
      }
      else {
        rect = rectDefault;
      }
    
      bIconic = AfxGetApp()->GetProfileInt(m_profileHeading,
                                           m_profileIcon, 0);
      bMaximized = AfxGetApp()->GetProfileInt(m_profileHeading,
                                              m_profileMax, 0);
    
      if (bIconic) {
        nCmdShow = SW_SHOWMINNOACTIVE;
        if (bMaximized) {
          flags = WPF_RESTORETOMAXIMIZED;
        }
      }
      else {
        if (bMaximized) {
          nCmdShow = SW_SHOWMAXIMIZED;
          flags = WPF_RESTORETOMAXIMIZED;
        }
        else {
          nCmdShow = SW_NORMAL;
          flags = 0;
        }
      }
      wndpl.length = sizeof(WINDOWPLACEMENT);
      wndpl.showCmd = nCmdShow;
      wndpl.flags = flags;
      wndpl.ptMinPosition = CPoint(0, 0);
      wndpl.ptMaxPosition = CPoint(-::GetSystemMetrics(SM_CXBORDER),
                                   -::GetSystemMetrics(SM_CYBORDER));
      wndpl.rcNormalPosition = rect;

      bTool = AfxGetApp()->GetProfileInt(m_profileHeading,
                                         m_profileTool, 1);
      if (pBar = GetDescendantWindow(AFX_IDW_TOOLBAR)) {
          pBar->ShowWindow(bTool);
      }
      bStatus = AfxGetApp()->GetProfileInt(m_profileHeading,
                                           m_profileStatus, 1);
      if (pBar = GetDescendantWindow(AFX_IDW_STATUS_BAR)) {
          pBar->ShowWindow(bStatus);
      }
      // sets window's position and iconized/maximized status
      BOOL bRet = SetWindowPlacement(&wndpl);
    }
    CFrameWnd::ActivateFrame(nCmdShow);
}


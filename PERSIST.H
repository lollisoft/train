// persist.h : CPersistentView class declaration
//

class CMDIPersistentFrameWnd : public CMDIFrameWnd
{ // remembers where it was on the desktop
    DECLARE_DYNAMIC(CMDIPersistentFrameWnd)
private:
    static const CRect NEAR rectDefault;
    CString m_profileHeading, m_profileRect, m_profileIcon;
    CString m_profileMax, m_profileTool, m_profileStatus;
    BOOL    m_bFirstTime;

public:
    BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW|FWS_ADDTOTITLE,
                  CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

protected: // create from serialization only

    LPCSTR GetIconWndClass(DWORD dwDefaultStyle, UINT nIDResource);


    CMDIPersistentFrameWnd();
    ~CMDIPersistentFrameWnd();

    virtual void ActivateFrame(int nCmdShow = -1);
    //{{AFX_MSG(CPersistentFrame)
    afx_msg void OnDestroy();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

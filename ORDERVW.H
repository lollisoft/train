// ordervw.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COrderView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#ifndef TABVIEW_H
#include "mapview.h"
#endif

class COrderView : public CFormView
{
        DECLARE_DYNCREATE(COrderView)
protected:
        COrderView();                   // protected constructor used by dynamic creation

// Form Data
public:
        //{{AFX_DATA(COrderView)
        enum { IDD = IDD_ORDERS };
        CComboBox       m_tabStyle;
        CComboBox       m_tabLook;
        UINT    m_tabHeight;
        UINT    m_tabMargin;
        BOOL    m_enable;
        //}}AFX_DATA
        CMapView* m_pMapView;

// Attributes
public:

// Operations
public:

// Implementation
protected:
        virtual ~COrderView();
        virtual void OnInitialUpdate();
        virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support
        // Generated message map functions
        //{{AFX_MSG(COrderView)
        afx_msg void OnTestOrder();
        afx_msg void OnSysCommand(UINT nChar,LONG lParam);
        afx_msg void OnSelchangeTabLook();
        afx_msg void OnSelchangeTabStyle();
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void OnApply();
        afx_msg void OnKillFocus(CWnd* pNewWnd);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
        void SetPosition(UINT nID,int x,int y);
        void SetSize(UINT nID,int width,int height);
        CRect GetRect(UINT nID);
};

/////////////////////////////////////////////////////////////////////////////

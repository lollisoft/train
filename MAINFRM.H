// mainfrm.h : interface of the CMainFrame class
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

#include "c:\c_prj\tooltip\tooltip.h"

class CNetEd;
//class CNetUse;

class CBenDefBar : public CToolTipBar
{
public:
        CComboBox   m_comboBox;
        CFont       m_font;
};

class CKundenBar : public CToolTipBar
{
public:
        CComboBox   m_comboBox;
        CFont       m_font;
};

class CMainToolBar : public CToolTipBar
{
public:
        CComboBox   m_comboBox;
        CFont       m_font;
};

class AFX_CLASS_EXPORT CMainFrame : public CMDIPersistentFrameWnd
{

        DECLARE_DYNAMIC(CMainFrame)
public:
        CMainFrame();

// Attributes
public:

// Operations
public:

// Implementation
public:
        virtual ~CMainFrame();
#ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
#endif

protected:
        // control bar embedded members
        CStatusBar  m_wndStatusBar;
        CMainToolBar    m_wndToolBar;
        CDialogBar  m_wndDlgBar;
        CToolTipBar    m_wndNetzEditBar;
        CBenDefBar  m_wndBenDefBar;
        CToolTipBar    m_wndWindowBar;
        CToolTipBar    m_wndFahrplanBar;
        CKundenBar  m_wndKundenBar;

public:

        BOOL OnSwitchTabs(UINT nID);


        afx_msg BOOL OnViewBar(UINT nID);

        void CreateOrActivateFrame(CDocTemplate* pTemplate, CRuntimeClass* pViewClass);
        CView* FindView(CRuntimeClass* pViewClass);
        BOOL CreateBenDefBar();
        BOOL CreateKundenBar();
        BOOL CreateToolBar();

// Generated message map functions
protected:
        //{{AFX_MSG(CMainFrame)
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        afx_msg void OnSysColorChange();        
        afx_msg void OnViewNetzEdit();
//        afx_msg void OnViewNetzUse();

                // NOTE - the ClassWizard will add and remove member functions here.
                //    DO NOT EDIT what you see in these blocks of generated code!
        //}}AFX_MSG



        DECLARE_MESSAGE_MAP()
};


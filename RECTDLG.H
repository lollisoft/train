// rectdlg.h : header file
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


/////////////////////////////////////////////////////////////////////////////
// CRectDlg dialog

class AFX_CLASS_EXPORT CRectDlg : public CDialog
{
// Construction
public:
        CRectDlg(CWnd* pParent = NULL); // standard constructor

// Dialog Data
        //{{AFX_DATA(CRectDlg)
        enum { IDD = IDD_PROP_RECT };
        BOOL    m_bNoFill;
        UINT    m_penSize;
        //}}AFX_DATA

// Implementation
protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

        // Generated message map functions
        //{{AFX_MSG(CRectDlg)
                // NOTE: the ClassWizard will add member functions here
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CZOOMDlg dialog

class AFX_CLASS_EXPORT CZoomDlg : public CDialog
{
// Construction
public:
        CZoomDlg(CWnd* pParent = NULL); // standard constructor

// Dialog Data
        //{{AFX_DATA(CRectDlg)
        enum { IDD = IDD_PROP_ZOOM };
        BOOL    m_bHundertProzent;
        UINT    m_Prozent;
        //}}AFX_DATA

// Implementation
protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

        // Generated message map functions
        //{{AFX_MSG(CRectDlg)
                // NOTE: the ClassWizard will add member functions here
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};


class AFX_CLASS_EXPORT CTextDlg : public CDialog
{
// Construction
public:
        CTextDlg(CWnd* pParent = NULL); // standard constructor

// Dialog Data
        //{{AFX_DATA(CRectDlg)
        enum { IDD = IDD_PROP_TEXT };
        BOOL    m_bNoFill;
        UINT    m_penSize;
        CString m_Text;
        LOGFONT m_Font, m_StandartFont;
        int     m_Angle;
        COLORREF m_FontColor;
        //}}AFX_DATA

// Implementation
protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

        // Generated message map functions
        //{{AFX_MSG(CRectDlg)
                // NOTE: the ClassWizard will add member functions here

        afx_msg void OnSetFont();
        afx_msg void OnSetStandartFont();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

// rectdlg.cpp : implementation file
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
#include "train.h"
#include "rectdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRectDlg dialog

CRectDlg::CRectDlg(CWnd* pParent /*=NULL*/)
        : CDialog(CRectDlg::IDD, pParent)
{
        //{{AFX_DATA_INIT(CRectDlg)
        m_bNoFill = FALSE;
        m_penSize = 0;
        //}}AFX_DATA_INIT
}

void CRectDlg::DoDataExchange(CDataExchange* pDX)
{
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CRectDlg)
        DDX_Check(pDX, IDC_NOFILL, m_bNoFill);
        DDX_Text(pDX, IDC_WEIGHT, m_penSize);
        DDV_MinMaxUInt(pDX, m_penSize, 0, 32767);
        //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRectDlg, CDialog)
        //{{AFX_MSG_MAP(CRectDlg)
                // NOTE: the ClassWizard will add message map macros here
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZoomDlg dialog

CZoomDlg::CZoomDlg(CWnd* pParent /*=NULL*/)
        : CDialog(CZoomDlg::IDD, pParent)
{
        //{{AFX_DATA_INIT(CRectDlg)
        m_bHundertProzent = FALSE;
        m_Prozent = 100;
        //}}AFX_DATA_INIT
}

void CZoomDlg::DoDataExchange(CDataExchange* pDX)
{
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CRectDlg)
        DDX_Check(pDX, IDC_HUNDERTPROZENT, m_bHundertProzent);
        DDX_Text(pDX, IDC_PROZENT, m_Prozent);
        DDV_MinMaxUInt(pDX, m_Prozent, 10, 1000);
        //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CZoomDlg, CDialog)
        //{{AFX_MSG_MAP(CRectDlg)
                // NOTE: the ClassWizard will add message map macros here
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()


CTextDlg::CTextDlg(CWnd* pParent /*=NULL*/)
        : CDialog(CTextDlg::IDD, pParent)
{
        //{{AFX_DATA_INIT(CTextDlg)
        m_bNoFill = FALSE;
        m_penSize = 0;
        m_Text = CString("");
        //}}AFX_DATA_INIT
}

void CTextDlg::DoDataExchange(CDataExchange* pDX)
{
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CTextDlg)
        DDX_Check(pDX, IDC_NOFILL, m_bNoFill);
        DDX_Text(pDX, IDC_WEIGHT, m_penSize);
        DDX_Text(pDX, IDC_WINKEL, m_Angle);
        DDX_Text(pDX, IDC_TEXT, m_Text);
        DDV_MinMaxUInt(pDX, m_penSize, 0, 32767);
        DDV_MinMaxUInt(pDX, m_Angle, 0, 3600);
        //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTextDlg, CDialog)
        //{{AFX_MSG_MAP(CTextDlg)
                // NOTE: the ClassWizard will add message map macros here

        ON_COMMAND(ID_SET_FONT, OnSetFont)
        ON_COMMAND(ID_SET_STANDART_FONT, OnSetStandartFont)
                
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRectDlg message handlers

void CTextDlg::OnSetFont()
{
    CFontDialog Dlg = CFontDialog(&m_Font);

    Dlg.DoModal();
    m_FontColor = Dlg.GetColor();
}

void CTextDlg::OnSetStandartFont()
{
    m_StandartFont = m_Font;
}

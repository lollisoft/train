// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp and/or WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.


#include "stdafx.h"
#include "train.h"
#include "str_inp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStringDlg dialog

CStringDlg::CStringDlg(CWnd* pParent /*=NULL*/)
        : CDialog(CStringDlg::IDD, pParent)
{
        //{{AFX_DATA_INIT(CStringDlg)
        m_String = "";
        //}}AFX_DATA_INIT
}

void CStringDlg::DoDataExchange(CDataExchange* pDX)
{
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CStringDlg)
        DDX_Text(pDX, IDC_EDITSTRING, m_String);
        //}}AFX_DATA_MAP
}

void CStringDlg::SetStringBeschreibung(CString S)
{
    GetDlgItem(IDC_STRINGBESCHREIBUNG)->SetWindowText(S);
}

BEGIN_MESSAGE_MAP(CStringDlg, CDialog)
        //{{AFX_MSG_MAP(CStringDlg)
                // NOTE: the ClassWizard will add message map macros here
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()


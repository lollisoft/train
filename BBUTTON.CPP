// bbutton.cpp : bitmap button test
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "train.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// BitmapButton Test dialog #1

// In this example we pass the bitmap resource names to LoadBitmaps
//  OnInitDialog is used to Subclass the buttons so the dialog
//  controls get attached to the MFC WndProc for C++ message map dispatch.

class AFX_CLASS_EXPORT CBMTest1Dlg : public CDialogBar
{
protected:
        CBitmapButton button1, button2;
public:
        //{{AFX_DATA(CBMTest1Dlg)
                enum { IDD = IDD_TEST_BITMAP_BUTTON1 };
        //}}AFX_DATA
        CBMTest1Dlg();

        BOOL OnInitDialog();
        //{{AFX_MSG(CBMTest1Dlg)
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CBMTest1Dlg, CDialogBar)
        //{{AFX_MSG_MAP(CBMTest1Dlg)
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()


CBMTest1Dlg::CBMTest1Dlg()
        : CDialogBar()
{
        // NOTE: The obsolete MFC V1 CBitmapButton constructor with 3 arguments is
        //  replaced by a call to LoadBitmaps.
        if (!button1.LoadBitmaps("Image1Up", "Image1Down", "Image1Focus") ||
                !button2.LoadBitmaps("Image2Up", "Image2Down", "Image2Focus"))
        {
                TRACE("Failed to load bitmaps for buttons\n");
                AfxThrowResourceException();
        }
}

BOOL CBMTest1Dlg::OnInitDialog()
{
        // each dialog control has special bitmaps
        VERIFY(button1.SubclassDlgItem(IDOK, this));
        button1.SizeToContent();
        VERIFY(button2.SubclassDlgItem(IDCANCEL, this));
        button2.SizeToContent();

        return TRUE;
}

// chkbkfrm.cpp : implementation of the CCheckBookFrame class
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp and/or WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.


#include "stdafx.h"
#include "chkbkfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CDrawFrame, CMDIChildWnd)

BOOL CDrawFrame::PreCreateWindow(CREATESTRUCT& cs)
{
        // By turning off the default MFC-defined FWS_ADDTOTITLE style,
        // the framework will use first string in the document template
        // STRINGTABLE resource instead of the document name.

        cs.style &= ~(LONG)FWS_ADDTOTITLE;
        return CMDIChildWnd::PreCreateWindow(cs);
}

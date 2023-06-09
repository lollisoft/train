// splitfra.h : header file
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
// CSplitFrame frame with splitter

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class AFX_CLASS_EXPORT CSplitFrame : public CMDIChildWnd
{
        DECLARE_DYNCREATE(CSplitFrame)
protected:
        CSplitFrame();          // protected constructor used by dynamic creation

// Attributes
protected:
        CSplitterWnd    m_wndSplitter;
public:

// Operations
public:

// Implementation
public:
        virtual ~CSplitFrame();
        virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

        // Generated message map functions
        //{{AFX_MSG(CSplitFrame)
                // NOTE - the ClassWizard will add and remove member functions here.
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

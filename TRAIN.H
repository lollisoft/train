// drawcli.h : main header file for the DRAWCLI application
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

#ifndef _DOS
#ifndef __AFXWIN_H__
  #error include 'stdafx.h' before including this file for PCH
#endif
#endif

#include "resource.h"       // main symbols

#define WM_IDLEACTIVATE WM_USER+1

/////////////////////////////////////////////////////////////////////////////
// CDrawApp:
// See drawcli.cpp for the implementation of this class
//

class AFX_CLASS_EXPORT CDrawApp : public CWinApp
{
public:
        CDrawApp();


        CMultiDocTemplate* m_pWindowViewTemplate;        // CWnd Objekte in CWindowView

        CMultiDocTemplate* m_pNetzEditViewTemplate;      // Netz bearbeiten

        CMultiDocTemplate* m_pFahrplanViewTemplate;      // Einsicht in Fahrpläne

        CMultiDocTemplate* m_pBenDefEditViewTemplate;    // Benutzerdefinierte Objekte

        CMultiDocTemplate* m_pKundenEditViewTemplate;    // Kunden - Verwaltung


        CMultiDocTemplate* m_pWagenEditViewTemplate;     // Editieren einzelner Wagen
        CMultiDocTemplate* m_pWagenParkEditViewTemplate; // Verwaltung des Wagenparks
        CMultiDocTemplate* m_pZugEditViewTemplate;       // Editieren eines Zuges

// Overrides
        virtual BOOL InitInstance();
#ifndef _OUTCOMMENT
        virtual BOOL OnIdle(LONG lCount);
#endif
        virtual int ExitInstance();
        virtual CDocument* OpenDocumentFile(LPCSTR lpszFileName);
        virtual void OnFileNew();

// Operations
  void UpdateIniFileWithDocPath(const char* pszPathName);


// Implementation
protected:
        CString GetDocPathFromIniFile();

        //{{AFX_MSG(CDrawApp)
        afx_msg void OnAppAbout();
        // NOTE - the ClassWizard will add and remove member functions here.
          //    DO NOT EDIT what you see in these blocks of generated code !
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

extern CDrawApp theApp;


/////////////////////////////////////////////////////////////////////////////

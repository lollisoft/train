
//
// This is a part of the Microsoft Foundation Classes C++ library.
// drawcli.cpp : Defines the class behaviors for the application.
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

#include "chkbkfrm.h"

// TABVIEW: Ordner in einem Fenster

#include "pageserv.h"

#include "persist.h"
#include "mainfrm.h"
#include "drawobj.h"
#include "drawdoc.h"
#include "basevw.h"
#include "pagevw.h"

#include "Net_Ed.h"  // NetzEditView

//#include "mainview.h" // Ordner

//#include "Net_Use.h"  // NetzUseView
#include "bendefvw.h"
#include "tabvw.h"
#include "windvw.h"
#include "kundenvw.h"
#include "splitfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrawApp

BEGIN_MESSAGE_MAP(CDrawApp, CWinApp)
  //{{AFX_MSG_MAP(CDrawApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
      // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
        //}}AFX_MSG_MAP
        // Standard file based document commands
        ON_COMMAND(ID_FILE_NEW, OnFileNew)
        ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
        // Standard print setup command
        ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawApp construction

CDrawApp::CDrawApp()
{
  // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDrawApp object

CDrawApp theApp;
BOOL     m_bOneWnd = FALSE;
BOOL     m_bShowAll = FALSE;
/////////////////////////////////////////////////////////////////////////////
// CDrawApp initialization

BOOL CDrawApp::InitInstance()
{
  // Initialize OLE 2.0 libraries
    if (!AfxOleInit())
      {
        AfxMessageBox(IDP_OLE_INIT_FAILED);
        return FALSE;
      }

        Ctl3dRegister     (m_hInstance);
        Ctl3dAutoSubclass (m_hInstance);


        // Standard initialization
        // If you are not using these features and wish to reduce the size
        //  of your final executable, you should remove from the following
        //  the specific initialization routines you do not need.

        SetDialogBkColor();        // set dialog background color to gray
        LoadStdProfileSettings();  // Load standard INI file options (including MRU)

        // Register the application's document templates.  Document templates
        //  serve as the connection between documents, frame windows and views.

        // TABVIEW

        m_pNetzEditViewTemplate = new CMultiDocTemplate(
                                      IDR_DRAWCLTYPE,
                                      RUNTIME_CLASS(CDrawDoc),
                                      RUNTIME_CLASS(CDrawFrame),
                                      RUNTIME_CLASS(CNetEd));
        m_pNetzEditViewTemplate->SetContainerInfo(IDR_DRAWCLTYPE_CNTR_IP);
        AddDocTemplate(m_pNetzEditViewTemplate);

        m_pBenDefEditViewTemplate = new CMultiDocTemplate(
                                        IDR_DRAWCLTYPEBENDEF,
                                        RUNTIME_CLASS(CDrawDoc),
                                        RUNTIME_CLASS(CDrawFrame),
                                        RUNTIME_CLASS(CBenDefView));
        m_pBenDefEditViewTemplate->SetContainerInfo(IDR_DRAWCLTYPE_CNTR_IP);
        AddDocTemplate(m_pBenDefEditViewTemplate);

        m_pFahrplanViewTemplate = new CMultiDocTemplate(
                                      IDR_DRAWCLTYPEFAHRPLAN,
                                      RUNTIME_CLASS(CDrawDoc),
                                      RUNTIME_CLASS(CDrawFrame),
                                      RUNTIME_CLASS(CTabView));
        m_pFahrplanViewTemplate->SetContainerInfo(IDR_DRAWCLTYPE_CNTR_IP);
        AddDocTemplate(m_pFahrplanViewTemplate);

        m_pWindowViewTemplate = new CMultiDocTemplate(
                                    IDR_DRAWCLTYPEWINDOW,
                                    RUNTIME_CLASS(CDrawDoc),
                                    RUNTIME_CLASS(CDrawFrame),
                                    RUNTIME_CLASS(CWndView));
        m_pWindowViewTemplate->SetContainerInfo(IDR_DRAWCLTYPE_CNTR_IP);
        AddDocTemplate(m_pWindowViewTemplate);

        m_pKundenEditViewTemplate = new CMultiDocTemplate(
                                    IDR_DRAWCLTYPEKUNDEN,
                                    RUNTIME_CLASS(CDrawDoc),
                                    RUNTIME_CLASS(CDrawFrame),
                                    RUNTIME_CLASS(CKundenView));
        m_pKundenEditViewTemplate->SetContainerInfo(IDR_DRAWCLTYPE_CNTR_IP);
        AddDocTemplate(m_pKundenEditViewTemplate);
        
        // create main MDI Frame window
        CMainFrame* pMainFrame = new CMainFrame;
        if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
        {
          // frame window creation failed.
            return FALSE;
        }
        m_pMainWnd = pMainFrame;

        // enable file manager drag/drop and DDE Execute open
        m_pMainWnd->DragAcceptFiles();
        EnableShellOpen();
        RegisterShellFileTypes();

        OnIdle(0);  // updates buttons before showing the window

        // the main window has been initialized, so show and update it.
        pMainFrame->ShowWindow(m_nCmdShow);
        pMainFrame->UpdateWindow();

        // simple command line parsing
        if (m_lpCmdLine[0] == '\0')
        {
          // Open file name saved in private INI file.
            CString strDocPath = GetDocPathFromIniFile();
              if (!strDocPath.IsEmpty())
                OpenDocumentFile(strDocPath);
        }
        else
        {
          // open an existing document
            OpenDocumentFile(m_lpCmdLine);
        }
        return TRUE;
}
#ifndef _OUTCOMMENT
// In this override of OnIdle we are doing UI for our app.
// Since this needs to be as fast as possible to give the user
// the best result we do our updates first when lCount is zero
// then we call the library to do its work.
BOOL CDrawApp::OnIdle(LONG lCount)
{
  if (lCount == 0)
    {
      ASSERT(m_pMainWnd != NULL);

        // look for any top-level windows owned by us
          // we use 'HWND's to avoid generation of too many temporary CWnds
            for (HWND hWnd = ::GetWindow(m_pMainWnd->m_hWnd, GW_HWNDFIRST);
              hWnd != NULL; hWnd = ::GetNextWindow(hWnd, GW_HWNDNEXT))
                {
                  if (::GetParent(hWnd) == m_pMainWnd->m_hWnd)
                    {
                      // if owned window is active, move the activation to the
                        //   application window
                          if (GetActiveWindow() == hWnd && (::GetCapture() == NULL))
                            m_pMainWnd->SetActiveWindow();

                              // also update the buttons for the top-level window
                                SendMessage(hWnd, WM_IDLEUPDATECMDUI, (WPARAM)TRUE, 0L);
                        }
                }
        }
        return CWinApp::OnIdle(lCount);
}
#endif

void CDrawApp::OnFileNew()
{
    CWinApp::OnFileNew();
}

int CDrawApp::ExitInstance()
{
  Ctl3dUnregister (m_hInstance);
    return CWinApp::ExitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// INI file implementation

static char BASED_CODE szIniFileSection[] = "Most Recent Check Book File";
static char BASED_CODE szIniFileEntry[] = "File";

void CDrawApp::UpdateIniFileWithDocPath(const char* pszPathName)
{
  WriteProfileString(szIniFileSection, szIniFileEntry, pszPathName);
}

CString CDrawApp::GetDocPathFromIniFile()
{
  return GetProfileString(szIniFileSection, szIniFileEntry, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// Überschriebene Funktionen

CDocument* CDrawApp::OpenDocumentFile(LPCSTR lpszFileName)
{

        CDrawDoc* pDoc = (CDrawDoc*)CWinApp::OpenDocumentFile(lpszFileName);
        if (pDoc == NULL)
        {
          CString strMessage;
            AfxFormatString1(strMessage, IDS_CANNOT_OPEN_DRAWDOC,
              lpszFileName);
                AfxMessageBox(strMessage);
                return NULL;
        }

        CMDIChildWnd* pNewFrame1 = (CMDIChildWnd*)m_pBenDefEditViewTemplate->CreateNewFrame(pDoc, NULL);
        if (pNewFrame1 == NULL)
          return pDoc;
        m_pBenDefEditViewTemplate->InitialUpdateFrame(pNewFrame1, pDoc);

        CMDIChildWnd* pNewFrame2 = (CMDIChildWnd*)m_pFahrplanViewTemplate->CreateNewFrame(pDoc, NULL);
        if (pNewFrame2 == NULL)
          return pDoc;
        m_pFahrplanViewTemplate->InitialUpdateFrame(pNewFrame2, pDoc);

        CMDIChildWnd* pNewFrame4 = (CMDIChildWnd*)m_pWindowViewTemplate->CreateNewFrame(pDoc, NULL);
        if (pNewFrame4 == NULL)
          return pDoc;
        m_pWindowViewTemplate->InitialUpdateFrame(pNewFrame4, pDoc);

        CMDIChildWnd* pNewFrame5 = (CMDIChildWnd*)m_pKundenEditViewTemplate->CreateNewFrame(pDoc, NULL);
        if (pNewFrame5 == NULL)
          return pDoc;
        m_pWindowViewTemplate->InitialUpdateFrame(pNewFrame5, pDoc);


        // Tile the two MDI children windows within the MDI frame window.

        ASSERT(pNewFrame1->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)));
        CMDIFrameWnd* pMDIFrameWnd = ((CMDIChildWnd*)pNewFrame1)->GetMDIFrame();
        ASSERT(pMDIFrameWnd != NULL);
        pMDIFrameWnd->MDITile(MDITILE_HORIZONTAL);

        return pDoc;
}




/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
  CAboutDlg();

// Dialog Data
  //{{AFX_DATA(CAboutDlg)
    enum { IDD = IDD_ABOUTBOX };
      //}}AFX_DATA

// Implementation
protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //{{AFX_MSG(CAboutDlg)
      // No message handlers
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
  //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
      //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
  //{{AFX_MSG_MAP(CAboutDlg)
    // No message handlers
      //}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CDrawApp::OnAppAbout()
{
  CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CDrawApp commands

// mainfrm.cpp : implementation of the CMainFrame class
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
//#include "tooltip.h"

// TABVIEW: Ordner

#include "mainview.h"
#include "basevw.h"
#include "pagevw.h"
#include "persist.h"

#include "mainfrm.h"

#include "Net_Ed.h"
//#include "Net_Use.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIPersistentFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIPersistentFrameWnd)

        //{{AFX_MSG_MAP(CMainFrame)
                // NOTE - the ClassWizard will add and remove mapping macros here.
                //    DO NOT EDIT what you see in these blocks of generated code !

        ON_COMMAND(ID_VIEW_NETZ_EDIT, OnViewNetzEdit)
//        ON_COMMAND(ID_VIEW_NETZ_USE , OnViewNetzUse)

        ON_WM_SYSCOLORCHANGE()                
        ON_WM_CREATE()
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars

// toolbar buttons - IDs are command buttons
static UINT BASED_CODE buttons[] = // Mainframe
{
        // same order as in the bitmap 'toolbar.bmp'
        ID_FILE_NEW,
        ID_FILE_OPEN,
        ID_FILE_SAVE,
                ID_SEPARATOR,
        ID_APP_ABOUT,
                ID_SEPARATOR,
        ID_HUNDERTPROZENT,
        ID_ZOOM,
                ID_SEPARATOR,
        ID_EDIT_CUT,
        ID_EDIT_COPY,
        ID_EDIT_PASTE,
                ID_SEPARATOR,
        ID_DRAW_SELECT,
                ID_SEPARATOR,
                ID_SEPARATOR, // Gruppe mit Seiten (was ist, wenn Untergruppen bestehen ?).
                ID_SEPARATOR,
        ID_SEITE_HOME,        // Seitenoperationen für jede View, wenn möglich
        ID_SEITE_DN,
        ID_DELETE_SEITE,
        ID_NEU_SEITE,
        ID_SEITE_UP,
        ID_SEITE_END,
};

// toolbar buttons - IDs are command buttons
static UINT BASED_CODE tools[] = // Netz editieren
{
        // same order as in the bitmap 'toolbar.bmp'
        ID_DRAW_TEXT,
        //ID_DRAW_TEXT_SENKRECHT,
                ID_SEPARATOR,
        ID_DRAW_ADD_ELEMENT,        
                ID_SEPARATOR,
        ID_DRAW_BAHNHOF,
        ID_CONNECT_BAHNHOF,
                ID_SEPARATOR,
        ID_DRAW_MAKE_FAHRPLAN,
        ID_CHOOSE_FAHRKARTE,
                ID_SEPARATOR,
        ID_FILE_PRINT,
};


// toolbar buttons - IDs are command buttons
static UINT BASED_CODE toolsfahrplan[] = // Netz editieren
{
        // same order as in the bitmap 'toolbar.bmp'
        ID_DRAW_TABELLE,
        ID_EDIT_TABELLE,
                ID_SEPARATOR,
        ID_ADD_FAHRT,
        ID_ADD_REVERS_FAHRPLAN,
        ID_FILE_PRINT,
};

// toolbar buttons - IDs are command buttons
static UINT BASED_CODE windowtools[] = // Window
{
        // same order as in the bitmap 'toolbar.bmp'
        ID_CREATE_WINDOW,
                ID_SEPARATOR,
        ID_FILE_PRINT,
};


// toolbar buttons - IDs are command buttons
static UINT BASED_CODE bendeftools[] = // Benutzerdefinierte Objekte
{
        // same order as in the bitmap 'toolbar.bmp'

                ID_SEPARATOR,
                ID_SEPARATOR,
                ID_SEPARATOR,
        ID_DRAW_LINE,
        ID_DRAW_RECT,
        ID_DRAW_ROUNDRECT,
        ID_DRAW_CIRCLE,
        ID_DRAW_POLYGON,
                ID_SEPARATOR,
        ID_DRAW_ADD_ELEMENT,
        ID_DRAW_ATTRIBUTE,        
                ID_SEPARATOR,
        ID_FILE_PRINT,
};

// toolbar buttons - IDs are command buttons
static UINT BASED_CODE kundentools[] = // Benutzerdefinierte Objekte
{
        // same order as in the bitmap 'toolbar.bmp'
        ID_NEU_KUNDE,
        ID_DELETE_KUNDE,
                ID_SEPARATOR,
        ID_FILE_PRINT,
};

static UINT BASED_CODE indicators[] =
{
        ID_SEPARATOR,           // status line indicator
        ID_SEPARATOR,    // Last action
        ID_INDICATOR_CAPS,
        ID_INDICATOR_NUM,
        ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
        // TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::OnViewBar(UINT nID)
{
    CWnd* pBar;
    if ((pBar = GetDlgItem(nID)) == NULL) return FALSE;

    pBar->ShowWindow((pBar->GetStyle() & WS_VISIBLE) == 0);
    if ((pBar->GetStyle() & WS_VISIBLE) == 0) return FALSE;
    RecalcLayout();
    return TRUE;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
        if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
                return -1;

        if (!CreateToolBar())
        return -1;

        if (!CreateBenDefBar())
        return -1;

        if (!CreateKundenBar())
        return -1;
        
        // Schienennetz editieren

        if (!m_wndNetzEditBar.Create(this, WS_CHILD|CBRS_TOP, IDW_EDIT) ||
                !m_wndNetzEditBar.LoadBitmap(IDR_DRAWCLTYPE) ||
                !m_wndNetzEditBar.SetButtons(tools,
                  sizeof(tools)/sizeof(UINT)))
        {
                TRACE("Failed to create toolbar\n");
                return -1;      // fail to create
        }

        if (!m_wndFahrplanBar.Create(this, WS_CHILD|CBRS_TOP, IDW_FAHRPLAN) ||
                !m_wndFahrplanBar.LoadBitmap(IDR_DRAWCLTYPEFAHRPLAN) ||
                !m_wndFahrplanBar.SetButtons(toolsfahrplan,
                  sizeof(toolsfahrplan)/sizeof(UINT)))
        {
                TRACE("Failed to create toolbar\n");
                return -1;      // fail to create
        }
        // Schienennetz benutzen (Fahrkarten auswählen etc.)

        // Fenster mit Dialog - Elementen

        if (!m_wndWindowBar.Create(this, WS_CHILD|CBRS_TOP, IDW_WINDOW) ||
                !m_wndWindowBar.LoadBitmap(IDR_DRAWCLTYPEWINDOW) ||
                !m_wndWindowBar.SetButtons(windowtools,
                  sizeof(windowtools)/sizeof(UINT)))
        {
                TRACE("Failed to create toolbar\n");
                return -1;      // fail to create
        }

        if (!m_wndStatusBar.Create(this))
        {
                TRACE("Failed to create status bar\n");
                return -1;      // fail to create
        }
        
        if (!m_wndStatusBar.SetIndicators(indicators,
                  sizeof(indicators)/sizeof(UINT)))
        {
                TRACE("Failed to create status bar\n");
                return -1;      // fail to create
        }

        return 0;
}

BOOL CMainFrame::CreateToolBar()
{
        if (!m_wndToolBar.Create(this) ||
                !m_wndToolBar.LoadBitmap(IDR_MAINFRAME) ||
                !m_wndToolBar.SetButtons(buttons,
                  sizeof(buttons)/sizeof(UINT)))
        {
                TRACE("Failed to create toolbar\n");
                return -1;      // fail to create
        }
        
        // Design guide advises 12 pixel gap between combos and buttons
        m_wndToolBar.SetButtonInfo(14, ID_SEPARATOR, TBBS_SEPARATOR, 12 );
    
        // Create the combo box
        m_wndToolBar.SetButtonInfo(15, IDW_TOOLCOMBO, TBBS_SEPARATOR, 100 );

        // Design guide advises 12 pixel gap between combos and buttons
        m_wndToolBar.SetButtonInfo(16, ID_SEPARATOR, TBBS_SEPARATOR, 12 );
        CRect rect;
        m_wndToolBar.GetItemRect( 15, &rect);
        rect.top = 1;
        rect.bottom = rect.top + 100;
        if (!m_wndToolBar.m_comboBox.Create(CBS_DROPDOWNLIST|WS_VISIBLE|WS_TABSTOP,
                        rect, &m_wndToolBar, IDW_TOOLCOMBO))
        {
                TRACE("Failed to create combo-box\n");
                return FALSE;
        }

        //  Fill the combo box
        m_wndToolBar.m_comboBox.AddString("Gruppen");
        m_wndToolBar.m_comboBox.AddString("Seiten");
        
        

        //  Create a font for the combobox
        LOGFONT logFont;
        memset(&logFont, 0, sizeof(logFont));

        // Since design guide says toolbars are fixed height so is the font.
        logFont.lfHeight = -13;
        logFont.lfWeight = FW_NORMAL;
        logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
        lstrcpy(logFont.lfFaceName, "MS Sans Serif");
        if (!m_wndToolBar.m_font.CreateFontIndirect(&logFont))
                TRACE("Could Not create font for combo\n");
        else
                m_wndToolBar.m_comboBox.SetFont(&m_wndToolBar.m_font);

        return TRUE;
}

BOOL CMainFrame::CreateBenDefBar()
{
        if (!m_wndBenDefBar.Create(this,WS_CHILD|CBRS_TOP, IDW_BENDEF) ||
                !m_wndBenDefBar.LoadBitmap(IDR_DRAWCLTYPEBENDEF) ||
                !m_wndBenDefBar.SetButtons(bendeftools,
                  sizeof(bendeftools)/sizeof(UINT)))
        {
                TRACE("Failed to create stylebar\n");
                return FALSE;       // fail to create
        }

        // Create the combo box
        m_wndBenDefBar.SetButtonInfo(1, IDW_BENDEFCOMBO, TBBS_SEPARATOR, 100 );

        // Design guide advises 12 pixel gap between combos and buttons
        m_wndBenDefBar.SetButtonInfo(2, ID_SEPARATOR, TBBS_SEPARATOR, 12 );
        CRect rect;
        m_wndBenDefBar.GetItemRect( 1, &rect);
        rect.top = 1;
        rect.bottom = rect.top + 100;
        if (!m_wndBenDefBar.m_comboBox.Create(CBS_DROPDOWNLIST|WS_VISIBLE|WS_TABSTOP,
                        rect, &m_wndBenDefBar, IDW_BENDEFCOMBO))
        {
                TRACE("Failed to create combo-box\n");
                return FALSE;
        }

        //  Fill the combo box
        m_wndBenDefBar.m_comboBox.AddString("Bahnhöfe");
        m_wndBenDefBar.m_comboBox.AddString("Strecken");
        m_wndBenDefBar.m_comboBox.AddString("Kunden - Tabellen");
        m_wndBenDefBar.m_comboBox.AddString("Fahrpläne");
        
        

        //  Create a font for the combobox
        LOGFONT logFont;
        memset(&logFont, 0, sizeof(logFont));

        // Since design guide says toolbars are fixed height so is the font.
        logFont.lfHeight = -13;
        logFont.lfWeight = FW_NORMAL;
        logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
        lstrcpy(logFont.lfFaceName, "MS Sans Serif");
        if (!m_wndBenDefBar.m_font.CreateFontIndirect(&logFont))
                TRACE("Could Not create font for combo\n");
        else
                m_wndBenDefBar.m_comboBox.SetFont(&m_wndBenDefBar.m_font);

        return TRUE;
}

BOOL CMainFrame::CreateKundenBar()
{
        if (!m_wndKundenBar.Create(this,WS_CHILD|CBRS_TOP, IDW_KUNDEN) ||
                !m_wndKundenBar.LoadBitmap(IDR_DRAWCLTYPEKUNDEN) ||
                !m_wndKundenBar.SetButtons(kundentools,
                  sizeof(kundentools)/sizeof(UINT)))
        {
                TRACE("Failed to create stylebar\n");
                return FALSE;       // fail to create
        }

        // Create the combo box
        m_wndKundenBar.SetButtonInfo(1, IDW_KUNDENCOMBO, TBBS_SEPARATOR, 100 );

        // Design guide advises 12 pixel gap between combos and buttons
        m_wndKundenBar.SetButtonInfo(2, ID_SEPARATOR, TBBS_SEPARATOR, 12 );
        CRect rect;
        m_wndKundenBar.GetItemRect( 1, &rect);
        rect.top = 1;
        rect.bottom = rect.top + 100;
        if (!m_wndKundenBar.m_comboBox.Create(CBS_DROPDOWNLIST|WS_VISIBLE|WS_TABSTOP,
                        rect, &m_wndKundenBar, IDW_KUNDENCOMBO))
        {
                TRACE("Failed to create combo-box\n");
                return FALSE;
        }

        //  Fill the combo box
        m_wndKundenBar.m_comboBox.AddString("Stammkunden");
        m_wndKundenBar.m_comboBox.AddString("Gruppen");
        
        

        //  Create a font for the combobox
        LOGFONT logFont;
        memset(&logFont, 0, sizeof(logFont));

        // Since design guide says toolbars are fixed height so is the font.
        logFont.lfHeight = -13;
        logFont.lfWeight = FW_NORMAL;
        logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
        lstrcpy(logFont.lfFaceName, "MS Sans Serif");
        if (!m_wndKundenBar.m_font.CreateFontIndirect(&logFont))
                TRACE("Could Not create font for combo\n");
        else
                m_wndKundenBar.m_comboBox.SetFont(&m_wndKundenBar.m_font);

        return TRUE;
}

CView* CMainFrame::FindView(CRuntimeClass* pViewClass)
{
        // If a check view or book view (specified by pViewClass)
        // already exists, then activate the MDI child window containing
        // the view.  Otherwise, create a new view for the document.

        CMDIChildWnd* pMDIActive = MDIGetActive();
        ASSERT(pMDIActive != NULL);
        CDocument* pDoc = pMDIActive->GetActiveDocument();
        ASSERT(pDoc != NULL);

        CView* pView;
        POSITION pos = pDoc->GetFirstViewPosition();
        while (pos != NULL)
        {
                pView = pDoc->GetNextView(pos);
                if (pView->IsKindOf(pViewClass))
                {
                        return pView;
                }
        }
        return NULL;
}

void CMainFrame::CreateOrActivateFrame(CDocTemplate* pTemplate,
        CRuntimeClass* pViewClass)
{
        // If a check view or book view (specified by pViewClass)
        // already exists, then activate the MDI child window containing
        // the view.  Otherwise, create a new view for the document.

        CMDIChildWnd* pMDIActive = MDIGetActive();
        ASSERT(pMDIActive != NULL);
        CDocument* pDoc = pMDIActive->GetActiveDocument();
        ASSERT(pDoc != NULL);

        CView* pView;
        POSITION pos = pDoc->GetFirstViewPosition();
        while (pos != NULL)
        {
                pView = pDoc->GetNextView(pos);
                if (pView->IsKindOf(pViewClass))
                {
                        pView->GetParentFrame()->ActivateFrame();
                        return;
                }
        }

        CMDIChildWnd* pNewFrame = (CMDIChildWnd*)pTemplate->CreateNewFrame(pDoc, NULL);
        if (pNewFrame == NULL)
                return;     // not created
        ASSERT(pNewFrame->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)));
        pTemplate->InitialUpdateFrame(pNewFrame, pDoc);
        MDITile(MDITILE_HORIZONTAL);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame commands

void CMainFrame::OnViewNetzEdit()
{
        CreateOrActivateFrame(theApp.m_pNetzEditViewTemplate,
                RUNTIME_CLASS(CNetEd));
}

//TABVIEW: Ordner

BOOL CMainFrame::OnSwitchTabs(UINT nID)
{
        CMDIChildWnd* pChild = MDIGetActive();
        ASSERT(pChild != NULL);
        CView* pView = (CView*)(pChild->GetActiveView());
        ASSERT(pView != NULL);
        CMainView* pMainView = (CMainView*)pView->GetParent();
        ASSERT(pMainView != NULL);
         
        if(!pMainView->IsKindOf(RUNTIME_CLASS(CMainView)))
                return 0;

        switch(nID)
        {
                case ID_GRAPHICS:
                        pMainView->switchTab(0);
                        break;
                case ID_TIMELINE:
                        pMainView->switchTab(1);
                        break;
        }
        return 1;
}

void CMainFrame::OnSysColorChange()
{
        CMDIFrameWnd::OnSysColorChange();
        Ctl3dColorChange();     
}



/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
        CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
        CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

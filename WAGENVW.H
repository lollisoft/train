// drawvw.h : interface of the CWagenView class
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


// Hints for UpdateAllViews/OnUpdate
class CDrawObj;
//class CBaseView;
class AFX_CLASS_EXPORT CWagenView : public CPageView
{
protected: // create from serialization only
        CWagenView();
        DECLARE_DYNCREATE(CWagenView)

// Operations
public:
        virtual CObList* GetDocObjects();

// Implementation
public:
        virtual ~CWagenView();
#ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
#endif

        virtual void OnActivateView(BOOL bActivate, CView* pActiveView, CView* pDeactiveView);
        virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo);
        virtual BOOL PreCreateWindow(CREATESTRUCT& cs);


protected:
        virtual void OnInitialUpdate(); // called first time after construct

        // Printing support
        virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
        virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
        virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

        // OLE Container support
public:
        virtual BOOL IsSelected(const CObject* pDocItem) const;

// Generated message map functions
protected:
        //{{AFX_MSG(CWagenView)

        afx_msg void OnShowLok(); // Toggle
        afx_msg void OnShowAllWagen(); // Toggle, auch Nicht-Personenwagen
        afx_msg void OnShowWagenUntereinander();
        afx_msg void OnShowWagenNebeneinander();


        afx_msg void OnDrawAddWagen();
        // Aktuell gewählter aus "Benutzerdefinierte Objekte/Wagen"

        afx_msg void OnUpdateDrawAddWagen(CCmdUI* pCmdUI);        

        afx_msg void OnDrawAddSitzgruppe();
        // Aktuell gewählter aus "Benutzerdefinierte Objekte/Sitzgruppen"

        afx_msg void OnUpdateDrawAddSitzgruppe(CCmdUI* pCmdUI);        

        afx_msg void OnDrawAddSitz();
        // Aktuell gewählter aus "Benutzerdefinierte Objekte/Sitze"

        afx_msg void OnUpdateDrawAddSitz(CCmdUI* pCmdUI);        


        // Sitzgruppen und Sitze können nur in einem Wagen existieren,
        // zusätzlich sollten Sie hier nicht direckt platziert werden.
        // Außer, man kann einen neu definierten Wagen hinterher in die
        // "Bibliothek" aufnehmen.


        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

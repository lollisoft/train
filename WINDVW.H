// drawvw.h : interface of the CWndView class
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
class AFX_CLASS_EXPORT CWndView : public CPageView
{
protected: // create from serialization only
        CWndView();
        DECLARE_DYNCREATE(CWndView)

// Operations
public:
        virtual CObList* GetDocObjects();

// Implementation
public:
        virtual ~CWndView();
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
        //{{AFX_MSG(CWndView)


        afx_msg void OnCreateWindow();
        afx_msg void OnUpdateCreateWindow(CCmdUI* pCmdUI);
        
        afx_msg void OnDrawBahnhof();
        afx_msg void OnUpdateDrawBahnhof(CCmdUI* pCmdUI);
        afx_msg void OnConnectBahnhof();
        afx_msg void OnUpdateConnectBahnhof(CCmdUI* pCmdUI);

        afx_msg void OnDrawAddElement();
        afx_msg void OnUpdateDrawAddElement(CCmdUI* pCmdUI);        

        afx_msg void OnDrawText();
        afx_msg void OnUpdateDrawText(CCmdUI* pCmdUI);

        afx_msg void OnAddDataObj();
        afx_msg void OnUpdateAddDataObj(CCmdUI* pCmdUI);

        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

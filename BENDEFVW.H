// drawvw.h : interface of the CBenDefView class
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
class AFX_CLASS_EXPORT CBenDefView : public CPageView
{
protected: // create from serialization only
        CBenDefView();
        DECLARE_DYNCREATE(CBenDefView)

// Operations
public:
        virtual CObList* GetDocObjects();

// Implementation
public:
        virtual ~CBenDefView();
#ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
#endif

        virtual void OnActivateView(BOOL bActivate, CView* pActiveView, CView* pDeactiveView);
        virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo);
        virtual BOOL PreCreateWindow(CREATESTRUCT& cs);


protected:
        virtual void OnInitialUpdate(); // called first time after construct
        //virtual void SetupPageServer();

        // Printing support
        virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
        virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
        virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

        // OLE Container support
public:
        virtual BOOL IsSelected(const CObject* pDocItem) const;

// Generated message map functions
protected:
        //{{AFX_MSG(CBenDefView)

        afx_msg void OnAddNewBenDefObj();
        afx_msg void OnDrawNeuElement();
        afx_msg void OnDrawAddElement();
        afx_msg void OnDrawAttribute(); 

        afx_msg void OnUpdateAddNewBenDefObj(CCmdUI* pCmdUI);
        afx_msg void OnUpdateDrawNeuElement(CCmdUI* pCmdUI);
        afx_msg void OnUpdateDrawAddElement(CCmdUI* pCmdUI);
        afx_msg void OnUpdateDrawAttribute(CCmdUI* pCmdUI); 

        afx_msg void OnDrawRoundRect();
        afx_msg void OnDrawRect();
        afx_msg void OnDrawLine();
        afx_msg void OnDrawCircle();
        afx_msg void OnDrawPolygon();

        afx_msg void OnUpdateDrawCircle(CCmdUI* pCmdUI);
        afx_msg void OnUpdateDrawLine(CCmdUI* pCmdUI);
        afx_msg void OnUpdateDrawRect(CCmdUI* pCmdUI);
        afx_msg void OnUpdateDrawRoundRect(CCmdUI* pCmdUI);
        afx_msg void OnUpdateDrawPolygon(CCmdUI* pCmdUI);



        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

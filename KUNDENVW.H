// drawvw.h : interface of the CKundenView class

class CDrawObj;

class AFX_CLASS_EXPORT CKundenView : public CPageView
{
protected: // create from serialization only
        CKundenView();
        DECLARE_DYNCREATE(CKundenView)

// Attributes
public:
        void SetPageSize(CSize size);
        virtual CObList* GetDocObjects();
        
// Operations
public:

// Implementation
public:
        virtual ~CKundenView();
#ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
#endif

        virtual void OnActivateView(BOOL bActivate, CView* pActiveView, CView* pDeactiveView);
        virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo);
        virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll);
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
        //{{AFX_MSG(CBaseView)

        afx_msg void OnNeuKunde();
        afx_msg void OnUpdateNeuKunde(CCmdUI* pCmdUI);
        afx_msg void OnDeleteKunde();
        afx_msg void OnUpdateDeleteKunde(CCmdUI* pCmdUI);

        /*afx_msg void OnKundeUp();
        afx_msg void OnUpdateKundeUp(CCmdUI* pCmdUI);
        afx_msg void OnKundeDn();
        afx_msg void OnUpdateKundeDn(CCmdUI* pCmdUI);
        afx_msg void OnKundeHome();
        afx_msg void OnUpdateKundeHome(CCmdUI* pCmdUI);
        afx_msg void OnKundeEnd();
        afx_msg void OnUpdateKundeEnd(CCmdUI* pCmdUI);
*/

        DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

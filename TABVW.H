// drawvw.h : interface of the CTabView class

class CDrawObj;
class CPageView;

class AFX_CLASS_EXPORT CTabView : public CPageView
{
protected: // create from serialization only
        CTabView();
        DECLARE_DYNCREATE(CTabView)

// Attributes
public:
        void SetPageSize(CSize size);
        virtual CObList* GetDocObjects();
        
// Operations
public:

// Implementation
public:
        virtual ~CTabView();
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

        afx_msg void OnDrawTabelle();
        afx_msg void OnUpdateDrawTabelle(CCmdUI* pCmdUI);

        afx_msg void OnAddReversFahrplan();
        afx_msg void OnUpdateAddReversFahrplan(CCmdUI* pCmdUI); 
        afx_msg void OnAddFahrt();
        afx_msg void OnUpdateAddFahrt(CCmdUI* pCmdUI); 

        DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

// drawvw.h : interface of the CPageView class

class CDrawObj;
class CBaseView;
class CPageServer;
class AFX_CLASS_EXPORT CPageView : public CBaseView
{
protected: // create from serialization only
        CPageView();
        DECLARE_DYNCREATE(CPageView)

        CString m_ViewPfad; // Pfad in PageServer zu den Daten dieser View.

        CString m_ThisViewName;

        CString m_ActualPage;
        CString m_ActualPfad;

// Attributes
public:
        void SetPageSize(CSize size);
        virtual CObList* GetPage();
        virtual CObList* GetDocObjects();

        CString GetThisViewName() { return m_ThisViewName; }
        void SetThisViewName(CString Name);
        void SetViewPfad(CString Pfad);
        void SetActualPage(CString Page);
        void SetActualPfad(CString Pfad);

        // Hauptpfad der View
        CString GetViewPfad() { return m_ViewPfad; }

        // Aktuelle Seite (Setzt sich zusammen)
        virtual CString GetActualPage();
        // Aktueller Pfad zu Seiten
        virtual CString GetActualPfad();        
        
// Operations
public:

// Implementation
public:
        virtual ~CPageView();
#ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
#endif

        virtual void OnActivateView(BOOL bActivate, CView* pActiveView, CView* pDeactiveView);
        virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo);
        virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll);
        virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

        CPageServer* GetPageServer();
        void ShowPageOverview();

protected:
        virtual void OnInitialUpdate(); // called first time after construct
        void SetupPageServer(CString Name);
        // Printing support
        virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
        virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
        virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);


// Generated message map functions
protected:
        //{{AFX_MSG(CBaseView)

        afx_msg void OnNeuSeite();
        afx_msg void OnUpdateNeuSeite(CCmdUI* pCmdUI);
        afx_msg void OnDeleteSeite();
        afx_msg void OnUpdateDeleteSeite(CCmdUI* pCmdUI);

        afx_msg void OnSeiteUp();
        afx_msg void OnUpdateSeiteUp(CCmdUI* pCmdUI);
        afx_msg void OnSeiteDn();
        afx_msg void OnUpdateSeiteDn(CCmdUI* pCmdUI);
        afx_msg void OnSeiteHome();
        afx_msg void OnUpdateSeiteHome(CCmdUI* pCmdUI);
        afx_msg void OnSeiteEnd();
        afx_msg void OnUpdateSeiteEnd(CCmdUI* pCmdUI);


        DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#ifndef TABVIEW_H
#define TABVIEW_H

// tabview.h : interface of the CMapView class
// Written by Gerry High
// V1.2 4/13/94
//
/////////////////////////////////////////////////////////////////////////////

class CTabInfo : public CObject
{
public:
        CTabInfo(){m_tabWidth = 0;m_tabLabel = NULL;m_pView = NULL;m_mnemonic = (char)0;m_active=TRUE;}
        ~CTabInfo()
        {
                if(m_tabLabel)  delete m_tabLabel;
                //don't need to delete view since it will
                //be handled by the delete of the parent
//              if(m_pView)     delete m_pView;
        }
        int             m_tabWidth;     // width of tab
        char*           m_tabLabel;     // label of tab 
        CView*          m_pView;        // pointer to CView object
        char            m_mnemonic;     // character of mnemonic
        BOOL            m_active;       //is this tab active?
};

enum eLookAndFeel { LAF_CHICAGO,LAF_MSWORD};
enum eTabPosition 
{ TABSONTOP, TABSONLEFT, TABSONLEFTBOT,TABSONRIGHT, TABSONRIGHTBOT, TABSONBOTTOM };

class CMapView : public CView             
{
protected: // create from serialization only
        CMapView();
        DECLARE_DYNCREATE(CMapView)

// Attributes
public:

// Operations
public:

// Implementation
public:
        virtual                 ~CMapView();
        virtual void    OnDraw(CDC* pDC);  // overridden to draw this view
        virtual void    OnInitialUpdate();

        virtual CView*  addTabView(CRuntimeClass* viewClass,CDocument* document,
                                        char* tabLabel, BOOL border = FALSE,BOOL show = TRUE,int tabWidth = 100);
        BOOL                    doSysCommand(UINT nID,LONG lParam);
        void                    enableView(int viewIndex, BOOL bEnable = TRUE);
        void                    setFrameBorderOn(BOOL on = TRUE);
        void                    setLAF(eLookAndFeel LAF=LAF_MSWORD);
        void                    setMargin(int margin=7);
        void                    setTabHeight(int height=25);
        void                    setTabPosition(eTabPosition tabPos=TABSONTOP);
        virtual void    switchTab(int viewIndex);
protected:
        virtual CView*  createTabView(CRuntimeClass* viewClass,CDocument* document,CWnd* parentWnd,
                                        BOOL border,BOOL show);
        
#ifdef _DEBUG
        virtual void    AssertValid() const;
        virtual void    Dump(CDumpContext& dc) const;
#endif


// Generated message map functions
protected:
        //{{AFX_MSG(CMapView)
        afx_msg BOOL    OnEraseBkgnd(CDC* pDC);
        afx_msg void    OnSize(UINT nType, int cx, int cy);
        afx_msg void    OnLButtonDown(UINT nFlags, CPoint point);
        afx_msg void    OnSetFocus(CWnd* pOldWnd);
        afx_msg int     OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

        void                    createFonts();
        void                    drawChicagoTabs(CDC *pDC, CPen& blackPen, CPen& darkPen, CPen& lightPen, CPen *pOldPen);
        void                    drawMSWordTabs(CDC *pDC, CPen& blackPen, CPen& darkPen, CPen& lightPen, CPen *pOldPen);
        void                    drawMSWordTopTabs(CDC *pDC, CPen& blackPen, CPen& darkPen, CPen& lightPen, CPen *pOldPen);
        void                    drawMSWordLeftTabs(CDC *pDC, CPen& blackPen, CPen& darkPen, CPen& lightPen, CPen *pOldPen);
        void                    drawMSWordRightTabs(CDC *pDC, CPen& blackPen, CPen& darkPen, CPen& lightPen, CPen *pOldPen);
        void                    repositionViews();
        BOOL                    switchTopTab(CPoint point);
        BOOL                    switchVerticalTab(CPoint point);
        int m_width;                            //view width
        int m_height;                           //view height
        int m_curTab;                           //index of current tab
        int m_nTabs;                            //number of tabs
        int m_tabHeight;                        //height of a tab (including margins, etc.)
        CObArray        m_tabArray;             //array of CTabInfo objects
        CView*          m_curView;              //current view
        eLookAndFeel    m_lookAndFeel;  // Look of Tabs (either LAF_CHICAGO or LAF_MSWORD)
        
        int             m_margin;               //margin around tab "pages"
        eTabPosition    m_position;             //position of tabs
        int             m_lfEscapement; //font escapement (rotation)
        BOOL            m_frameBorderOn;// draw tab page frame?

        CFont* m_normalFont;            //font of non-current tab
        CFont* m_boldFont;                      //font of current tab
};

/////////////////////////////////////////////////////////////////////////////
#endif //TABVIEW_H

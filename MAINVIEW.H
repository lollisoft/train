// ordview.h : interface of the CMainView class
//
/////////////////////////////////////////////////////////////////////////////
#ifndef TABVIEW_H
#include "mapview.h"
#endif
class CDrawDoc;
class CMainView : public CMapView
{
protected: // create from serialization only
        CMainView();
        DECLARE_DYNCREATE(CMainView)

// Attributes
public:
        CDrawDoc* GetDocument();

// Operations
public:

// Implementation
public:
        virtual ~CMainView();
        virtual void OnInitialUpdate();
#ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
        //{{AFX_MSG(CMainView)
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in mainview.cpp
inline CDrawDoc* CMainView::GetDocument()
   { return (CDrawDoc*) m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

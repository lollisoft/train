// editvw.h : interface of the CEditVw class
//
/////////////////////////////////////////////////////////////////////////////
#ifndef TABVIEW_H
#include "mapview.h"
#endif
class CEditVw : public CEditView
{
protected: // create from serialization only
        CEditVw();
        DECLARE_DYNCREATE(CEditVw)

// Attributes
public:
        CDrawDoc* GetDocument();
        CMapView* m_pMapView;

// Operations
public:

// Implementation
public:
        virtual ~CEditVw();

// Generated message map functions
protected:
        //{{AFX_MSG(CEditVw)
        afx_msg void OnSysCommand(UINT nChar,LONG lParam);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
        BOOL PreCreateWindow(CREATESTRUCT& cs);
};

#ifndef _DEBUG  // debug version in demoview.cpp
inline CDrawDoc* CEditVw::GetDocument()
   { return (CDrawDoc*) m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

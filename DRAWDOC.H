// drawdoc.h : interface of the CDrawDoc class
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

class CBaseView;
class CPageView;
class CPageServer;
//class CDrawView;
//class CTabView;
class CDrawObj;
//class CDrawNode;
//class CKante;

class AFX_CLASS_EXPORT CDrawDoc : public COleDocument
{
protected: // create from serialization only
        CDrawDoc();
        DECLARE_DYNCREATE(CDrawDoc)

// Attributes
public:

// Operationen bezogen auf die neue Seitenverwaltung


        void MakeChangesRegardingMe(CPageView* pView);

        CPageServer* GetPageServer() { return &m_PageServer; }

        void AddNewPage(CPageView* pView);
        BOOL IsAnyPageLeft(CPageView* pView);

        void DeletePage(CPageView* pView);
        BOOL ExsistAnyPage(CPageView* pView);

        void GetNextPage(CPageView* pView);
        BOOL IsNextPageAvailable(CPageView* pView);

        void GetPrevPage(CPageView* pView);
        BOOL IsPrevPageAvailable(CPageView* pView);

        void GetFirstPage(CPageView* pView);
        void GetLastPage(CPageView* pView);
//--------------------------------------------------
        CDrawObj* FindObject(WORD number);
       
        CObList* GetDeletedObjects() { return &m_deletedobjects; }


        void     SelectFahrplan(CDrawObj* pObj);

        const CSize& GetSize() const { return m_size; }
        void ComputePageSize();
        void ErrorMessage(LPCSTR msg);
        int GetMapMode() const { return m_nMapMode; }
        COLORREF GetPaperColor() const { return m_paperColor; }


        void SetStandartFont(LOGFONT font) { m_LogfontStandart = font; }
        LOGFONT GetStandartFont() { return m_LogfontStandart; }

// Operations
public:

        CBaseView* FindView(CString Name);
        CDrawObj* ObjectAt(const CPoint& point, CBaseView* pView);
        void Draw(CDC* pDC, CBaseView* pView);
        void Add(CDrawObj* pObj, CObList* pView);
        void Add(CDrawObj* pObj, CBaseView* pView);        
        void AddHead(CDrawObj* pObj, CBaseView* pView);
        void Remove(CDrawObj* pObj, CBaseView* pView);

        
// Overrides CDocument
        BOOL OnOpenDocument(const char* pszPathName);
        BOOL OnSaveDocument(const char* pszPathName);


public:
// Implementation
        void UpdateIniFileWithDocPath(const char* pszPathName);

        virtual ~CDrawDoc();
        virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
#endif

protected:
        virtual BOOL OnNewDocument();

        CObList m_deletedobjects;


        // Neue Seitenverwaltung

        CPageServer m_PageServer;


        // Für Netz:
//        CObList m_objects;
        
//        CObList m_bendefobjects;
        
//        CObList m_tabellen;
        int     selectedtab;

//        CObList m_kundenpages;
        int     m_ActualKunden;

//        CObList m_FahrplanPages;
//        CObList m_Fahrplaene;
//        CDrawObj* m_pActiveFahrplan;

        CSize m_size;
        int m_nMapMode;
        COLORREF m_paperColor;

// Standarts
        LOGFONT m_LogfontStandart;

// Generated message map functions
protected:
        //{{AFX_MSG(CDrawDoc)
                // NOTE - the ClassWizard will add and remove member functions here.
                //    DO NOT EDIT what you see in these blocks of generated code !
        afx_msg void OnViewPaperColor();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

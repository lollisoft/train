// drawobj.h - interface for CDrawObj and derivatives
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


#ifndef __DRAWOBJ_H__
#define __DRAWOBJ_H__



class CBaseView;
class CDrawDoc;

#include "mconnect.h" 

class AFX_CLASS_EXPORT CNamedObject : public CObject
{
protected:
        DECLARE_SERIAL(CNamedObject);
        CNamedObject() { m_pObj = NULL; }

public:
        ~CNamedObject() {}
        CString GetName() { return m_sName; }
        CObject*   GetObj() { return m_pObj; }
        void    SetObjAndName(CString n, CObject* pObj) {
                          ASSERT_VALID(pObj);
                          m_sName = n;
                          m_pObj = pObj;
                          }
protected:
        CObject* m_pObj;
        CString  m_sName;
};

/////////////////////////////////////////////////////////////////////////////
// CDrawObj - base class for all 'drawable objects'

class AFX_CLASS_EXPORT CDrawObj : public CMConnect
{
protected:
        DECLARE_SERIAL(CDrawObj);
        CDrawObj();

// Constructors
public:
        CDrawObj(const CRect& position);

// Attributes

        enum NetzTyp { Knoten, Kante, KeinNetz };

        NetzTyp Netz;

        

        void SetView(CBaseView* p);
        void AddViewList(CObList* pList);
        void UpDateViews();
        void RemoveView(CBaseView* p);
        CBaseView* FindHomeView();
        CBaseView* FindActiveView();
        POSITION FindView(CBaseView* pView);

        CObList* GetViewList();
        
        CPoint     GetPos(const CRect& rect);

        // Übernimmt Fang - Umrechnung
        void SetRect(const CRect& rect);

        void SetFang(const int f) { Fang = f; };
        int GetFang() { return Fang; };

        CRect GetRect() { return m_position; };
        virtual CRect GetClientRect(CDrawObj* pObj);

        CRect InflatedRect(CRect r);

        virtual void KillConnections() {};

        virtual void SetNumber(const WORD& i) { number = i; };

        virtual int GetHandleCount();
        virtual CPoint GetHandle(int nHandle);
        CRect GetHandleRect(int nHandleID, CBaseView* pView);
        virtual HCURSOR GetHandleCursor(int nHandle);
        virtual void SetLineColor(COLORREF color);
        virtual void SetFillColor(COLORREF color);

// Operations
        virtual void Draw(CDC* pDC);
        virtual void DrawInClient(CDC* pDC);

        CRect CalcClientRect();
        BOOL IsDrawInClient();

        enum TrackerState { normal, selected, active };
        virtual void DrawTracker(CDC* pDC, TrackerState state);
        virtual void MoveTo(const CRect& positon, BOOL NurSelbst = FALSE);
        virtual int HitTest(CPoint point, CBaseView* pView, BOOL bSelected);
        virtual BOOL Intersects(const CRect& rect);
        virtual void MoveHandleTo(int nHandle, CPoint point);
        virtual void OnOpen(CBaseView* pView);
        virtual CDrawObj* Clone(CDrawDoc* pDoc);
        void Invalidate();

// Overridables
        virtual void OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point);
// ?        virtual void OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnCancel();

// Implementation
public:
        virtual ~CDrawObj();
        virtual void Serialize(CArchive& ar);
#ifdef _DEBUG
        void AssertValid();
#endif

        // implementation data

private:

        CRect m_position;
       
protected:

        BOOL m_bPen;
        LOGPEN m_logpen;
        BOOL m_bBrush;
        LOGBRUSH m_logbrush;

        int Fang;  // Wird benötigt um Objekte in ein Raster zu "fangen".

        WORD number;
        BOOL numadded;
        BOOL m_bDraw;
        CObList m_ViewList;

        BOOL m_bObjectChanged;
};

////////////////////////////////////////////////////////////////////////
// specialized draw objects

class AFX_CLASS_EXPORT CDrawRect : public CDrawObj
{
protected:
        DECLARE_SERIAL(CDrawRect);
        CDrawRect();

public:
        CDrawRect(const CRect& position);

        void KillConnections();

// Implementation
public:
        virtual void Serialize(CArchive& ar);
        virtual void Draw(CDC* pDC);
        virtual int GetHandleCount();
        virtual CPoint GetHandle(int nHandle);
        virtual HCURSOR GetHandleCursor(int nHandle);
        virtual void MoveHandleTo(int nHandle, CPoint point);
        virtual BOOL Intersects(const CRect& rect);
        virtual CDrawObj* Clone(CDrawDoc* pDoc, CBaseView* pView);

        enum Shape {line, rectangle, roundRectangle, ellipse };
protected:
        
        Shape m_nShape;
        CPoint m_roundness; // for roundRect corners

        friend class CRectTool;

public:
        void SetShape(Shape S) { m_nShape = S; }
                
};

/////////////////////////////////////////////////////////////////////////////
//class CDrawPoly;

class AFX_CLASS_EXPORT CDrawPoly : public CDrawObj
{
protected:
        DECLARE_SERIAL(CDrawPoly);
        CDrawPoly();

public:
        CDrawPoly(const CRect& position);

// Operations
        void AddPoint(const CPoint& point);
        BOOL RecalcBounds(CObList* pView);

// Implementation
public:
        virtual ~CDrawPoly();
        virtual void Serialize(CArchive& ar);
        virtual void Draw(CDC* pDC);
        virtual void MoveTo(const CRect& positon, BOOL NurSelbst = FALSE);
        virtual int GetHandleCount();
        virtual CPoint GetHandle(int nHandle);
        virtual HCURSOR GetHandleCursor(int nHandle);
        virtual void MoveHandleTo(int nHandle, CPoint point);
        virtual BOOL Intersects(const CRect& rect);
        virtual CDrawObj* Clone(CDrawDoc* pDoc, CBaseView* pView);

        // static helper for creating arrays of points
        static CPoint* NewPoints(int nPoints);

protected:
        int m_nPoints;
        int m_nAllocPoints;
        CPoint* m_points;
        CDrawPoly* m_pDrawObj;

        friend class CPolyTool;
};

class CDrawItem;    // COleClientItem derived class

class AFX_CLASS_EXPORT CDrawOleObj : public CDrawObj
{
protected:
        DECLARE_SERIAL(CDrawOleObj);
        CDrawOleObj();

public:
        CDrawOleObj(const CRect& position);

// Implementation
public:
        virtual void Serialize(CArchive& ar);
        virtual void Draw(CDC* pDC);
        virtual CDrawObj* Clone(CDrawDoc* pDoc, CBaseView* pView);
        virtual void OnOpen(CBaseView* pView);
        virtual void MoveTo(const CRect& positon, BOOL NurSelbst = FALSE);
        static BOOL c_bShowItems;

        CDrawItem* m_pClientItem;
        CSize m_extent; // current extent is tracked separate from scaled position
};

#endif // __DRAWOBJ_H__

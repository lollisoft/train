// drawtool.h - interface for CDrawTool and derivatives
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


#ifndef __DRAWTOOL_H__
#define __DRAWTOOL_H__

class CDrawObj;
class CBaseView;

enum DrawShape
{
        select,
        hand, // Objekte interaktiv benutzen (z.B. einen Knopf drücken)
              // hand könnte Objekt.Select aufrufen
        line,
        rect,
        roundRect,
        ellipse,
        poly,
        text,

        bahnhof,
        connect,

        
        adddata,

        tabelle,
        bendef,
        addnewbendefobj,
        addconnection,
        choosefahrkarte,


        // Windows
        window,

        makefahrplan,

        makekundentabelle,
        makenewkunde,
        deletekunde,

        // Finanzverwaltung, je nach Objekt
        // unterschiedliches Verhalten.

        zeigekosten,
        editkosten, //...

        // Wagenfunktionen

          // Design

          addwagen,
          wagenplatzieren,
          addsitzgruppe,
          deletesitzgruppe,
          addsitz,
          deletesitz,

          // Wagen verwenden...  

          suchefreieplaetze, // Einzellne oder Gruppen in einem Wagen(Voreinstellung)
          erstellereservierkaertchen,
          besetzeplatz,
          gebeplatzfrei,
          besetzegruppe,
          gebegruppefrei,
};

class AFX_CLASS_EXPORT CDrawTool
{
// Constructors
public:
        CDrawTool(DrawShape nDrawShape);

// Overridables
        virtual void OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnCancel();

// Attributes
        DrawShape m_drawShape;

        static CDrawTool* FindTool(DrawShape drawShape);
        static CPtrList c_tools;
        static CPoint c_down;
        static UINT c_nDownFlags;
        static CPoint c_last;
        static DrawShape c_drawShape;
};

class AFX_CLASS_EXPORT CSelectTool : public CDrawTool
{
// Constructors
public:
        CSelectTool();

// Implementation
        virtual void OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point);
};

class AFX_CLASS_EXPORT CRectTool : public CDrawTool
{
// Constructors
public:
        CRectTool(DrawShape drawShape);

// Implementation
        virtual void OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point);
};

class AFX_CLASS_EXPORT CWindowTool : public CDrawTool
{
// Constructors
public:
        CWindowTool(DrawShape drawShape);

// Implementation
        virtual void OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point);
};

class AFX_CLASS_EXPORT CNodeTool : public CRectTool
{
// Constructors
public:
        CNodeTool(DrawShape drawShape);

// Implementation
        virtual void OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point);
};

class AFX_CLASS_EXPORT CAddConnectionTool : public CRectTool
{
// Constructors
public:
        CAddConnectionTool(DrawShape drawShape);

// Implementation
        virtual void OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point);
};

class AFX_CLASS_EXPORT CTabTool : public CRectTool
{
// Constructors
public:
        CTabTool(DrawShape drawShape);

// Implementation
        virtual void OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point);
};


class AFX_CLASS_EXPORT CPolyTool : public CDrawTool
{
// Constructors
public:
        CPolyTool();

// Implementation
        virtual void OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnCancel();

        CDrawPoly* m_pDrawObj;
};

class AFX_CLASS_EXPORT CBenDefTool : public CDrawTool
{
// Constructors
public:
        CBenDefTool(DrawShape drawShape);

// Implementation
        virtual void OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point);
};

class AFX_CLASS_EXPORT CMakeFahrplanTool : public CDrawTool
{
// Constructors
public:
        CMakeFahrplanTool(DrawShape drawShape);

// Implementation
        virtual void OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point);
};

class AFX_CLASS_EXPORT CMakeNewKundenTabelleTool : public CDrawTool
{
// Constructors
public:
        CMakeNewKundenTabelleTool(DrawShape drawShape);

// Implementation
        virtual void OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point);
};

class AFX_CLASS_EXPORT CMakeNewKundeTool : public CDrawTool
{
// Constructors
public:
        CMakeNewKundeTool(DrawShape drawShape);

// Implementation
        virtual void OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point);
};

class AFX_CLASS_EXPORT CDeleteKundeTool : public CDrawTool
{
// Constructors
public:
        CDeleteKundeTool(DrawShape drawShape);

// Implementation
        virtual void OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point);
};

class AFX_CLASS_EXPORT CWagenTool : public CDrawTool
{
// Constructors
public:
        CWagenTool(DrawShape drawShape);

// Implementation
        virtual void OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point);
};

class AFX_CLASS_EXPORT CWagenBenutzenTool : public CDrawTool
{
// Constructors
public:
        CWagenBenutzenTool(DrawShape drawShape);

// Implementation
        virtual void OnLButtonDown(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonDblClk(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnLButtonUp(CBaseView* pView, UINT nFlags, const CPoint& point);
        virtual void OnMouseMove(CBaseView* pView, UINT nFlags, const CPoint& point);
};


enum SelectMode
{
        none,
        netSelect,
        move,
        size
};

extern SelectMode selectMode;
extern int nDragHandle;
extern CPoint lastPoint;
extern CSelectTool selectTool;
////////////////////////////////////////////////////////////////////////////

#endif // __DRAWTOOL_H__

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


#ifndef __WAGEN_H__
#define __WAGEN_H__

#include "tabobj.h" // In Implementation nicht bekannt!


class CDrawDoc;

class AFX_CLASS_EXPORT CSitzplatz : public CDrawRect
{
protected:
   DECLARE_SERIAL(CSitzplatz);
   CSitzplatz();

// Constructors
public:
   CSitzplatz(const CRect& position);
   ~CSitzplatz();
// Implementation
public:
   virtual void Serialize(CArchive& ar);
   virtual void Draw(CDC* pDC);

   virtual int HitTest(CPoint point, CBaseView* pView, BOOL bSelected);
   virtual HCURSOR GetHandleCursor(int nHandle);
   virtual void MoveTo(const CRect& positon, BOOL NurSelbst = FALSE);
   virtual void MoveHandleTo(int nHandle, CPoint point);

   virtual void KillConnections();

// Funktionen Sitzplatzbetreffend

   void Besetze_mit(CKundenDatensatz* Kunde); // Kunde ist irgendwo gespeichert
   void Freigeben();
   CKundenDatensatz* GetKunde();

   friend class CDrawDoc;
   friend class CWagenTool;

protected:
        enum Shape { mittelplatz, fensterplatz, gangplatz };
        Shape m_nShape;

        BOOL besetzt;        

        CRect m_oldpos;
        
        CPalette* m_palDIB;
        CSize m_sizeDoc;
        CSize m_size;

        COLORREF m_Besetzt, m_Frei;
};

class AFX_CLASS_EXPORT CSitzreihe : public CTabelleObj
{
protected:
        DECLARE_SERIAL(CSitzreihe);
        CSitzreihe();

// Constructors
public:
        CSitzreihe(CPoint P, int Sitze, CDrawDoc* pDoc);

// Implementation
public:
        virtual ~CSitzreihe();
        virtual void Serialize(CArchive& ar);

// Implementation data
protected:
        // CGruppeObj dient als Referenz.
        // Sie wird wo anders gespeichert.
        // Es ist moeglich, eine Gruppe in mehreren Fahrten zu
        // reservieren, nicht aber gleichzeitig in einer mehrfach!
        // In einer Gruppe muá eine/mehrere Verbindung/en zu verschiedenen
        // Zugfahrten bestehen
        virtual void BesetzeGruppe(CGruppeObj* pGruppe);
        virtual void GebeGruppeFrei();
        int GetAnzahl() { return m_Sitzreihe.GetCount(); }

        friend class CWagenTool;

private:
        BOOL BesetzeUmgebung(CGruppeObj* pGruppe, int rest);

        CObList m_Sitzreihe;
};

class AFX_CLASS_EXPORT CSitzgruppe : public CSitzreihe
{
protected:
        DECLARE_SERIAL(CSitzgruppe);
        CSitzgruppe();

// Constructors
public:
        CSitzgruppe(CPoint P, int Sitze_a_Reihe, CDrawDoc* pDoc);

// Implementation
public:
        virtual ~CSitzgruppe();
        virtual void Serialize(CArchive& ar);

// Implementation data
protected:
        virtual void BesetzeGruppe(CGruppeObj* pGruppe);
        virtual void GebeGruppeFrei();
};

class AFX_CLASS_EXPORT CWagen : public CTabelleObj
{
protected:
        DECLARE_SERIAL(CWagen);
        CWagen();

// Constructors
public:
        CWagen(CPoint P, CDrawDoc* pDoc);

// Implementation
public:
        virtual ~CWagen();
        virtual void Serialize(CArchive& ar);

        virtual void Draw(CDC* pDC);

private:
        void DrawOutlineFrame(CDC* pDC);

// Implementation data
protected:
        void SetInsertMode(BOOL mode) { m_bmode = mode; }
        void AddSitzgruppe(int x, int y); // x = vorne bis hinten
                                           // y = rechts bis links (jeweils in Fahrtrichtung).
                                           // Hinzugefügt wird die aktuelle Sitzgruppe

// Äußere Form des Wagens:

        void ChooseEckform(); // z.B. "Views/Benutzer..Objekte/Wagen/Eckformen/..."
        void InitUsedColorsEtc();

        CObList m_ShapePoints;
        BOOL m_bmode;
};

class AFX_CLASS_EXPORT CEckform : public CDrawRect
{
protected:
        DECLARE_SERIAL(CEckform);
        CEckform();

// Constructors
public:
        CEckform(CPoint P, CDrawDoc* pDoc);

// Implementation
public:
        virtual ~CEckform();
        virtual void Serialize(CArchive& ar);

// Implementation data
protected:
        void AddPoint(int at, CPoint p);

        CObList m_Points;
};


#endif // WAGEN

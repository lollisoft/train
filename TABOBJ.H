////////////////////
// Tabellen - Objekt

/* Diese Klasse soll eine Tabelle auf dem Bildschirm darstellen,
   welcher von CBaseView abgeleitet ist. Die Tabelle selbst ist
   von CDrawObj abgeleitet. */

#ifndef __TABOBJ_H__
#define __TABOBJ_H__


class AFX_CLASS_EXPORT CTabelleConnect : public CMConnect
{
protected:
        DECLARE_SERIAL(CTabelleConnect);
        CTabelleConnect();

public:
        CTabelleConnect(CDrawDoc* pConnHome);
        ~CTabelleConnect();        

// Implementation
public:
        virtual void Serialize(CArchive& ar);

        BOOL IsConnected() { return Connected; }
        void DeleteConnection() { Connected = FALSE; }


// Data - Access

protected:
        BOOL Connected;
    
};

class AFX_CLASS_EXPORT CTabelleObj : public CDrawObj
{
protected:
        DECLARE_SERIAL(CTabelleObj);
        CTabelleObj();

// Constructors
public:
        CTabelleObj(CPoint P, int Zeilen, int Spalten, CDrawDoc* pDoc);

// Implementation
public:
        virtual ~CTabelleObj();
        virtual void Serialize(CArchive& ar);

// Operations

        void MakeDummy(int Zeilen, int Spalten);
        BOOL CalcRect();

        virtual void Draw(CDC* pDC);
        virtual void MoveTo(const CRect& position, BOOL NurSelbst = FALSE);
        virtual int HitTest(CPoint point, CBaseView* pView, BOOL bSelected);
        virtual int GetHandleCount();
        CRect GetHandleRect(int nHandleID, CBaseView* pView);
        virtual CPoint GetHandle(int nHandle);
        virtual HCURSOR GetHandleCursor(int nHandle);
        virtual void MoveHandleTo(int nHandle, CPoint point);
        void KillConnections();

        // Tabelle
protected:
        void DeleteSpalte(int s);
        void InsertSpalte(int s, CObList* Spalte = NULL);

        void DeleteZeile(int z);
        void InsertZeile(int z, CObList* Zeile = NULL);

        void SetElement(CDrawObj* pObj, int z, int s);
        CDrawObj* GetElement(int z, int s);
        void SetElementSizes(int z, int s, int cz, int cs);


        CRect GetClientRect(CDrawObj* pObj);
        CRect GetClientRect(int Zeile, int Spalte);
        CPoint GetLocalPos(int Zeile, int Spalte);
        void RecalcAllPositions();


// Implementation data
protected:
        CObList m_TabellenZeilen;

        WORD m_Zeilen, m_Spalten;

        // Diese beiden Angaben ergeben die Fläche eines Feldes, in dem das
        // eingehängte Objekt hineinzeichnen soll.

        // Die Angabe der Spaltenbreite und Höhe kann sich eigentlich nur auf
        // eine ganze Spalte/Zeile beziehen. Daher ist wohl eher eine Liste
        // mit Feldbreite und eine mit Feldhöhe nötig, damit jedes Element
        // gezielt auf seine Größe zugreifen kann.
        
        CObList SizesOfSpalte;
        CObList SizesOfZeile;


        // Abspeichern der Handles:
        // Der erste Handle ist z.B. Rect.Top/Left
        // Der zweite ist dann Rect.Top + SizesOfSpalte[1]->Get() / Left
        // Der dritte ist dann Rect.Top + SizesOfSpalte[1]->Get() +
        //                              + SizesOfSpalte[2]->Get() / Left


        /*            " = L...
          L(1) ->  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*  Zeile 1
                    | | | | | | | | | | | | | | | |                                
          L(2) ->  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*  Zeile 2
                    | | | | | | | | | | | | | | | |
          L(3) ->  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*  Zeile ..
                    | | | | | | | | | | | | | | | |                                
          L(4) ->  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*    
                    | | | | | | | | | | | | | | | |
          L(5) ->  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*    
                    | | | | | | | | | | | | | | | |-----> Querverbindungen für schnelleren                                
          L(6) ->  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*       Zugriff. Noch nicht implementiert.
        */

        friend class CTabTool;

};
#endif // __TABOBJ_H__

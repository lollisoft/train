// MConnect soll die dynamische Verbindung von Objekten verwalten


#ifndef __MCONNECT_H__
#define __MCONNECT_H__

#define DrawObj_seg "DRAWOBJ_CODE_SEG"

class CDrawObj;
class CDrawDoc;
class CMConnect;

class AFX_CLASS_EXPORT CConCount : public CObject
{
    DECLARE_SERIAL(CConCount);
    CConCount();
    

public:

    ~CConCount();

    virtual void Serialize(CArchive& ar);
    virtual void Add(CDrawObj* pObj);
    virtual void Sub(CDrawObj* pObj);
    virtual WORD GetAnzahl() { return anzahl; }

protected:

    CObList deletednumbers;
    WORD anzahl;
};

extern CConCount concount;
extern CObList Moved;
extern int Moves;
extern int OMoves;


class AFX_CLASS_EXPORT CInt : public CObject
{
protected:
        DECLARE_SERIAL(CInt);
        CInt();

public:
        CInt(int num);
        ~CInt();

        virtual void Serialize(CArchive& ar);

        virtual int Get() { return TheNumber; }
        virtual void Set(int I) { TheNumber = I; }

protected:
        int TheNumber;        

};

class AFX_CLASS_EXPORT CNumber : public CObject
{
protected:
        DECLARE_SERIAL(CNumber);
        CNumber();

public:
        CNumber(WORD num);
        ~CNumber();

        virtual void Serialize(CArchive& ar);

        virtual WORD Get() { return TheNumber; }
        virtual void Set(WORD I) { TheNumber = I; }

protected:
        WORD TheNumber;        
};

class AFX_CLASS_EXPORT CConnect : public CObject
{
protected:
        DECLARE_SERIAL(CConnect);
        CConnect();

public:
        CConnect(CDrawDoc* pConnHome);
        ~CConnect();        

// Implementation
public:
        virtual void Serialize(CArchive& ar);

// Data - Access

        virtual void SetConnection(CMConnect* pConnObj);
        virtual void SetConnection(WORD& i) { connection = i; }
        
        virtual CDrawObj* GetConnection();
        /*CDrawObj* GetConnection(CString S)
        {
            SetActualConnection(S);
            return GetConnection();
        }*/
        virtual CDrawObj* GetConnection(CString Typ);
        virtual WORD GetConnectionNumber() { return connection; }
        void SetDoc(CDrawDoc* D) { pHome = D; }
protected:
        CDrawDoc* pHome;
        WORD connection;
        CDrawObj* pObj;
        
};

enum MT_Mode // Art der Bewegung
{
    MT_OnlySelf,   //  Bewegt nur sich selbst
    MT_MoveRel,    //  Bewegt angeschlossene Objekte relativ mit
    MT_ChangeSize  //  Wird benötigt, um Netzlinien nach zu zeichnen
};

// Art der Verbindung, wie wird vorgegangen beim Zeichnen und Löschen etc.
/*
enum CString
{
    CT_AllMode,          // Normale Verbindung, angehängtes Objekt kann gelöscht werden
    CT_KnotenKante,      // Es werden nur noch die Kanten mitgelöscht
    CT_KanteKnoten,      // Es werden die beiden Knoten gelöscht
    CT_ServerClient,     // Der Client kann gelöscht werden   ???
    CT_ClientServer,     // Nur sich selbst löschen           ???
    CT_CopyClient,       // Hilfsverbindung, um komplexe Objekte kopieren zu können
    CT_CopyServer,       // Hilfsverbindung, Muster - Objekt
    CT_ReferFrom,        // Referenziertes Objekt, dieses kann nur gelöscht werden,
                         // wenn es vom Benutzer erlaubt wird
    CT_ReferTo,          // Referenzierendes Objekt, kann gelöscht werden

    CT_BenDefServer,     // Benutzerdefinierte Objekt - Verbindung, Löscht auch Clients
                         // vom Typ CT_BenDef... 
    CT_BenDefClient,     // Löscht sich selbst und weitere Clients vom Typ CT_BenDef...
                         // (mit Rückfrage auch den Server vom Typ CT_BenDef...)

    CT_Nothing,          // Wenn MoveConnections alle Verbindungen bewegen soll


    // Dieses Verbindungspaar wird verwendet, wenn eine Tabelle in einem Element
    // ein Objekt enthält. Dieses Objekt muß sich in die Fläche zeichnen, die sich
    // aus Spalte und Zeile errechnet.

    // Die Frage ist: kann ein Objekt in zwei Tabellen stehen, also kann dieses mehr
    // als eine CT_ServerObject - Connection haben ?

    
    CT_ClientObject,     // Objektverbindung zu einem Client, die gegenseite muß eine
                         // Objektverbindung zu sich haben (als CT_ServerObject)
    CT_ServerObject      // Wie oben, nur umgekehrt !
};*/

enum DT_Mode
{
    DT_DrawSelf,    // Zeichnet sich direkt an "position"
    DT_DrawInClient // Zeichnet sich an der gegebenen Stelle aus dem Server - Objekt.
                    // Wenn DT_DrawInClient gewählt ist, wird in Draw eine andere Routine
                    // aufgerufen, Draw selbst macht sonst nichts mehr.
};

class AFX_CLASS_EXPORT CConnTypElem : public CObject
{
protected:
        DECLARE_SERIAL(CConnTypElem);
        CConnTypElem();

public:
        CConnTypElem(CDrawDoc* pConnHome, CString S);
        ~CConnTypElem();

// Operations
public:

        CDrawObj* Find(CDrawObj* pObj);
        CDrawObj* FindObj(CDrawObj* pObj);

        virtual void InsertElement(CMConnect* pObj, CDrawDoc* pDoc);
        virtual void DeleteElement(CMConnect* pObj);

        virtual void DeleteElement(WORD& i);

        BOOL IsEmpty() { return ConnElements.IsEmpty(); }
        int GetCount() { return ConnElements.GetCount(); }

        void If_Exsist_pObj_then_Delete(CDrawObj* pObj);
        BOOL SearchDeleteAbleObjects(CObList* pList, CObList* pProblemList);
        void CutConnections(CDrawObj* pObj);

        void MoveConnections(const CRect& rect, const CRect& OldPos);
        void RemoveConnectionsAndReDraw(CDrawDoc* pDoc);
        void DeleteGruppe();
        void MeldeElementeAb(CDrawObj* pObj);
        void SetMax(int I) { Max = I; }
        void SetDoc(CDrawDoc* D);
        
        virtual CString GetTyp() { return Typ; }
        virtual void SetTyp(CString S) { Typ = S; }
        void SetMoveTyp(MT_Mode Mode) { MT_ = Mode; }

        virtual CObList* GetList() { return &ConnElements; }
        CDrawObj* GetElem();
        void ResetList() { selected = ConnElements.GetHeadPosition(); }

// Implementation

public:
        virtual void Serialize(CArchive& ar);

protected:
        int Max;
        MT_Mode MT_;

        CDrawDoc* pHome;
        CString Typ;
        POSITION selected;
        CObList ConnElements;
};

extern CObList Moved;
extern CObList Visited;
class AFX_CLASS_EXPORT CMConnect : public CObject
{
protected:
        DECLARE_SERIAL(CMConnect);
        CMConnect();

        CMConnect(CDrawDoc* pConnHome);
        ~CMConnect();

// Implementation
public:
        virtual void Serialize(CArchive& ar);

// Data - Access

        DT_Mode GetDT_Mode() { return m_Mode; }
        void    SetDT_Mode(DT_Mode m) { m_Mode = m; }
        // Definieren neuer Verbindungsarten

        void SetConnectionTyp(CString ConnTyp);
        void SetMaxConnections(int I);
        int  GetConnCount();
        void SetMoveTyp(MT_Mode Mode);
        void SetDrawTyp(DT_Mode Mode);

        // Damit einfachere Aufrufe m”glich sind
        BOOL SetActualConnection(CString ConnTyp);

        virtual void SetConnection(CMConnect* pObj);
        virtual WORD GetNumber() { return number; };
        
        CDrawObj* GetPropertyObj(CString S, int i);

        CMConnect* GetConnection(CString Typ);
        CMConnect* GetConnection();
        
        void DeleteGruppe(CString S);
        WORD GetConnectionNumber();
        CDrawObj* GetOtherTarget(CDrawObj* pObj);

        // Move macht alle Bewegungen der angeh. Objekte.
        // ( Abhängig von Movemode (MT_Mode) )
        void MoveConnections(const CRect& rect, const CRect& OldPos, CString NotToMove = CString("Nothing"));

        BOOL SearchDeleteAbleObjects(CObList* pList, CObList* pProblemList);

        void If_Exsist_pObj_then_Delete(CDrawObj* pObj);
        void CutConnections();

        void RemoveConnectionsAndReDraw();
        virtual void RemoveConnections();
        virtual void RemoveConnections(CDrawDoc* pDoc);
        void UpDateConnections(CDrawDoc* pDoc);
        void MeldeElementeAb(CString S, CDrawObj* pObj);
        CRect GetNewRect();
        CDrawDoc* GetDoc(); 
        void  SetDoc(CDrawDoc* D);


        CDrawObj* Find(CDrawObj* pObj);
protected:

        void Connect(CMConnect* pObj1, CMConnect* pObj2,
                      MT_Mode mode1,   MT_Mode mode2,
                      CString S1,      CString S2);

        void Disconnect(CMConnect* pObj, CString S1);

protected:
        CDrawDoc* m_pDocument;
        POSITION selected;
        CObList ConnTypList;
        WORD number;

        DT_Mode m_Mode;
};

#endif // __MCONNECT_H__


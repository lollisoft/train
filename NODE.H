// node.h - Interface für Train Object ( Knoten )
// Dieses Modul beschreibt einen Knoten in einem Netz
// z.B. ein Netz für Bahnhöfe und deren Verbindungen.

// Die Idee: Verallgemeinerung des Netzes auf so viele 
// Anwendungen wie möglich.

// Es sollen weitere Klassen davon abgeleitet werden können.


#ifndef __NODE_H__
#define __NODE_H__


class CDrawRect;
class CDrawDoc;
class CKante;

class AFX_CLASS_EXPORT CDrawNode : public CDrawRect
{
protected:
   DECLARE_SERIAL(CDrawNode);
   CDrawNode();

// Constructors
public:
   CDrawNode(const CRect& position);
   ~CDrawNode();
// Implementation
public:
   virtual void Serialize(CArchive& ar);
   virtual void Draw(CDC* pDC);

   virtual int HitTest(CPoint point, CBaseView* pView, BOOL bSelected);
   virtual HCURSOR GetHandleCursor(int nHandle);
   virtual void MoveTo(const CRect& positon, BOOL NurSelbst = FALSE);
   virtual void MoveHandleTo(int nHandle, CPoint point);

   virtual void MeldeKanteAb(CKante* pKante);
   virtual void KillConnections();

   CKante* FindKanteZuKnoten(CDrawNode* pObj);

   friend class CTabTool;

   friend class CNodeTool;
   friend class CDrawDoc;

protected:
        enum Shape { connect, bahnhof };
        Shape m_nShape;

        /* Liste mit Verbindungsobjekten, über dieses Objekt kann auf den anderen Knoten */
        /* zugegriffen werden. Fraglich ist der Zugriff über Pointer, da ich nicht weiß, */
        /* ob sich die Pointer beim Speichern und Laden verändern !!!                    */


        CRect m_oldpos;
        
        CPalette* m_palDIB;
        CSize m_sizeDoc;
        CSize m_size;

        int m_nMapMode;
        COLORREF m_paperColor;
};


class AFX_CLASS_EXPORT CKante : public CDrawRect
{
protected:
        DECLARE_SERIAL(CKante);
        CKante();

public:
        CKante(const CRect& position);
        ~CKante();        

// Implementation
public:
        virtual void Serialize(CArchive& ar);
        virtual void Draw(CDC* pDC);
        virtual void DrawTracker(CDC* pDC, TrackerState state);

        virtual void MoveTo(const CRect& positon, BOOL NurSelbst = FALSE);
        virtual int GetHandleCount();
        virtual CPoint GetHandle(int nHandle);
        virtual void MoveHandleTo(int nHandle, CPoint point);
        virtual void MeldeKnotenAb(CDrawNode* pNode);
        virtual void KillConnections();

        void ShowNormal();
        void ShowSelected();

protected:

        enum Shape { connect };
        Shape m_nShape;

        COLORREF m_SelectedColor;
        COLORREF m_NormalColor;

public:

        //CConnect* node_AB;
        int entfernung_AB;
        
        //CConnect* node_BA;
        int entfernung_BA;

protected:

        friend class CNodeTool;
        friend class CDrawDoc;        
};        



#endif // __NODE_H__

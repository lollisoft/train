// Das Objekt Kante stellt eine Verbindung von
// zwei Knoten dar, wobei Daten wie Entfernung
// usw. in der Kante gespeichert werden.

#ifndef __KANTE_H__
#define __KANTE_H__

class CDrawView;
class CDrawDoc;
class CDrawNode;

class CKante : public CDrawRect
{
protected:
        DECLARE_SERIAL(CKante);
        CKante();

public:
        CKante(const CRect& position);

// Implementation
public:
        virtual void Serialize(CArchive& ar);
        virtual void Draw(CDC* pDC);

protected:

        CDrawNode* node_AB;
        int entfernung_AB;
        
        CDrawNode* node_BA;
        int entfernung_BA;

        friend class CNodeTool;
}        

#endif // Kante

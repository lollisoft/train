// Das Objekt CDataElem stellt ein einfaches Datenobjekt dar,
// das in Textform Daten beinhaltet. Dieses Objekt kann an
// andere angeh„ngt werden, sowie selbst weitere Objekte
// aufnehmen.

#ifndef __DATAELEM_H__
#define __DATAELEM_H__

class CBaseView;
class CDrawDoc;
class CDrawObj;

class AFX_CLASS_EXPORT CDataElem : public CDrawRect
{
protected:
        DECLARE_SERIAL(CDataElem);
        CDataElem();

public:
        CDataElem(CDrawDoc* pDoc, const CString& Typ, const CString& Data = CString(""));
        ~CDataElem();

// Implementation
public:
        virtual void Serialize(CArchive& ar);

        virtual void Draw(CDC* pDC);
        virtual void DrawInClient(CDC* pDC);
        
        virtual void OnOpen(CBaseView* pView);

        CRect CalcRect(CDC* pDC);

        void SetAngle(int a);

        void SetTyp(CString S) { DataTyp = S; }
        CString GetTyp() { return DataTyp; }

        void SetElement(CString S) { DataElement = S; }
        CString GetElement() { return DataElement; }

        void KillConnections();

        virtual void MoveTo(const CRect& position, BOOL NurSelbst = FALSE);

        virtual CDrawObj* Clone(CDrawDoc* pDoc, CObList* pView);

protected:
        CPoint p;
        BOOL m_DC;
        
        BOOL m_bFont;
        LOGFONT m_Logfont;
        
        int m_Angle;
        int m_Rand;

        COLORREF m_FontColor;

        CString DataTyp;
        CString DataElement;

        friend class CNodeTool;
        friend class CDrawDoc;
};

#endif // DataElem

// Das Objekt CDrawWindow stellt ein einfaches Datenobjekt dar,
// das in Textform Daten beinhaltet. Dieses Objekt kann an
// andere angeh„ngt werden, sowie selbst weitere Objekte
// aufnehmen.

#ifndef __DRAWWINDOW_H__
#define __DRAWWINDOW_H__

class CBaseView;
class CDrawDoc;
class CDrawObj;

class AFX_CLASS_EXPORT CDrawWindow : public CDrawRect
{
protected:
        DECLARE_SERIAL(CDrawWindow);
        CDrawWindow();

public:
        CDrawWindow(const CString& Typ, const CString& Data = CString(""));
        ~CDrawWindow();

// Implementation
public:
        virtual void Serialize(CArchive& ar);

        virtual void Draw(CDC* pDC);
        virtual void DrawInClient(CDC* pDC);
        
        virtual void OnOpen(CBaseView* pView);

        CRect CalcRect(CDC* pDC);

        void SetAngle(int radius) { m_Angle = radius; }

        void SetTyp(CString S) { DataTyp = S; }
        CString GetTyp() { return DataTyp; }

        void SetElement(CString S) { DataElement = S; }
        CString GetElement() { return DataElement; }

        void KillConnections();

        virtual void MoveTo(const CRect& position, BOOL NurSelbst = FALSE);

        virtual CDrawObj* Clone(CDrawDoc* pDoc, CObList* pView);

        enum WNDTyp {button, edit, combo};
        

protected:

        BOOL m_bFont;
        LOGFONT m_Logfont;
        int m_Angle;
        int m_Rand;
        WNDTyp m_nWNDTyp;
        COLORREF m_FontColor;

        CString DataTyp;
        CString DataElement;

        friend class CWindowTool;
        friend class CDrawDoc;
};

#endif // DrawWindow

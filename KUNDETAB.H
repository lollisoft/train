////////////////////
// Fahrplan - Objekt

/* Diese Klasse soll einen Fahrplan auf einer View darstellen,
   welche von CBaseView abgeleitet ist. Der Fahrplan selbst ist
   von CTabelleObj abgeleitet. */

#ifndef __KUNDENTABELLE_H__
#define __KUNDENTABELLE_H__



class AFX_CLASS_EXPORT CKundenTabObj : public CTabelleObj
{
protected:
        DECLARE_SERIAL(CKundenTabObj);
        CKundenTabObj();

// Constructors
public:
        CKundenTabObj(CPoint P, int Zeilen, int Spalten, CDrawDoc* pDoc);

// Implementation
public:
        virtual ~CKundenTabObj();
        virtual void Serialize(CArchive& ar);

// Implementation data
protected:
        void AddKunde(CString Kunde);
        void AddKunde(int z);
        void DeleteKunde(CString Kunde);
        void DeleteKunde();
        void DeleteKunde(int z);
        void DeleteKunde(CDrawObj* pObj);

        void AddSpalte(CString Typ, CString Bezeichnung);
        void DeleteSpalte(CString Bezeichnung);
        void SelectSpalte(CString Bezeichnung);
        void SelectZeile(CString Bezeichnung, CString Eintrag);

        friend class CMakeNewKundenTabelleTool;
        friend class CMakeNewKundeTool;
        friend class CDeleteKundeTool;
};
#endif // __KUNDENTABELLE_H__

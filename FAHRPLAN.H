////////////////////
// Fahrplan - Objekt

/* Diese Klasse soll einen Fahrplan auf einer View darstellen,
   welche von CBaseView abgeleitet ist. Der Fahrplan selbst ist
   von CTabelleObj abgeleitet. */

#ifndef __FAHRPLAN_H__
#define __FAHRPLAN_H__



class AFX_CLASS_EXPORT CFahrplanObj : public CTabelleObj
{
protected:
        DECLARE_SERIAL(CFahrplanObj);
        CFahrplanObj();

// Constructors
public:
        CFahrplanObj(CPoint P, int Zeilen, int Spalten, CDrawDoc* pDoc);

// Implementation
public:
        virtual ~CFahrplanObj();
        virtual void Serialize(CArchive& ar);

// Implementation data
protected:
        CObList Haltepunkte;  // Wo der Zug anhae

        // Diese Routine ÅberprÅft auch noch die folgenden
        // StreckenfÅhrungen:
        //
        // Kreisferkehr
        // Schleife
        // Wendezug - Betrieb

        BOOL AddHaltepunkt(int s, CDrawObj* pObj);
        void SetSpaltenName(CDataElem* pObj);
        void ShowHiddenNormal();
        void ShowHiddenSelected();

        friend class CMakeFahrplanTool;

};
#endif // __FAHRPLAN_H__

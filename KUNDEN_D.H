// CKundenDatensatz speichert, alle Daten des Kunden, der eine Fahrkarte
// reserviert hat.

// CStammkundenDatensatz

// Alle Klassen müssen von CMConnect abgeleitet sein, damit sie im Datenverbund
// speicherbar sind.

#ifndef __KUNDENDATENSATZ_H__
#define __KUNDENDATENSATZ_H__



class CBaseView;
class CDrawDoc;

#include "mconnect.h"
//#include "kunden_d.h"

class AFX_CLASS_EXPORT CKundenDatensatz : public CMConnect
{
protected:
        DECLARE_SERIAL(CKundenDatensatz);
        CKundenDatensatz();

public:

        virtual void EditDatensatz(); // Dialog, mit dem die Daten aus CDatensatz!!
                                      // und CKundenDatensatz verändert werden können.
        virtual void KillConnections(); // Sinnvolle verbindungen


        // Kundendaten...

        void SetKundenName(CString V, CString N);
        CString GetKundenName(BOOL FirstVorname = FALSE); // Behrens, Lothar

        // Weitere Daten zum Kunden...

        void SetAlter(int a);
        int GetAlter();

        void SetWeiblich(BOOL w);
        BOOL IsWeiblich();

        // Gruppe?

        void SetGruppenStatus(BOOL g);
        BOOL GetGruppenStatus();
// Implementation
public:
        virtual void Serialize(CArchive& ar);


protected:
        BOOL weiblich;
        BOOL gruppe;
        int alter;
        CString VorName;
        CString NachName;
        
};

class AFX_CLASS_EXPORT CGruppeObj : public CMConnect
{
protected:
        DECLARE_SERIAL(CGruppeObj);
        CGruppeObj();

public:

        virtual void EditDatensatz(); // Dialog, mit dem die Daten aus CDatensatz!!
                                      // und CGruppeObj verändert werden können.
        virtual void KillConnections(); // Sinnvolle verbindungen


        // Kundendaten...


        // Weitere Daten zum Kunden...


        // Gruppe?

        int GetAnzahl() { return m_Kunden.GetCount(); }
        POSITION GetHeadPosition() { return m_Kunden.GetHeadPosition(); }
        CKundenDatensatz* GetNext(POSITION& pos) { return (CKundenDatensatz*) m_Kunden.GetNext(pos); }

// Implementation
public:
        virtual void Serialize(CArchive& ar);


protected:
       CObList m_Kunden; 
};


#endif //__KUNDENDATENSATZ_H__

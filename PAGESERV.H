// MConnect soll die dynamische Verbindung von Objekten verwalten


#ifndef __PAGE_SERVER_H__
#define __PAGE_SERVER_H__

//#define DrawObj_seg "DRAWOBJ_CODE_SEG"

class CDrawObj;
class CDrawDoc;
class CPageView;


// Soll die CPageServerElem - Klasse nicht nur die zu
// zeichnenden Elemente enthalten, sowie einen Namen etc. ?
class AFX_CLASS_EXPORT CPageServerElem : public CObject
{
protected:
        DECLARE_SERIAL(CPageServerElem);
        CPageServerElem();

public:
        CPageServerElem(CDrawDoc* pConnHome);
        ~CPageServerElem();

        void DeleteContents();
        void KillAnyConnectedObjects();

// Operations
public:
        virtual void Serialize(CArchive& ar);

        // Liefert die Seite, die in Pfad addressiert wird
        // oder die Aktuelle.
        // Dabei muá der Aufrufer nicht wissen wie der Pfad aussieht
        // bei einem Fehler wird ab da, wo er auftritt der weitere
        // Inhalt zur auswahl angezeigt.

        CObject* GetObject(); // Allgemeiner Container !
        CString  GetName() { return ObjectName; }
        // PageElements muá zuvor geleert sein
        void SetObject(CObject* pObj, CString Name);
protected:

        CDrawDoc* pHome;
        CString ObjectName; // Name des Objekts z.B. Kundenname
        CObject* m_pObject; // Das Objekt
        BOOL m_bObjectStored;
};
// Der PageServer enth„lt die Verzeichnisse, dann die Seiten,
// welche dann ein a Seite ein PageServerElem enth„lt

class AFX_CLASS_EXPORT CPageServer : public CObject
{
protected:
        DECLARE_SERIAL(CPageServer);
        CPageServer();

public:
        CPageServer(CDrawDoc* pConnHome);
        ~CPageServer();

        void DeleteContents();
        void KillAnyConnectedObjects();

// Implementation
public:
        virtual void Serialize(CArchive& ar);

        CObject* FindObject(WORD Number);
        CPageServer* FindPfad(CString Pfad);

        void SetDirFlag(BOOL flag) { m_bLayer = flag; }
        void ShowOverview(CString What);
        void FreePage(CString Page);
        void GenerateOverview(CString ToView, CString FromView);

        void InitDirToView(CPageView* pView, CString Name);


// Data - Access
        // Ist der Uebergabestring nicht vorhanden, gilt der Aktuelle Pfad!
        // (Seitenzahl ist in Pfad mit zu uebergeben)

        CString GetPfad();
        void SetPfad(CString Pfad);

        CString CheckPfad(CString Pfad);

        CString CheckForAName(CString Name);

// Layers:
        
        CString GetLayerName() { return m_LayerName; }
        void SetLayerName(CString Name);
        CPageServer* FindLayer(CString Layer);
        int FindLayerIndex(CString Layer);

        CObject* FindPage(CString ThisPage);

        void NewDir(CString Pfad = "");  // fuegt ein neues Unterverzeichnis ein
        void DeleteDir(CString Pfad = "");

// Layer - Strings:
protected:
        CString GenerateViewName(CString Pfad);
        CString GenerateLayerName(CString Pfad);
        CString RemoveLayerName(CString Pfad);
public:

// Pages:

        virtual int  GetPageNumber(CString Pfad = "");

        virtual CObList* GetPage(CString Pfad = "");
        void SetPage(CString Pfad = "");

        void AddPage(CString Pfad = ""); // fuegt eine neue Seite ein
        void DeletePage(CString Pfad = "");

        void GetNextPage();
        void GetPrevPage();
        void GetFirstPage();
        void GetLastPage();

        BOOL ExsistAnyPage(CString Pfad = CString(""));
        BOOL IsAnyPageLeft(CString Pfad = CString(""));
        //BOOL ExistAnyPage();
        BOOL IsNextPageAvailable();
        BOOL IsPrevPageAvailable();

        BOOL IsDirectory() { return m_bLayer; }

        void SetDoc(CDrawDoc* pDoc) { m_pDocument = pDoc; }

// Data
protected:
        CDrawDoc* m_pDocument;

        BOOL m_bLayer;
        CString m_LayerName; // Layer

        CString m_LastLayer; // Layermerker wenn Dir
        int m_LastPage; // Seitenmerker wenn kein Dir
        
        int m_LastIter;

        CObList m_NextPageLayers; // Liste mit "Unterverzeichnissen"
        // Diese Liste enth„lt CPageServerElem - Objekte, wenn
        // m_bLayer FALSE ist.
        // Wenn m_bLayer TRUE ist, sind in dieser Liste weitere
        // PageServer - Objekte


        // Zugriff direckt von CPageView

        friend class CPageView;

};

#endif // __PAGE_SERVER_H__


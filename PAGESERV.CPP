// Implementation des Mehrfach - Verbindungs - Objekts

#include "stdafx.h"
#include "train.h"

#include "basevw.h"
#include "pagevw.h"

#include "mconnect.h"

#include "drawobj.h"

#include "pageserv.h"
#include "drawdoc.h"

#include "tabvw.h"

#include "inhaltvw.h"

#include "node.h"



#include "cntritem.h"
#include "rectdlg.h"

#include <io.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>
#include <direct.h>
#include <limits.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_SERIAL(CPageServerElem, CObject, 0)

CPageServerElem::CPageServerElem()
{
    if (m_pObject != NULL)
    {
        m_pObject = NULL;
    }
}

CPageServerElem::CPageServerElem(CDrawDoc* pConnHome)
{
    ASSERT_VALID(pConnHome);
    pHome = pConnHome;
    if (m_pObject != NULL)
    {
        AfxMessageBox("Init PageServerElem..m_pObject");
        m_pObject = NULL;
    }
}

CPageServerElem::~CPageServerElem()
{
    if (m_pObject != NULL) AfxMessageBox("Warnung: PageServerElem..m_pObject != NULL!");
}

void CPageServerElem::DeleteContents()
{
    delete m_pObject;
    m_pObject = NULL;
}

void CPageServerElem::KillAnyConnectedObjects()
{
    if (m_pObject != NULL)
    {
        if (m_pObject->IsKindOf(RUNTIME_CLASS(CDrawObj)))
        {
            CMConnect* pConn = (CMConnect*) m_pObject;
            pConn->RemoveConnections();
        }
    }
}

void CPageServerElem::Serialize(CArchive& ar)
{
    ASSERT_VALID(this);
    CObject::Serialize(ar);
    if (ar.IsStoring())
    {
        ar.Write(&m_bObjectStored, sizeof(m_bObjectStored));

        if (m_bObjectStored)
        {
            ar << ObjectName;
            ar << m_pObject;
        }
    }
    else
    {
        // get the document back pointer from the archive
        pHome = (CDrawDoc*)ar.m_pDocument;
        ASSERT_VALID(pHome);
        ASSERT(pHome->IsKindOf(RUNTIME_CLASS(CDrawDoc)));

        ar.Read(&m_bObjectStored, sizeof(m_bObjectStored));
        if (m_bObjectStored)
        {
            ar >> ObjectName;
            ar >> m_pObject;
        }
    }
//    pObject->Serialize(ar);
}


CObject* CPageServerElem::GetObject()
{
    return m_pObject;
}

void CPageServerElem::SetObject(CObject* pObj, CString Name)
{
    ObjectName = Name;
    m_pObject = pObj;
}

IMPLEMENT_SERIAL(CPageServer, CObject, 0)


CPageServer::CPageServer()
{
}

CPageServer::CPageServer(CDrawDoc* pConnHome)
{
    ASSERT_VALID(this);
    ASSERT_VALID(pConnHome);

    m_pDocument = pConnHome;
    SetDirFlag(FALSE);
}

CPageServer::~CPageServer()
{
}

void CPageServer::DeleteContents()
{
    if (IsDirectory())
    {
        POSITION pos = m_NextPageLayers.GetHeadPosition();

        while (pos != NULL)
        {
            POSITION posdel = pos;
            CPageServer* pServer = (CPageServer*) m_NextPageLayers.GetNext(pos);
            pServer->DeleteContents();
            m_NextPageLayers.RemoveAt(posdel);
            delete pServer;
        }
    }
    else
    {
        POSITION pos = m_NextPageLayers.GetHeadPosition();

        while (pos != NULL)
        {
            POSITION posdel = pos;
            CPageServerElem* pServer = (CPageServerElem*) m_NextPageLayers.GetNext(pos);
            pServer->DeleteContents();
            m_NextPageLayers.RemoveAt(posdel);
            delete pServer;
        }
    }
}

void CPageServer::KillAnyConnectedObjects()
{
    if (IsDirectory())
    {
        POSITION pos = m_NextPageLayers.GetHeadPosition();

        while (pos != NULL)
        {
            CPageServer* pServer = (CPageServer*) m_NextPageLayers.GetNext(pos);
            pServer->KillAnyConnectedObjects();
        }
    }
    else
    {
        POSITION pos = m_NextPageLayers.GetHeadPosition();

        while (pos != NULL)
        {
            ((CPageServerElem*) m_NextPageLayers.GetNext(pos))->KillAnyConnectedObjects();
        }
    }
}

void CPageServer::Serialize(CArchive& ar)
{
    ASSERT_VALID(this);
    CObject::Serialize(ar);

    if (ar.IsStoring())
    {
        ar.Write(&m_bLayer, sizeof(m_bLayer));
        ar << m_LayerName;
        ar.Write(&m_LastPage, sizeof(m_LastPage));
        ar << m_LastLayer;
    }
    else
    {
        // get the document back pointer from the archive
        m_pDocument = (CDrawDoc*)ar.m_pDocument;
        ASSERT_VALID(m_pDocument);
        ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDrawDoc)));

        ar.Read(&m_bLayer, sizeof(m_bLayer));
        ar >> m_LayerName;
        ar.Read(&m_LastPage, sizeof(m_LastPage));
        ar >> m_LastLayer;
    }
    m_NextPageLayers.Serialize(ar);
}


void CPageServer::SetLayerName(CString Name)
{
    if (Name.IsEmpty())
    {
        AfxMessageBox("Layername kann nicht leer sein!");
        ASSERT(FALSE);
    }

    m_LayerName = Name;
}


CString CPageServer::RemoveLayerName(CString Pfad)
{
    CString Name = GenerateLayerName(Pfad);
    CString Rest = Pfad.Right(Pfad.GetLength() - m_LastIter);

    return Rest;
}

CString CPageServer::GenerateLayerName(CString Pfad)
{
    return GenerateViewName(Pfad);
}

CObject* CPageServer::FindObject(WORD number)
{
    if (IsDirectory())
    {
        POSITION pos = m_NextPageLayers.GetHeadPosition();

        while (pos != NULL)
        {
            CPageServer* pServer = (CPageServer*) m_NextPageLayers.GetNext(pos);

            CObject* pObj = pServer->FindObject(number);
            if (pObj != NULL) return pObj;
        }
    }
    else
    {
        POSITION pos = m_NextPageLayers.GetHeadPosition();

        while (pos != NULL)
        {
            CPageServerElem* pElem = (CPageServerElem*) m_NextPageLayers.GetNext(pos);

            CObject* pObj = pElem->GetObject();

            if (pObj != NULL)
            {
                if (pObj->IsKindOf(RUNTIME_CLASS(CObList)))
                {// Erwartetes Object ist korekterweise eine Liste
                    CObList* pList = (CObList*) pObj;

                    POSITION pos1 = pList->GetHeadPosition();

                    while (pos1 != NULL)
                    {
                        CDrawObj* pDrawObj = (CDrawObj*) pList->GetNext(pos1);

                        if (pDrawObj->GetNumber() == number) return pDrawObj;
                    }
                }
            }
        }
    }
    return NULL;
}

CPageServer* CPageServer::FindPfad(CString Pfad)
{
    CString Layer = GenerateLayerName(Pfad);
    CString Rest = RemoveLayerName(Pfad);

    POSITION pos = m_NextPageLayers.GetHeadPosition();

    if (!IsDirectory())
    {
        if (Rest.IsEmpty())
        {
            POSITION pos = m_NextPageLayers.GetHeadPosition();

            while (pos != NULL)
            {
                CPageServerElem* pElem = (CPageServerElem*) m_NextPageLayers.GetNext(pos);

                if (pElem->GetName() == Layer)
                {
                    return this;
                }
            }
            AfxMessageBox("Objekt nicht gefunden!");
            return NULL;
        }
        else
        {
            AfxMessageBox("Verzeichnis enthält Daten, keine weiteren Unterverzeichnisse!");
            return NULL;
        }
    }

    while (pos != NULL)
    {
        CObject* pO = m_NextPageLayers.GetNext(pos);

        if (pO->IsKindOf(RUNTIME_CLASS(CPageServer)))
        {// Unterverzeichnis
            CPageServer* pObj = (CPageServer*) pO;
            if (pObj->GetLayerName() == Layer)
            {
                if (Rest.GetLength() == 0)
                {// Pfad gefunden, wenn keine Seite angegeben wurde. Seitenzugriff wo anders.
                    return pObj;
                }
                else
                {// Teil des Pfades gefunden (Layer)
                    return pObj->FindPfad(Rest);
                }
            }
        }
    }
    return NULL;
}

void CPageServer::InitDirToView(CPageView* pView, CString Name)
{
    CString ViewPfad = pView->GetViewPfad();
    
    CPageServer* pServer = FindPfad(ViewPfad);

    if (pServer == NULL)
    {// Nur wenn der Pfad nicht gefunden wird, darf diese Funktion
     // einen neuen Pfad mit einer Eroeffnungsseite anlegen.

        NewDir(ViewPfad); // Rekursiv
        pServer = FindPfad(ViewPfad);
    }

    CString CorrectPfad = CheckPfad(Name);

    CPageServer* pServerView = pServer->FindPfad(CorrectPfad);

    if (pServerView == NULL)
    {// Pfad dieser View nicht da, erzeugen...
        pServer->NewDir(CorrectPfad);

        CPageServer* pViewPageServer = pServer->FindPfad(CorrectPfad);

        pViewPageServer->SetDirFlag(FALSE); // Nur noch Objekte <> CPageServer (Seiten)
        pViewPageServer->AddPage();
    }
/*    else
    {// pServerView da...
        if (IsDirectory())
        {
            AfxMessageBox("Hier dürften eigentlich nur noch Seiten sein!");
            ASSERT(FALSE);
        }
        else
        {
            if (m_NextPageLayers.IsEmpty())
            {
                AfxMessageBox("Keine Seiten da!");
                ASSERT(FALSE);
            }
        }
    }*/
}

BOOL CPageServer::ExsistAnyPage(CString Pfad)
{
    CPageServer* pServer = FindPfad(Pfad);
    if (pServer == NULL)
    {
        return FALSE;
    }
    return !IsAnyPageLeft(Pfad);
}

BOOL CPageServer::IsAnyPageLeft(CString Pfad)
{
    CString CorrectPfad = CheckPfad(Pfad);
    
    if (!Pfad.IsEmpty())
    {
        SetPfad(CorrectPfad); // Setzt m_LastLayer
    }
    
    if (!IsDirectory())
    {// Seiten
        if (!CorrectPfad.IsEmpty())
        {
            AfxMessageBox(CString(CorrectPfad + " ist kein Teil des Pfades mehr!"));
            return FALSE;
        }
        return !m_NextPageLayers.IsEmpty();
    }
    else
    {
        CPageServer* pServer = FindLayer(m_LastLayer); // Letztes Verzeichnis zur nächsten Ebene

        if (pServer != NULL)
        {
            return pServer->IsAnyPageLeft();
        }
        else
        {
            AfxMessageBox("Eingestellter Pfad falsch!");
            return NULL;
        }
    }
}

BOOL CPageServer::IsNextPageAvailable()
{
    if (!IsDirectory())
    {// Seiten
        if (m_LastPage == m_NextPageLayers.GetCount() - 1) return FALSE;
        POSITION pos = m_NextPageLayers.FindIndex(m_LastPage + 1);
        return  (pos != NULL);
    }
    else
    {
        CPageServer* pServer = FindLayer(m_LastLayer);

        if (pServer != NULL)
        {
            return pServer->IsNextPageAvailable();
        }
        else
        {
            AfxMessageBox("Eingestellter Pfad falsch!");
            return NULL;
        }
    }
}

BOOL CPageServer::IsPrevPageAvailable()
{
    if (!IsDirectory())
    {// Seiten
        if (m_LastPage == 0) return FALSE;
        POSITION pos = m_NextPageLayers.FindIndex(m_LastPage - 1);
        return  (pos != NULL);
    }
    else
    {
        CPageServer* pServer = FindLayer(m_LastLayer);

        if (pServer != NULL)
        {
            return pServer->IsPrevPageAvailable();
        }
        else
        {
            AfxMessageBox("Eingestellter Pfad falsch!");
            return NULL;
        }
    }
}

void CPageServer::AddPage(CString Name)
// Zukünftig AddObject(CString Pfad, CObject* pObj)
{
    if (IsDirectory())
    {
        FindLayer(m_LastLayer)->AddPage(Name);
        // Nun Rest nicht mehr nötig.
        return;
    }

    if (Name.IsEmpty())
    {
        //AfxMessageBox("Seite nicht angegeben! (0 wird angenommen)");
        m_LastPage = 0;
    }


    // Hier nur noch Pages...

    // Einfügen:

    if (m_NextPageLayers.IsEmpty())
    {
        CPageServerElem* pElem = new CPageServerElem();

        pElem->SetObject(new CObList(), "Test");
        
        m_NextPageLayers.AddHead(pElem);
        m_LastPage = 0;
    }
    else
    {
        CPageServerElem* pElem = new CPageServerElem();

        pElem->SetObject(new CObList(), "Test");

        POSITION pos = m_NextPageLayers.FindIndex(m_LastPage);
        m_NextPageLayers.InsertBefore(pos, pElem);
    }
    
    return; // !!

    
    if (Name == CString(""))
    {
        if (!IsDirectory()) // Seite einfügen, oder Object
        {
            POSITION pos = m_NextPageLayers.FindIndex(m_LastPage);
            m_NextPageLayers.InsertAfter(pos, new CObList());
        }
        else
        {
            CPageServer* pServer = FindPfad(m_LastLayer);

            if (pServer == NULL)
            {
                AfxMessageBox(CString("AddPage() - Fehler: Standartpfad noch nicht gesetzt/n") +
                              CString("oder Verzeichnis nicht gefunden."));
            }
            else
            {
                pServer->AddPage(Name);
            }
        }
    }
    else
    {
        CString CorrectPfad = CheckPfad(Name);

        CPageServer* pServer = FindPfad(CorrectPfad); // Ist eine Seite enthalten,
                                                     // so wird m_StandartPage
                                                     // neu indiziert

        if (pServer == NULL)
        {
            AfxMessageBox("Pfad nicht gefunden!");
        }
        else
        {
            if (pServer->IsDirectory())
            {
                AfxMessageBox("Pfad nach Layer " + pServer->GetLayerName() + " Fehlerhaft!");
            }
            else
            {
                pServer->AddPage(); // An m_StandartPage
            }
        }
    }
}

void CPageServer::DeletePage(CString Pfad)
{
}

int CPageServer::FindLayerIndex(CString Layer)
{
    FindLayer(Layer);
    return m_LastIter;
}

CPageServer* CPageServer::FindLayer(CString Layer)
{
    POSITION pos = m_NextPageLayers.GetHeadPosition();

    m_LastIter = 0;

    if (IsDirectory())
    {
        while (pos != NULL)
        {
            CPageServer* pServer = (CPageServer*) m_NextPageLayers.GetNext(pos);
            m_LastIter++;
            if (pServer->GetLayerName() == Layer) return pServer;
        }
    }
    else
    {
        AfxMessageBox("Bitte benutzen Sie ""FindPage"", dies ist ein Verzeichnis mit Daten.");
    }
    return NULL;
}

CObject* CPageServer::FindPage(CString ThisPage)
{
    POSITION pos = m_NextPageLayers.GetHeadPosition();

    m_LastIter = 0;

    if (!IsDirectory())
    {
        while (pos != NULL)
        {
            CPageServerElem* pServerElem = (CPageServerElem*) m_NextPageLayers.GetNext(pos);

            if (pServerElem->GetName() == ThisPage) return pServerElem->GetObject();
        }
    }
    else
    {
        AfxMessageBox("Bitte benutzen Sie ""FindLayer"", dies ist ein Verzeichnis.");
    }
    return NULL;
}

void CPageServer::NewDir(CString Pfad)
{
    CString Layer = GenerateLayerName(Pfad);
    CString NextLayer = RemoveLayerName(Pfad);

    CPageServer* pLayer = FindLayer(Layer);

    m_LastLayer = Layer;

    if ((pLayer == NULL) && (NextLayer.IsEmpty())) // Einfügen
    {
        CPageServer* pServer = new CPageServer(m_pDocument);

        pServer->SetLayerName(Layer); // nicht den Pfad, sondern nur den Namen
        pServer->SetDirFlag(TRUE); // Unterscheidung bei Directory (Layer)
        m_NextPageLayers.AddTail(pServer);
    }

    if ((pLayer == NULL) && (!NextLayer.IsEmpty()))
    {
        AfxMessageBox("Pfadangabe falsch (ein Layer stimmt nicht)!");
        ASSERT(FALSE);
    }

    if ((pLayer != NULL) && (!NextLayer.IsEmpty())) // Recursion
    {
        pLayer->NewDir(NextLayer);
    }

    if ((pLayer != NULL) && (NextLayer.IsEmpty())) // Pfad schon da
    {
        AfxMessageBox("Pfad existiert bereits!");
    }
}

void CPageServer::DeleteDir(CString Pfad)
{
}

int CPageServer::GetPageNumber(CString Pfad)
{
    if (Pfad.IsEmpty())
    {// Alter Pfad
        if (IsDirectory())
        {
            return FindLayer(m_LastLayer)->GetPageNumber(Pfad);
        }
        else
        {
            return m_LastPage;
        }
    }
    else
    {
        if (IsDirectory()) // Bsp. Netz
        {
            m_LastLayer = GenerateLayerName(Pfad);
            CPageServer* pServer = FindLayer(m_LastLayer);

            return pServer->GetPageNumber(RemoveLayerName(Pfad));
        }
        else
        {

            return GetPageNumber();
            
            /*CString test = GenerateLayerName(Pfad);
            if (test.IsEmpty())
            {
                GetPage();
            }
            else
            {
                BOOL ischar = FALSE;
                for (int i = 0; i < test.GetLength(); i++)
                {
                    if (isalpha(test[i])) ischar = TRUE;
                }
            }

            int s = atoi(Pfad);

            POSITION pos = m_NextPageLayers.FindIndex(s);

            return (CObList*) m_NextPageLayers.GetAt(pos);
            */
        }
    }
}

CObList* CPageServer::GetPage(CString Pfad) // "/Netz/Sinsheim/1"
{
    if (Pfad.IsEmpty())
    {// Alter Pfad
        if (IsDirectory())
        {
            AfxMessageBox("GetPage: Kein Directory angegeben!");
            ASSERT(FALSE);
            return NULL;
        }
        else
        {
            CPageServerElem* pElem = (CPageServerElem*) m_NextPageLayers.GetHead();
            CObList* pList = (CObList*) pElem->GetObject();
            ASSERT_VALID(pList);
            if (pList->IsKindOf(RUNTIME_CLASS(CObList)))
            {
                return pList;
            }
            else
            {
                AfxMessageBox("Rückgabe müsste eine Liste sein, ist es aber nicht!");
                ASSERT(FALSE);
                return NULL;
            }
        }
    }
    else
    {
        if (IsDirectory()) // Bsp. Netz
        {
            m_LastLayer = GenerateLayerName(Pfad);
            CPageServer* pServer = FindLayer(m_LastLayer);

            return pServer->GetPage(RemoveLayerName(Pfad));
        }
        else
        {
            CString test = GenerateLayerName(Pfad);
            if (test.IsEmpty())
            {
                GetPage();
            }
            else
            {
                BOOL ischar = FALSE;
                for (int i = 0; i < test.GetLength(); i++)
                {
                    if (isalpha(test[i])) ischar = TRUE;
                }
            }

            int s = atoi(Pfad);

            POSITION pos = m_NextPageLayers.FindIndex(s);

            CPageServerElem* pElem = (CPageServerElem*) m_NextPageLayers.GetAt(pos);
            CObList* pList = (CObList*) pElem->GetObject();
            ASSERT_VALID(pList);
            if (pList->IsKindOf(RUNTIME_CLASS(CObList)))
            {
                return pList;
            }
            else
            {
                AfxMessageBox("Rückgabe müsste eine Liste sein, ist es aber nicht!");
                ASSERT(FALSE);
                return NULL;
            }
        }
    }
}

void CPageServer::SetPage(CString Pfad)
{
}

CString CPageServer::GetPfad()
{
    if (IsDirectory())
    {
        return m_LastLayer + "/" + FindLayer(m_LastLayer)->GetPfad();
    }
    else
    {
        char cp[10];
        itoa(m_LastPage, cp, 10);
        return cp;
    }
}

void CPageServer::SetPfad(CString Pfad)
{
    CString CorrectPfad = CheckPfad(Pfad);

    m_LastLayer = GenerateLayerName(CorrectPfad);
}

CString CPageServer::CheckPfad(CString Pfad)
{
    return Pfad;
}

CString CPageServer::CheckForAName(CString Name)
{
    int i = Name.FindOneOf("/");
    if (i != -1)
    {// Zeichen darf nicht im Namen vorkommen -> Fehler!
        AfxMessageBox("CheckForAName..Name hat ungültige Zeichen!");
    }
    return Name;
}

void CPageServer::GetNextPage()
{
    if (IsDirectory())
    {
        FindLayer(m_LastLayer)->GetNextPage();
    }
    else
    {
        m_LastPage++;
    }
}
void CPageServer::GetPrevPage()
{
    if (IsDirectory())
    {
        FindLayer(m_LastLayer)->GetPrevPage();
    }
    else
    {
        m_LastPage--;
    }
}
void CPageServer::GetFirstPage()
{
    if (IsDirectory())
    {
        FindLayer(m_LastLayer)->GetFirstPage();
    }
    else
    {
        m_LastPage = 1;
    }
}
void CPageServer::GetLastPage()
{
    if (IsDirectory())
    {
        FindLayer(m_LastLayer)->GetLastPage();
    }
    else
    {
        m_LastPage == m_NextPageLayers.GetCount();
    }
}

void CPageServer::ShowOverview(CString What)
{
    CString v = GenerateViewName(What);
    CPageView* pView = (CPageView*) m_pDocument->FindView(v);
    CInhaltView* pInhaltView = (CInhaltView*) m_pDocument->FindView("Inhalt");
    FreePage("Inhalt");
    GenerateOverview("Inhalt", v);
    pView->OnActivateView(TRUE, pInhaltView, pView);
}

void CPageServer::FreePage(CString Page)
{
    CObList* list = GetPage(Page);
    POSITION pos = list->GetHeadPosition();
    while (!list->IsEmpty())
    {
        CDrawObj* pObj = (CDrawObj*) list->GetAt(pos);
        list->RemoveHead();
        pObj->KillConnections();
        delete pObj;
    }
}

CString CPageServer::GenerateViewName(CString Pfad)
{
    CString Name;
    CString pfad = Pfad;
    int l = pfad.GetLength();
    int i = 0;
    int count = 0;

    m_LastIter = 0;
    for  (i = 0; i < l; i++)
    {
        if (count <= 1)
        {
            m_LastIter++;
            if ((pfad[i] == '/') && (i > 0)) count++;
            if (pfad[i] != '/') Name += pfad[i];
            if (count == 1) return Name;
        }
    }
    return Name;
}

void CPageServer::GenerateOverview(CString ToView, CString FromView)
{
    CObList* list = GetPage(ToView);
    CString Name = GenerateViewName(FromView);
    CPageView* pView = (CPageView*) m_pDocument->FindView(Name);
}

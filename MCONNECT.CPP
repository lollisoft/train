// Implementation des Mehrfach - Verbindungs - Objekts

#include "stdafx.h"
#include "train.h"
#include "pageserv.h"
#include "drawdoc.h"
#include "basevw.h"
#include "pagevw.h"
#include "tabvw.h"
#include "drawobj.h"
#include "node.h"
#include "mconnect.h"


#include "cntritem.h"
#include "rectdlg.h"

#include <io.h>
#include <errno.h>

#include <math.h>
#include <direct.h>
#include <limits.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#pragma code_seg(DrawObj_seg);

CObList Moved;
int Moves;
int OMoves;
CObList Visited;
CConCount concount;

IMPLEMENT_SERIAL(CConCount, CObject, 0)

CConCount::CConCount()
{
    anzahl = 0;
}

CConCount::~CConCount()
{
}

void CConCount::Serialize(CArchive& ar)
{
    ASSERT_VALID(this);

    CObject::Serialize(ar);

    if (ar.IsStoring())
    {
        ar << anzahl;
    }
    else
    {
        ar >> anzahl;
    }

    deletednumbers.Serialize(ar);
}

void CConCount::Add(CDrawObj* pObj)
{
    if (deletednumbers.IsEmpty())
    {
        pObj->SetNumber(anzahl);
        anzahl += 1;
    }
    else
    {
        POSITION pos = deletednumbers.GetHeadPosition();

        CNumber* Num = (CNumber*)deletednumbers.GetAt(pos);

        pObj->SetNumber(Num->Get());
        deletednumbers.RemoveAt(pos);
        delete Num;
    }
}

void CConCount::Sub(CDrawObj* pObj)
{
    if (pObj != NULL)
    {
        CNumber* Num = new CNumber(pObj->GetNumber());

        POSITION pos = deletednumbers.GetHeadPosition();
        int d = 0;

        while (pos != NULL)
        {
            if (((CNumber*) deletednumbers.GetNext(pos))->Get() == Num->Get())
                d += 1;
        }

        if (d == 0)
            deletednumbers.AddHead(Num);
    }
    else AfxMessageBox("Fehler in Sub (pObj = NULL)");
}


IMPLEMENT_SERIAL(CNumber, CObject, 0)

CNumber::CNumber()
{
}

CNumber::CNumber(WORD num)
{
    TheNumber = num;
}

CNumber::~CNumber()
{
}

void CNumber::Serialize(CArchive& ar)
{
    CObject::Serialize(ar);

    if (ar.IsStoring())
    {
        ar << TheNumber;
    }
    else
    {
        ar >> TheNumber;
    }
}


IMPLEMENT_SERIAL(CInt, CObject, 0)

CInt::CInt()
{
}

CInt::CInt(int num)
{
    TheNumber = num;
}

CInt::~CInt()
{
}

void CInt::Serialize(CArchive& ar)
{
    CObject::Serialize(ar);

    if (ar.IsStoring())
    {
        ar.Write(&TheNumber, sizeof(TheNumber));
    }
    else
    {
        ar.Read(&TheNumber, sizeof(TheNumber));
    }
}


IMPLEMENT_SERIAL(CConnect, CObject, 0)

CConnect::CConnect()
{
    pObj = NULL;
    pHome = NULL;
}

CConnect::CConnect(CDrawDoc* pConnHome)
{
    pObj = NULL;
    ASSERT_VALID(pConnHome); // Fail (Copy)
    pHome = pConnHome;
}

CConnect::~CConnect()
{
    ASSERT_VALID(this);
    pObj = NULL;
    pHome = NULL;
}

void CConnect::Serialize(CArchive& ar)
{
    ASSERT_VALID(this);
    CObject::Serialize(ar);

    if (ar.IsStoring())
    {
        connection = GetConnection()->GetNumber();
        ar << connection;
    }
    else
    {
        // get the document back pointer from the archive
        pHome = (CDrawDoc*)ar.m_pDocument;
        ASSERT_VALID(pHome);
        ASSERT(pHome->IsKindOf(RUNTIME_CLASS(CDrawDoc)));

        ar >> connection;
        ASSERT(pObj == NULL);
    }
}

void CConnect::SetConnection(CMConnect* pConnObj)
{
    //pObj = pConnObj; // Weglassen -> Copy/Paste
    connection = pConnObj->GetNumber();
}


CDrawObj* CConnect::GetConnection(CString Typ)
{
    AfxMessageBox("Wiso diese Routine?");
    ASSERT(FALSE);
    return NULL;
}


CDrawObj* CConnect::GetConnection()
{
    if (pObj == NULL)
    {
        ASSERT_VALID(pHome);

        CObList* dlist = pHome->GetDeletedObjects();

        POSITION pos = dlist->GetHeadPosition();
        
        while (pos != NULL)
        {
            pObj = (CDrawObj*)dlist->GetNext(pos);
            if (pObj == NULL)
            {
                AfxMessageBox("Element in Liste mit gelöschten Objekten ist NULL");
                return NULL;
            }
            if (pObj->GetNumber() == connection) 
               return pObj;
        }


        pObj = pHome->FindObject(connection);
        ASSERT_VALID(pObj);
        return pObj;
    }
    else
    {
        return pObj;
    }
}


IMPLEMENT_SERIAL(CConnTypElem, CObject, 0)

CConnTypElem::CConnTypElem()
{
}

CConnTypElem::CConnTypElem(CDrawDoc* pConnHome, CString S)
{
    ASSERT_VALID(pConnHome);
    pHome = pConnHome;
    Typ = S;
}

CConnTypElem::~CConnTypElem()
{
    while (!ConnElements.IsEmpty())
    {
        delete ConnElements.RemoveHead();
    }
}

CDrawObj* CConnTypElem::Find(CDrawObj* pObj)
{
    POSITION pos = ConnElements.GetHeadPosition();
    while (pos != NULL)
    {
        CConnect* Conn = (CConnect*) ConnElements.GetNext(pos);

        if (Conn->GetConnection() != pObj) return Conn->GetConnection();
    }
    return NULL;
}

CDrawObj* CConnTypElem::FindObj(CDrawObj* pObj)
{
    POSITION pos = ConnElements.GetHeadPosition();
    while (pos != NULL)
    {
        CConnect* Conn = (CConnect*) ConnElements.GetNext(pos);

        if (Conn->GetConnection() == pObj) return Conn->GetConnection();
    }
    return NULL;
}


void CConnTypElem::MeldeElementeAb(CDrawObj* pObj)
{
    POSITION pos = ConnElements.GetHeadPosition();

    while (pos != NULL)
    {
        POSITION p = pos;
        CConnect* Conn = (CConnect*) ConnElements.GetNext(pos);
        if (Conn->GetConnection() == pObj)
        {
            ConnElements.RemoveAt(p);
            delete Conn;
        }
    }
}

void CConnTypElem::SetDoc(CDrawDoc* D)
{
    ASSERT_VALID(D);
    POSITION pos = ConnElements.GetHeadPosition();
    while (pos != NULL)
    {
        ((CConnect*) ConnElements.GetNext(pos))->SetDoc(D);
    }
}


void CConnTypElem::InsertElement(CMConnect* pObj, CDrawDoc* pDoc)
{
    ASSERT_VALID(pDoc); // Fail (Copy)
    CConnect* Conn = new CConnect(pDoc); // <-- Fail von oben !!
    Conn->SetConnection(pObj);
    ConnElements.AddHead(Conn);
    selected = ConnElements.GetHeadPosition();
}

void CConnTypElem::DeleteElement(CMConnect* pObj)
{
    if (pObj != NULL) ConnElements.RemoveAt(ConnElements.Find(pObj));
}

void CConnTypElem::DeleteElement(WORD& i)
{
}

void CConnTypElem::MoveConnections(const CRect& rect, const CRect& OldPos)
{
    POSITION pos = ConnElements.GetHeadPosition();
    int I = ConnElements.GetCount();

    CRect r;
    CPoint p1, p2;
    CPoint delta;
    CDrawObj* pObj, *pBObj;
    while (pos != NULL)
    {
        CConnect* Conn = (CConnect*) ConnElements.GetNext(pos);

        switch (MT_) {
            case MT_MoveRel:
                                pObj = Conn->GetConnection();

                                if (Moved.Find(pObj) != NULL) break;

                                ASSERT_VALID(pObj);
                                r = pObj->GetRect();
                                p1 = pObj->GetPos(rect);
                                p2 = pObj->GetPos(OldPos);

                                delta = (CPoint)(p1 - p2);

                                r += delta;

                                pObj->MoveTo(r);
                                break;
            case MT_OnlySelf:
                                break;
            case MT_ChangeSize:
                                // pObj ist zu verändern
                                pObj = Conn->GetConnection();
                                ASSERT_VALID(pObj);
                                
                                // pBObj ist anderes Objekt für Rect
                                pBObj = pObj->GetOtherTarget((CDrawObj*) this);
                                if (pBObj == NULL) break;

                                r.UnionRect(rect,   //pObj->m_position,
                                            pBObj->GetRect());
                                Conn->GetConnection()->MoveTo(r);// , TRUE
                                break;
            default:           AfxMessageBox(CString("MT - Wert nicht definiert!"));
                                break;                    
        }
    }
}

BOOL CConnTypElem::SearchDeleteAbleObjects(CObList* pList, CObList* pProblemList)
{
    POSITION pos = ConnElements.GetHeadPosition();
    BOOL DeleteAble = TRUE;
    while (pos != NULL)
    {
        CConnect* Conn = (CConnect*) ConnElements.GetNext(pos);
        if (!Conn->GetConnection()->SearchDeleteAbleObjects(pList, pProblemList))
            DeleteAble = FALSE;
    }
    return DeleteAble;
}

void CConnTypElem::If_Exsist_pObj_then_Delete(CDrawObj* pObj)
{
    POSITION pos = ConnElements.GetHeadPosition();

    while (pos != NULL)
    {
        POSITION p = pos;
        CConnect* Conn = (CConnect*) ConnElements.GetNext(pos);

        if (Conn->GetConnection() == pObj)
        {
            ConnElements.RemoveAt(p);
            delete Conn;
        }
    }
}

void CConnTypElem::CutConnections(CDrawObj* pObj) // Achtung! pObj - pDrawObj
// Verbindung zu dem übergebenen Objekt (pObj) löschen.
{
    POSITION pos = ConnElements.GetHeadPosition();

    while (pos != NULL)
    {
        CConnect* Conn = (CConnect*) ConnElements.GetNext(pos);
        ASSERT_VALID(Conn);
        ASSERT(Conn->IsKindOf(RUNTIME_CLASS(CConnect)));
        CDrawObj* pDrawObj = Conn->GetConnection();

        ASSERT_VALID(pDrawObj);

        pDrawObj->If_Exsist_pObj_then_Delete(pObj);
    }
}

void CConnTypElem::DeleteGruppe()
{
    while(!ConnElements.IsEmpty())
    {
        CConnect* Conn = (CConnect*) ConnElements.RemoveHead();
        CDrawObj* pObj = Conn->GetConnection();
        delete Conn;
    }
}

void CConnTypElem::RemoveConnectionsAndReDraw(CDrawDoc* pDoc)
{
    if (Moved.Find(this) == NULL)
    {
        Moved.AddHead(this);
        while (!ConnElements.IsEmpty())
        {
            CConnect* Conn = (CConnect*) ConnElements.RemoveHead();
            ASSERT_VALID(pDoc);
            CDrawObj* pObj = Conn->GetConnection();
            ASSERT_VALID(pObj);

            if (pDoc->GetDeletedObjects()->Find(pObj) != NULL)
            {
                POSITION pos = pDoc->GetFirstViewPosition();

                AfxMessageBox("Objekt schon gelöscht");

                /*while (pos != NULL)
                    ((CBaseView*)pDoc->GetNextView(pos))->Remove(pObj);
                
                pos = pDoc->GetFirstViewPosition();

                while (pos != NULL)
                    ((CBaseView*)pDoc->GetNextView(pos))->InvalObj(pObj);*/
            }
            else
            {
                pDoc->Remove(pObj, pObj->FindHomeView());

                ASSERT(pDoc->GetDeletedObjects()->Find(pObj) != NULL);

                POSITION pos = pDoc->GetFirstViewPosition();

                while (pos != NULL)
                {
                    CBaseView* v = (CBaseView*) pDoc->GetNextView(pos);
                    if (pObj->GetViewList()->Find(v) != NULL) v->Remove(pObj);
                }
                
                pos = pDoc->GetFirstViewPosition();

                while (pos != NULL)
                {
                    CBaseView* v = (CBaseView*) pDoc->GetNextView(pos);
                    if (pObj->GetViewList()->Find(v) != NULL) v->InvalObj(pObj);
                }

                ASSERT_VALID(Conn->GetConnection());

                if (pDoc->GetDeletedObjects()->Find(pObj) == NULL) delete pObj;
                delete Conn;
            }
        }
        Moved.RemoveAt(Moved.Find(this));
    }
}

CDrawObj* CConnTypElem::GetElem()
{
    CConnect* Conn = (CConnect*) ConnElements.GetNext(selected);
    if (selected == NULL) selected = ConnElements.GetHeadPosition();
    return Conn->GetConnection();
}

void CConnTypElem::Serialize(CArchive& ar)
{
    ASSERT_VALID(this);
    CObject::Serialize(ar);
    if (ar.IsStoring())
    {
        ar << Typ;
        ar << (WORD) MT_;
    }
    else
    {
        // get the document back pointer from the archive
        pHome = (CDrawDoc*)ar.m_pDocument;
        ASSERT_VALID(pHome);
        ASSERT(pHome->IsKindOf(RUNTIME_CLASS(CDrawDoc)));


        WORD wTemp;
        ar >> Typ;
        ar >> wTemp; MT_ = (MT_Mode) wTemp;
    }

    // Muß Nummern abspeichern, Pointer gehen nicht!
    ConnElements.Serialize(ar);
    selected = ConnElements.GetHeadPosition();
}

IMPLEMENT_SERIAL(CMConnect, CObject, 0)

CMConnect::CMConnect()
{
    m_pDocument = NULL;
}

CMConnect::CMConnect(CDrawDoc* pConnHome)
{
    ASSERT_VALID(pConnHome);
    m_pDocument = NULL;
}

CMConnect::~CMConnect()
{
}

void CMConnect::Serialize(CArchive& ar)
{
    ASSERT_VALID(this);
    CObject::Serialize(ar);

    if (ar.IsStoring())
    {
        ConnTypList.Serialize(ar);
    }
    else
    {
        // get the document back pointer from the archive
        m_pDocument = (CDrawDoc*)ar.m_pDocument;
        ASSERT_VALID(m_pDocument);
        ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDrawDoc)));

        ConnTypList.Serialize(ar);
    }
}

void CMConnect::SetConnectionTyp(CString ConnTyp)
{
    POSITION pos = ConnTypList.GetHeadPosition();

    while (pos != NULL)
    {
        POSITION p = pos;
        CConnTypElem* Conn = (CConnTypElem*) ConnTypList.GetNext(pos);
        if (Conn->GetTyp() == ConnTyp)
        {
            selected = p;
            return;
        }
    }

    if (pos == NULL)
    {
        CConnTypElem* Conn = new CConnTypElem();

        Conn->SetTyp(ConnTyp);

        ConnTypList.AddHead(Conn);
        selected = ConnTypList.GetHeadPosition();
    }
}

void CMConnect::SetMaxConnections(int I)
{
    CConnTypElem* Conn = (CConnTypElem*) ConnTypList.GetAt(selected);
    Conn->SetMax(I);
}

BOOL CMConnect::SetActualConnection(CString ConnTyp)
{
    POSITION pos = ConnTypList.GetHeadPosition();

    while (pos != NULL)
    {
        POSITION p = pos;
        CConnTypElem* Conn = (CConnTypElem*) ConnTypList.GetNext(pos);
        if (Conn->GetTyp() == ConnTyp)
        {
            selected = p;
            return TRUE;
        }
        selected = NULL;
    }
    return FALSE;
}

void CMConnect::SetConnection(CMConnect* pObj)
{
    ASSERT_VALID(this);
    CConnTypElem* Conn = (CConnTypElem*) ConnTypList.GetAt(selected);
    Conn->InsertElement(pObj, pObj->m_pDocument);
}

CDrawObj* CMConnect::GetPropertyObj(CString S, int i)
// Property und Connection noch nicht richtig getrennt!!
{
    ASSERT_VALID(this);

    // Bestehen Verbindungen zu anderen Objekten in dieser Verbindungsart ??
    if (!SetActualConnection(S))
    {
        AfxMessageBox("Objekt hat keine Verbindung vom Typ ´" + S + "´");
        return NULL;
    }

    if (GetConnCount() == i)
    {
        return (CDrawObj*) GetConnection();
    }
    else
    {
        AfxMessageBox("Fehler in GetPropertyObj entdeckt !!/n Anzahl der Verbindungen berschritten.", i);
    }
    return NULL;
}

int CMConnect::GetConnCount()
{
    ASSERT_VALID(this);
    CConnTypElem* Conn = (CConnTypElem*) ConnTypList.GetAt(selected);
    return Conn->GetCount();
}

CMConnect* CMConnect::GetConnection(CString Typ)
{
    SetActualConnection(Typ);
    return GetConnection();
}

CMConnect* CMConnect::GetConnection()
{
    ASSERT_VALID(this);
    CConnTypElem* Conn = (CConnTypElem*) ConnTypList.GetAt(selected);
    return Conn->GetElem();
}

CDrawObj* CMConnect::Find(CDrawObj* pObj)
{
    ASSERT_VALID(this);
    return ((CConnTypElem*) (ConnTypList.GetAt(selected)))->FindObj(pObj);
}

CDrawObj* CMConnect::GetOtherTarget(CDrawObj* pObj)
{
    POSITION pos = ConnTypList.GetHeadPosition();
    CDrawObj* p;
    while (pos != NULL)
    {
        CConnTypElem* Conn = (CConnTypElem*) ConnTypList.GetNext(pos);

        p = Conn->Find(pObj);
        if (p != NULL) return p;
    }
    return NULL;
}

WORD CMConnect::GetConnectionNumber()
{
    ASSERT_VALID(this);
    CConnTypElem* Conn = (CConnTypElem*) ConnTypList.GetAt(selected);
    CDrawObj* pObj = Conn->GetElem();
    return pObj->GetNumber();
}

void CMConnect::SetMoveTyp(MT_Mode Mode)
{
    ASSERT_VALID(this);
    CConnTypElem* Conn = (CConnTypElem*) ConnTypList.GetAt(selected);
    Conn->SetMoveTyp(Mode);
}

void CMConnect::SetDrawTyp(DT_Mode Mode)
{
    ASSERT_VALID(this);
    m_Mode = Mode;
}


BOOL CMConnect::SearchDeleteAbleObjects(CObList* pList, CObList* pProblemList)
{
    BOOL DeleteAble = TRUE;
    if (Visited.Find(this) == NULL)
    {
        Visited.AddHead(this);
        
        POSITION pos = ConnTypList.GetHeadPosition();
        
        while (pos != NULL)
        {
            CConnTypElem* Conn = (CConnTypElem*) ConnTypList.GetNext(pos);

            if (Conn->GetTyp() == "CT_KanteKnoten")
            {
                if (pList->Find(this) == NULL)
                   pList->AddHead(this);
            }
            if (Conn->GetTyp() == "CT_KnotenKante")
            {
                DeleteAble = Conn->SearchDeleteAbleObjects(pList, pProblemList);
            }
            if (Conn->GetTyp() == "CT_ClientServer")
            {
                DeleteAble = Conn->SearchDeleteAbleObjects(pList, pProblemList);
                if ((DeleteAble) && (pList->Find(this) == NULL))
                    pList->AddHead(this);
            }
            if (Conn->GetTyp() == "CT_ServerClient")
            {
                DeleteAble = Conn->SearchDeleteAbleObjects(pList, pProblemList);
                if ((DeleteAble) && (pList->Find(this) == NULL))
                    pList->AddHead(this);
            }
  // Hat nichts mit Programmende - Fehler zu tun
            if (Conn->GetTyp() == "CT_BahnhofnameKnoten")
            {
                DeleteAble = Conn->SearchDeleteAbleObjects(pList, pProblemList);
                if ((DeleteAble) && (pList->Find(this) == NULL))
                    pList->AddHead(this);
            }
            if (Conn->GetTyp() == "CT_KnotenBahnhofname")
            {
                DeleteAble = Conn->SearchDeleteAbleObjects(pList, pProblemList);
                if ((DeleteAble) && (pList->Find(this) == NULL))
                    pList->AddHead(this);
            }

        }
        Visited.RemoveAt(Visited.Find(this));
        
        return DeleteAble;
    }
    else return TRUE;
}

void CMConnect::CutConnections()
// Alle Verbindungen zu diesem Objekt löschen
{
    ASSERT_VALID(this);
    while (!ConnTypList.IsEmpty())
    {
        CConnTypElem* Element = (CConnTypElem*) ConnTypList.RemoveHead();

        ASSERT_VALID(Element);
        ASSERT(Element->IsKindOf(RUNTIME_CLASS(CConnTypElem)));

        Element->CutConnections((CDrawObj*) this);

        ASSERT_VALID(Element);
        delete Element;
    }
    ASSERT(ConnTypList.IsEmpty());
}

void CMConnect::If_Exsist_pObj_then_Delete(CDrawObj* pObj)
{
    POSITION pos = ConnTypList.GetHeadPosition();

    while (pos != NULL)
    {
        CConnTypElem* Conn = (CConnTypElem*) ConnTypList.GetNext(pos);
        ASSERT_VALID(Conn);
        ASSERT(Conn->IsKindOf(RUNTIME_CLASS(CConnTypElem)));
        Conn->If_Exsist_pObj_then_Delete(pObj);
    }

    pos = ConnTypList.GetHeadPosition();

    int Anzahl = ConnTypList.GetCount();
    int Count = 0;


    while (pos != NULL)
    {
        CConnTypElem* Conn = (CConnTypElem*) ConnTypList.GetNext(pos);
        ++Count;
        if (Conn->IsEmpty())
        {
            ConnTypList.RemoveAt(ConnTypList.Find(Conn));
        }
    }
    if (Count != Anzahl)
    {
        char buf[100];

        sprintf(buf, "RemoveAt geht nicht: (Anzahl %d, Count %d)", Anzahl, Count);
        
        AfxMessageBox(buf);
    }
}

void CMConnect::RemoveConnectionsAndReDraw()
{
    if (Moved.Find(this) == NULL)  // Verhindert kreisförmige Aufrufe (Ringverbindungen)
    {
        Moved.AddHead(this);

        while (!ConnTypList.IsEmpty())
        {
            CConnTypElem* Conn = (CConnTypElem*) ConnTypList.GetHead();
            ASSERT_VALID(m_pDocument);
            Conn->RemoveConnectionsAndReDraw(m_pDocument);
            ConnTypList.RemoveHead(); 
            ASSERT_VALID(Conn);
            ASSERT(Conn->IsKindOf(RUNTIME_CLASS(CConnTypElem)));
            delete Conn;
        }
        Moved.RemoveAt(Moved.Find(this));
    }
}

void CMConnect::RemoveConnections()
{
    while (!ConnTypList.IsEmpty())
    {
        delete ConnTypList.RemoveHead(); // Objekt übernimmt Aufräumarbeiten
    }
}

void CMConnect::RemoveConnections(CDrawDoc* pDoc)
{
    while (!ConnTypList.IsEmpty())
    {
        delete ConnTypList.RemoveHead(); // Objekt übernimmt Aufräumarbeiten
    }
}

void CMConnect::UpDateConnections(CDrawDoc* pDoc)
{
}

void CMConnect::MeldeElementeAb(CString S, CDrawObj* pObj)
{
    POSITION pos = ConnTypList.GetHeadPosition();
    while (pos != NULL)
    {
        CConnTypElem* Conn = (CConnTypElem*) ConnTypList.GetNext(pos);
        if ((S == "CT_AllMode") || (S == Conn->GetTyp()))
            Conn->MeldeElementeAb(pObj);
    }
}

void CMConnect::DeleteGruppe(CString S)
{
    POSITION pos = ConnTypList.GetHeadPosition();
    while (pos != NULL)
    {
        CConnTypElem* Conn = (CConnTypElem*) ConnTypList.GetNext(pos);
        if (Conn->GetTyp() == S) Conn->DeleteGruppe();
    }
}

CRect CMConnect::GetNewRect()
{
    CRect rect;
    return rect;
}

void CMConnect::MoveConnections(const CRect& rect, const CRect& OldPos, CString NotToMove)
{
    if (Moved.Find(this) == NULL)  // Verhindert kreisförmige Aufrufe (Ringverbindungen)
    {
        Moved.AddHead(this);
        POSITION pos = ConnTypList.GetHeadPosition();
        while (pos != NULL)
        {
            CConnTypElem* Conn = (CConnTypElem*) ConnTypList.GetNext(pos);
            ASSERT_VALID(Conn);
            if (Conn->GetTyp() != NotToMove)
            {
                Conn->MoveConnections(rect, OldPos);
            }
        }
        Moved.RemoveAt(Moved.Find(this));
    }
}

void CMConnect::SetDoc(CDrawDoc* D)
{
    ASSERT_VALID(D);
    m_pDocument = D;

    POSITION pos = ConnTypList.GetHeadPosition();
    while (pos != NULL)
    {
        ((CConnTypElem*) ConnTypList.GetNext(pos))->SetDoc(D);
    }
    
}

CDrawDoc* CMConnect::GetDoc()
{
    ASSERT_VALID(m_pDocument);
    return m_pDocument;
}

void CMConnect::Connect(CMConnect* pObj1, CMConnect* pObj2,
                        MT_Mode mode1,   MT_Mode mode2,
                        CString S1,      CString S2)
{
        pObj2->SetConnectionTyp(S2);
        pObj2->SetMoveTyp(mode2);
        pObj2->SetConnection(pObj1);

        pObj1->SetConnectionTyp(S1);
        pObj1->SetMoveTyp(mode1);
        pObj1->SetConnection(pObj2);
}

void CMConnect::Disconnect(CMConnect* pObj, CString S1)
{
    POSITION pos = ConnTypList.GetHeadPosition();
    while (pos != NULL)
    {
        CConnTypElem* Conn = (CConnTypElem*) ConnTypList.GetNext(pos);
        if (Conn->GetTyp() == S1)
        {
            if (Conn->GetCount() > 1)
            {
                AfxMessageBox("Sitzplatz kann nur von einer Person besetzt sein!");
                ASSERT(FALSE);
            }
            ASSERT(FALSE);
        }
    }
}          


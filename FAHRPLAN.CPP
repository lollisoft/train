// Implementation des Tabellen - Objekts

#include "stdafx.h"
#include "train.h"
#include "persist.h"
#include "mainfrm.h"
#include "pageserv.h"
#include "drawdoc.h"

#include "basevw.h"
#include "pagevw.h"
#include "tabvw.h"

#include "drawobj.h"
#include "drawtool.h"
#include "mconnect.h"

#include "cntritem.h"
#include "rectdlg.h"

#include "tabobj.h"
#include "node.h"
#include "dataelem.h"
#include "fahrplan.h"

#include <io.h>
#include <errno.h>

#include <math.h>
#include <direct.h>
#include <limits.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//#pragma code_seg(DrawObj_seg);

IMPLEMENT_SERIAL(CFahrplanObj, CTabelleObj, 0)

CFahrplanObj::CFahrplanObj()
{
}

CFahrplanObj::CFahrplanObj(CPoint P, int Zeilen, int Spalten, CDrawDoc* pDoc)
           :CTabelleObj(P, Zeilen, Spalten, pDoc)
{
}

CFahrplanObj::~CFahrplanObj()
{
}

void CFahrplanObj::Serialize(CArchive& ar)
{
    ASSERT_VALID(this);
    
    CTabelleObj::Serialize(ar);

    Haltepunkte.Serialize(ar);
}

void CFahrplanObj::SetSpaltenName(CDataElem* pObj)
{
    // Der Bahnhof - Name ist eine Eigenschaft des Bahnhof´s
    // Der Name steht allerdings in einem separatem CDrawObjekt (Abgeleitet)
    
    CDrawNode* pNode = (CDrawNode*) pObj->GetPropertyObj("CT_BahnhofnameKnoten", 1);

    ASSERT_VALID(pNode);

    POSITION pos = Haltepunkte.Find(pNode);

    if (pos != NULL)
    {
        POSITION pos = Haltepunkte.GetHeadPosition();

        int i = - 1;

        while (pos != NULL)
        {
            CDrawNode* pNodeI = (CDrawNode*) Haltepunkte.GetNext(pos);
            if (pNodeI == pNode) pos = NULL;
            i++;
        }

        // Index i...
        if (Haltepunkte.GetCount() > i)
        {
            CRect rect = ((CDataElem*)GetElement(0,i))->GetRect();
            SetElementSizes(0, i, rect.Height(), rect.Width());
            CalcRect();
            Invalidate();
        }
    }
    else
    {
        AfxMessageBox("Bahnhof nicht in Fahrplan enthalten");
    }
}

BOOL CFahrplanObj::AddHaltepunkt(int s, CDrawObj* pObj)
{
    ASSERT_VALID(this);

    if (Haltepunkte.Find(pObj) != NULL)
    {
        if(AfxMessageBox("Ende der Eingabe ?", MB_YESNO|MB_ICONQUESTION) == IDYES)
        {
            return FALSE;
        }

        CDrawNode* pNode = (CDrawNode*) Haltepunkte.GetTail();

        if (pNode->FindKanteZuKnoten((CDrawNode*) pObj) != NULL)
        {
            Haltepunkte.AddTail(pObj);
        }
        else
        {
            AfxMessageBox("Keine Verbindung zum vorherigen Haltepunkt gefunden");
        }
    }

    if (Haltepunkte.GetCount() >= 1)
    {
        CDrawNode* pNode = (CDrawNode*) Haltepunkte.GetTail();

        if (pNode->FindKanteZuKnoten((CDrawNode*) pObj) != NULL)
        {
            Haltepunkte.AddTail(pObj);
        }
        else
        {
            AfxMessageBox("Keine Verbindung zum vorherigen Haltepunkt gefunden");
        }
    }
    else
    {
        Haltepunkte.AddTail(pObj);
    }
    // Von Bahnhof den Namen holen

    if (!pObj->SetActualConnection(CString("CT_KnotenBahnhofname")))
    {
        AfxMessageBox("Bahnhof hat noch keinen Namen!!");
        ASSERT(FALSE);
    }

    if (pObj->GetConnCount() == 1) // Nur ein Name erlaubt.
    {
        CDataElem* pName = (CDataElem*) pObj->GetConnection();

        // Clone richtig anwenden !!!
        // Kopie für Fahrplan
        CDataElem* pClone = (CDataElem*) pName->Clone(NULL, NULL);

        

        CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
        CBaseView* pTabView = (CBaseView*) pFrame->FindView(RUNTIME_CLASS(CTabView)); 

        ASSERT_VALID(pTabView);

        pClone->SetAngle(900);
   
        pTabView->GetDocument()->Add(pClone, pTabView);
        pClone->CalcRect(&CClientDC(pTabView));
        pClone->Invalidate();

        // Objekte noch verbinden:

        // "CT_SameData" bedeutet, daß die Daten nach Änderungen immer abzugleichen sind.
        
        // pClone ---> pName : pName darf nicht mitgelöscht werden.
        //                     Eigentlich kann pClone auch nicht gelöscht werden,
        //                     da der Zug den Bahnhof doch passieren wird.
        // ---> Spalte darf nicht gelöscht werden !!

        // pName ---> pClone : Soll ein Bahnhof in NetEd gelöscht werden, so
        //                     darf er nur gelöscht werden, wenn alle betroffenen
        //                     Fahrpläne dies erlauben.

        // Da der Name in dem Fahrplan der Gleiche ist wie am Knoten, muß bei Änderungen
        // des Namens der andere auch geändert werden (automatisch).

        // CheckForAutoUpdate

        pClone->SetConnectionTyp("CT_SameData");
        pClone->SetMoveTyp(MT_OnlySelf);
        pClone->SetConnection(pName);

        pName->SetConnectionTyp("CT_SameData");
        pName->SetMoveTyp(MT_OnlySelf);
        pName->SetConnection(pClone);


        Connect(this,                      pClone,
                MT_MoveRel,                MT_MoveRel,
                "CT_Fahrplan_SpaltenName", "CT_SpaltenName_Fahrlan");
/*                
        SetConnectionTyp("CT_Fahrplan_SpaltenName");
        SetMoveTyp(MT_MoveRel);
        SetConnection(pClone);

        pClone->SetConnectionTyp("CT_SpaltenName_Fahrplan");
        pClone->SetMoveTyp(MT_MoveRel);
        pClone->SetConnection(this);
*/

        InsertSpalte(s, NULL);

        SetElement(pClone, 0,s);
        SetSpaltenName(pName); // Setzt die Größe des Elements mit
        pClone->SetRect(GetClientRect(0, s)); // Zeile, Spalte // Fehler

        RecalcAllPositions();
    }
    else
    {
        AfxMessageBox("Bahnhof kann nur einen Namen haben!");
    }
    

    return TRUE;
}

void CFahrplanObj::ShowHiddenNormal()
// Gew„hlte Strecke wieder normal anzeigen
{
    POSITION pos = Haltepunkte.GetHeadPosition();

    CDrawNode* pObj = (CDrawNode*) Haltepunkte.GetNext(pos);

    while (pos != NULL)
    {
        CDrawNode* pNextObj = (CDrawNode*) Haltepunkte.GetNext(pos);

        (pObj->FindKanteZuKnoten(pNextObj))->ShowNormal();
        pObj = pNextObj;
    }
}

void CFahrplanObj::ShowHiddenSelected()
// Zur visuellen Anzeige der gew„hlten Strecke
{
    POSITION pos = Haltepunkte.GetHeadPosition();

    CDrawNode* pObj = (CDrawNode*) Haltepunkte.GetNext(pos);

    while (pos != NULL)
    {
        CDrawNode* pNextObj = (CDrawNode*) Haltepunkte.GetNext(pos);

        (pObj->FindKanteZuKnoten(pNextObj))->ShowSelected();
        pObj = pNextObj;
    }
}

// drawobj.cpp - implementation for drawing objects
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1993 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp and/or WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.


#include "stdafx.h"
#include "train.h"
#include "pageserv.h"
#include "drawdoc.h"
#include "basevw.h"
#include "pagevw.h"
#include "tabvw.h"
#include "mconnect.h"
#include "drawobj.h"
#include "kunden_d.h"
#include "wagen.h"

#include "cntritem.h"
#include "rectdlg.h"
#include <stdlib.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif




////////////////////////////////////////////////////////////////////////////
// CSitzplatz

IMPLEMENT_SERIAL(CSitzplatz, CDrawRect, 0)

CSitzplatz::CSitzplatz()
{
}

CSitzplatz::CSitzplatz(const CRect& position)
   :CDrawRect(position)
{
    ASSERT_VALID(this);

    m_oldpos = position;

}

CSitzplatz::~CSitzplatz()
{
    ASSERT_VALID(this);

    RemoveConnectionsAndReDraw(); // Mit Abmeldung...
}

void CSitzplatz::KillConnections()
{
    RemoveConnections(m_pDocument);
}

void CSitzplatz::Serialize(CArchive& ar)
{
    ASSERT_VALID(this);

    CDrawRect::Serialize(ar);
    if (ar.IsStoring())
    {
        ar << (WORD) m_nShape;
        ar << (WORD) Netz;
    }
    else
    {
        WORD wTemp;

        ar >> wTemp; m_nShape = (Shape)wTemp;
        ar >> wTemp; Netz = (NetzTyp)wTemp;
    }
}


void CSitzplatz::Draw(CDC* pDC)
{
    ASSERT_VALID(this);
    ASSERT_VALID(m_pDocument);

    CBrush brush;
    if (!brush.CreateBrushIndirect(&m_logbrush))
       return;
    CPen pen;
    if (!pen.CreatePenIndirect(&m_logpen))
       return;

    CBrush* pOldBrush;
    CPen* pOldPen;

    if (m_bBrush)
            pOldBrush = pDC->SelectObject(&brush);
    else
            pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);

    if (m_bPen)
            pOldPen = pDC->SelectObject(&pen);
    else
            pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);

    CRect rect = GetRect();

    SetFillColor(besetzt ? m_Besetzt : m_Frei);

    pDC->Rectangle(rect);

    pDC->SelectObject(pOldBrush);
    pDC->SelectObject(pOldPen);
}

//  point is in logical coordinates
int CSitzplatz::HitTest(CPoint point, CBaseView* pView, BOOL bSelected)
{
        ASSERT_VALID(this);
        ASSERT(pView != NULL);

        return CDrawRect::HitTest(point, pView, bSelected);
}

// position is in logical
void CSitzplatz::MoveTo(const CRect& position, BOOL NurSelbst)
{
        ASSERT_VALID(this);

        CRect OldPos = GetRect();
        if (position == GetRect())
                return;
        SetRect(InflatedRect(OldPos));
        UpDateViews();
        SetRect(InflatedRect(position));
        UpDateViews();
        SetRect(position);

        m_pDocument->SetModifiedFlag();

        // Der volgende Befehl bewegt alle angehängten Objekte
        // je nach MT - Option an die neue Position

        //if (pView->m_selection.Find(this) != NULL)
        MoveConnections(position, OldPos);
}


// point must be in logical
void CSitzplatz::MoveHandleTo(int nHandle, CPoint point)
{
    ASSERT_VALID(this);

    CRect position = GetRect();
    MoveTo(position);
}

HCURSOR CSitzplatz::GetHandleCursor(int nHandle)
{
        ASSERT_VALID(this);

        LPCSTR id;
        switch (nHandle)
        {
        default:
                ASSERT(FALSE);

        case 1:
        case 5:
                id = IDC_SIZENWSE;
                break;

        case 2:
        case 6:
                id = IDC_SIZENS;
                break;

        case 3:
        case 7:
                id = IDC_SIZENESW;
                break;

        case 4:
        case 8:
                id = IDC_SIZEWE;
                break;
        }

        return AfxGetApp()->LoadStandardCursor(id);
}

void CSitzplatz::Besetze_mit(CKundenDatensatz* Kunde) // Kunde ist irgendwo gespeichert
{
    ASSERT_VALID(this);
    ASSERT_VALID(Kunde);
    besetzt = TRUE;

    Connect(this,                      Kunde,
            MT_OnlySelf,                MT_OnlySelf,
            "CT_Sitzplatz_Kunde", "CT_Kunde_Sitzplatz");
}
void CSitzplatz::Freigeben()
{
    ASSERT_VALID(this);    

    if (!besetzt)
    {
        return;
    }

    // this kann nur einen Kunden haben!! (Von - Zu)
    Disconnect(this, "CT_Sitzplatz_Kunde");

    besetzt = FALSE;
}

CKundenDatensatz* CSitzplatz::GetKunde()
{
    ASSERT_VALID(this);
    return (CKundenDatensatz*) GetConnection("CT_SitzPlatz_Kunde");
}


////////////////////////////////////////////////////////////////////////////
// CSitzreihe

IMPLEMENT_SERIAL(CSitzreihe, CTabelleObj, 0)


CSitzreihe::CSitzreihe()
{
}

CSitzreihe::~CSitzreihe()
{
}

void CSitzreihe::Serialize(CArchive& ar)
{
    ASSERT_VALID(this);

    CTabelleObj::Serialize(ar);

    if (ar.IsStoring())
    {
    }
    else
    {
    }
}

BOOL CSitzreihe::BesetzeUmgebung(CGruppeObj* pGruppe, int rest)
{
    // BesetzeRest versucht den Rest der Kunden in der Nähe der anderen Kunden aus der
    // gleichen Gruppe zu platzieren. 
    
}

void CSitzreihe::BesetzeGruppe(CGruppeObj* pGruppe)
{
    // BesetzeGruppe kann von der Sitzgruppe aufgerufen werden, wenn nach der
    // Methode "Topdown" vorgegangen wird. D.h. der Bediener versucht eine
    // Gruppe im Zug unter zu bringen der dann und dann fährt.

    // Wird aber ein Teil der Sitzreihe selektiert und die Anzahl der Kunden ist
    // größer als die Kapazität der Sitzreihe, sollte nicht einfach eine Fehler-
    // meldung wie diese zurück gegeben werden: "Sitzreihe kann diese Gruppe nicht aufnehmen!"
    // Vielmehr sollte Sie die Umgebung in diesem Fall abklopfen, um festzustellen, ob
    // die Gruppe mit auf den Nachbarsitzen verteilt werden kann. Wenn ja, dann
    // ist das Problem gelöst.
    // Ist in der Umgebung aber nicht so viel Platz, dann könnte man vorschlagen, daß im Wagen
    // an einer anderen Stelle noch  platz währe.


    
    ASSERT_VALID(this);

    if (pGruppe->GetAnzahl() > GetAnzahl())
    // Mehr angefordert...
    {
        int rest = pGruppe->GetAnzahl() - GetAnzahl();

        if (BesetzeUmgebung(pGruppe, rest))
        {
            // Nur wenn der Rest einen Sitzplatz in der Umgebung bekommen hat,
            // ist diese Sitzreihe und Ihre Umgebung geeignet für diese Gruppe

            POSITION pos       = m_Sitzreihe.GetHeadPosition();
            POSITION gruppenpos = pGruppe->GetHeadPosition();
    
            while (pos != NULL)
            {
                CSitzplatz* pSitzplatz = (CSitzplatz*) m_Sitzreihe.GetNext(pos);
    
                pSitzplatz->Besetze_mit(pGruppe->GetNext(gruppenpos));
            }
        }
        else // Umgebung reicht nicht aus für die Gruppe!
        {
            CWagen* wagen = GetConnection("CT_Sitzreihe_Sitzgruppe")->
                            GetConnection("CT_Sitzgruppe_Wagen");

            if (wagen->HasPlacesFor(pGruppe))
            {// Im Wagen platzierbar...
            }
            else
            {
                CZug* zug = wagen->GetConnection("CT_Wagen_Zug");
                
            }
        }

    }
    else
    {
        POSITION pos       = m_Sitzreihe.GetHeadPosition();
        POSITION gruppenpos = pGruppe->GetHeadPosition();

        while (gruppenpos != NULL)
        {
            CSitzplatz* pSitzplatz = (CSitzplatz*) m_Sitzreihe.GetNext(pos);

            pSitzplatz->Besetze_mit(pGruppe->GetNext(gruppenpos));
        }

    }
}

void CSitzreihe::GebeGruppeFrei()
{
}


////////////////////////////////////////////////////////////////////////////
// CSitzgruppe

IMPLEMENT_SERIAL(CSitzgruppe, CSitzreihe, 0)

CSitzgruppe::CSitzgruppe()
{
}

CSitzgruppe::~CSitzgruppe()
{
}

void CSitzgruppe::Serialize(CArchive& ar)
{
    ASSERT_VALID(this);

    CTabelleObj::Serialize(ar);

    if (ar.IsStoring())
    {
    }
    else
    {
    }
}

void CSitzgruppe::BesetzeGruppe(CGruppeObj* pGruppe)
{
}

void CSitzgruppe::GebeGruppeFrei()
{
}

//////////////////////////////////
// CWagen

IMPLEMENT_SERIAL(CWagen, CTabelleObj, 0)

CWagen::CWagen()
{
}

CWagen::~CWagen()
{
}

void CWagen::Serialize(CArchive& ar)
{
}

void CWagen::DrawOutlineFrame(CDC* pDC)
{
    ASSERT_VALID(this);

    POSITION pos = m_ShapePoints.GetHeadPosition();

    CPoint* pa = (CPoint*) m_ShapePoints.GetNext(pos);
    CPoint* p1 = pa;
    CPoint* p2;

    while (pos != NULL)
    {// Verbinden der Punkte...
        p2 = (CPoint*) m_ShapePoints.GetNext(pos);

        pDC->MoveTo(p1->x, p1->y);
        pDC->LineTo(p2->x, p2->y);
    }
    pDC->MoveTo(p2->x, p2->y);
    pDC->LineTo(pa->x, pa->y);
}

void CWagen::InitUsedColorsEtc()
{
}

void CWagen::Draw(CDC*  pDC)
{
    ASSERT_VALID(this);

    InitUsedColorsEtc();

    DrawOutlineFrame(pDC);

    // Nicht die üblichen Zeilen- und Spaltenlinien zeichnen
}

void CWagen::AddSitzgruppe(int x, int y)
{
}

void CWagen::ChooseEckform()
{
}

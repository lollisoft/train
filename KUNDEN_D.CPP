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

#include "cntritem.h"
#include "rectdlg.h"
#include <stdlib.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif




////////////////////////////////////////////////////////////////////////////
// CKundenDatensatz


IMPLEMENT_SERIAL(CKundenDatensatz, CMConnect, 0)

CKundenDatensatz::CKundenDatensatz()
{
}

void CKundenDatensatz::EditDatensatz()
{
}

void CKundenDatensatz::KillConnections()
{
}

void CKundenDatensatz::SetKundenName(CString V, CString N)
{
    VorName = V;
    NachName = N;
}

CString CKundenDatensatz::GetKundenName(BOOL FirstVorname)
{
    CString S;
    if (FirstVorname)
        S = VorName + ", " + NachName;
    else
        S = NachName + ", " + VorName;
        
    return S;
}

void CKundenDatensatz::SetAlter(int a)
{
    alter = a;
}

int CKundenDatensatz::GetAlter()
{
    return alter;
}

void CKundenDatensatz::SetWeiblich(BOOL w)
{
    weiblich = w;
}

BOOL CKundenDatensatz::IsWeiblich()
{
    return weiblich;
}

void CKundenDatensatz::SetGruppenStatus(BOOL g)
{
    gruppe = g;
}

BOOL CKundenDatensatz::GetGruppenStatus()
{
    return gruppe;
}

void CKundenDatensatz::Serialize(CArchive& ar)
{
    ASSERT_VALID(this);
    CMConnect::Serialize(ar);

    if (ar.IsStoring())
    {
        ar.Write(&gruppe, sizeof(gruppe));
        ar.Write(&weiblich, sizeof(weiblich));
        ar.Write(&alter, sizeof(alter));
        ar << VorName << NachName;
    }
    else
    {
        ar.Read(&gruppe, sizeof(gruppe));
        ar.Read(&weiblich, sizeof(weiblich));
        ar.Read(&alter, sizeof(alter));
        ar >> VorName >> NachName;
    }
}

////////////////////////////////////////////////////////////////////////////
// CGruppeObj


IMPLEMENT_SERIAL(CGruppeObj, CMConnect, 0)

CGruppeObj::CGruppeObj()
{
}

void CGruppeObj::EditDatensatz()
{// Dialog mit Kundenauswahl
}

void CGruppeObj::KillConnections()
{
}

void CGruppeObj::Serialize(CArchive& ar)
{
    ASSERT_VALID(this);
    CMConnect::Serialize(ar);

    if (ar.IsStoring())
    {
    }
    else
    {
    }
}

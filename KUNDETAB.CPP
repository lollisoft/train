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
#include "kundetab.h"
#include "str_inp.h"

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

IMPLEMENT_SERIAL(CKundenTabObj, CTabelleObj, 0)

CKundenTabObj::CKundenTabObj()
{
}

CKundenTabObj::CKundenTabObj(CPoint P, int Zeilen, int Spalten, CDrawDoc* pDoc)
           :CTabelleObj(P, Zeilen, Spalten, pDoc)
{
}

CKundenTabObj::~CKundenTabObj()
{
}

void CKundenTabObj::Serialize(CArchive& ar)
{
    ASSERT_VALID(this);
    
    CTabelleObj::Serialize(ar);
}

void CKundenTabObj::DeleteKunde(CString Kunde)
{
    if (m_Zeilen > 1)
    {
        int i;
        for (i = 1; i < m_Zeilen; i++)
        {
            CDrawObj* pObj = GetElement(i,0);
            if (pObj != NULL)
                if (pObj->IsKindOf(RUNTIME_CLASS(CDataElem)))
                // ein Name gefunden
                {
                    CDataElem* pKunde = (CDataElem*) pObj;
                    if (Kunde == pKunde->GetElement())
                    {
                        DeleteKunde(i);
                        return;
                    }
                }
        }
    }
    AfxMessageBox("Kunde nicht enthalten!");
}
void CKundenTabObj::DeleteKunde()
{
    CStringDlg dlg;

    dlg.SetStringBeschreibung(CString("Name"));
    if (dlg.DoModal() == IDOK) DeleteKunde(dlg.m_String);
}
void CKundenTabObj::DeleteKunde(int z)
{
    DeleteZeile(z);
}
void CKundenTabObj::DeleteKunde(CDrawObj* pObj)
{
    if(pObj->IsKindOf(RUNTIME_CLASS(CDataElem)))
    {
        CDataElem* pKunde = (CDataElem*) pObj;
        DeleteKunde(pKunde->GetElement());
    }
}
void CKundenTabObj::AddKunde(CString Kunde)
{
    CDataElem* pKunde = new CDataElem(m_pDocument, Kunde, "");
    CBaseView* pView = (CBaseView*) m_ViewList.GetHead();
    pView->GetDocument()->Add(pKunde, pView);
    pKunde->CalcRect(&CClientDC(pView));
    pKunde->Invalidate();

    Connect(this,                         pKunde,
            MT_MoveRel,                    MT_MoveRel,
            "CT_Kundentabelle_Zeilentyp", "CT_Zeilentyp_Kundentabelle");
    InsertZeile(0, NULL); // m_Zeilen
    SetElement(pKunde, 0, 0); // m_Zeilen - 1, 0
    SetElementSizes(0, 0, pKunde->GetRect().Height(), // m_Zeilen - 1, 0
                                     pKunde->GetRect().Width());

                                     
}
void CKundenTabObj::AddSpalte(CString Typ, CString Bezeichnung)
{
    CDataElem* pSpaltenTyp = new CDataElem(m_pDocument, Bezeichnung, "String");
    CBaseView* pView = (CBaseView*) m_ViewList.GetHead();
    pView->GetDocument()->Add(pSpaltenTyp, pView);
    //pSpaltenTyp->SetAngle(900);
    pSpaltenTyp->CalcRect(&CClientDC(pView));
    pSpaltenTyp->Invalidate();
    Connect(this,                         pSpaltenTyp,
            MT_MoveRel,                    MT_MoveRel,
            "CT_Kundentabelle_Spaltentyp", "CT_Spaltentyp_Kundentabelle");
    InsertSpalte(m_Spalten, NULL);
    SetElement(pSpaltenTyp, 0, m_Spalten - 1);
    SetElementSizes(0, m_Spalten - 1, pSpaltenTyp->GetRect().Height(),
                                  pSpaltenTyp->GetRect().Width());
}
void CKundenTabObj::DeleteSpalte(CString Bezeichnung)
{
}
void CKundenTabObj::SelectSpalte(CString Bezeichnung)
{
}
void CKundenTabObj::SelectZeile(CString Bezeichnung, CString Eintrag)
{
}

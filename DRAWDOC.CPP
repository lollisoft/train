// drawdoc.cpp : implementation of the CDrawDoc class//
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

#include "basevw.h"
#include "pagevw.h"
#include "pageserv.h"

#include "drawdoc.h"
//#include "pagevw.h"
#include "tabvw.h"
#include "drawobj.h"
#include "mconnect.h"
#include "dataelem.h"
#include "tabobj.h"
#include "node.h"
#include "cntritem.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrawDoc

IMPLEMENT_DYNCREATE(CDrawDoc, COleDocument)

BEGIN_MESSAGE_MAP(CDrawDoc, COleDocument)
        //{{AFX_MSG_MAP(CDrawDoc)
        ON_COMMAND(ID_VIEW_PAPERCOLOR, OnViewPaperColor)
        //}}AFX_MSG_MAP
        // Enable default OLE container implementation
        ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, COleDocument::OnUpdatePasteMenu)
        ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_LINK, COleDocument::OnUpdatePasteLinkMenu)
        ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, COleDocument::OnUpdateEditLinksMenu)
        ON_COMMAND(ID_OLE_EDIT_LINKS, COleDocument::OnEditLinks)
        ON_UPDATE_COMMAND_UI(ID_OLE_VERB_FIRST, COleDocument::OnUpdateObjectVerbMenu)
        ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_CONVERT, COleDocument::OnUpdateObjectVerbMenu)
        ON_COMMAND(ID_OLE_EDIT_CONVERT, COleDocument::OnEditConvert)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawDoc construction/destruction

CDrawDoc::CDrawDoc()
{
        m_nMapMode = MM_ANISOTROPIC;
        m_paperColor = RGB(255, 255, 255);
        ComputePageSize();
        ::GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &m_LogfontStandart);
        m_PageServer.SetDoc(this);
}

CDrawDoc::~CDrawDoc()
{
        // Löst Verbindungen, wenn welche vorhanden sind
        m_PageServer.KillAnyConnectedObjects();
        // Löscht alle Daten        
        m_PageServer.DeleteContents();
        
        if (!m_deletedobjects.IsEmpty()) m_deletedobjects.RemoveAll();
}

BOOL CDrawDoc::OnSaveDocument(const char* pszPathName)
{
        // Upon saving the document, tell the application object
        // to save the path name in the private INI file.

        if (!COleDocument::OnSaveDocument(pszPathName))
                return FALSE;
        UpdateIniFileWithDocPath(pszPathName);
        return TRUE;
}


BOOL CDrawDoc::OnOpenDocument(const char* pszPathName)
{
        // Upon opening the document, tell the application object
        // to save the path name in the private INI file.

        if (!COleDocument::OnOpenDocument(pszPathName))
                return FALSE;
        UpdateIniFileWithDocPath(pszPathName);
        return TRUE;
}


BOOL CDrawDoc::OnNewDocument()
{
        if (!COleDocument::OnNewDocument())
                return FALSE;

        // TODO: add reinitialization code here
        // (SDI documents will reuse this document)

        return TRUE;
}



void CDrawDoc::ErrorMessage(LPCSTR msg)
{
    POSITION pos = GetFirstViewPosition();

    while (pos != NULL)
    {
        CBaseView* pView = (CBaseView*)GetNextView(pos);
        if (pView->m_bActive)
            pView->MessageBox(msg);
    }
}

/////////////////////////////////////////////////////////////////////////////
// CDrawDoc serialization

void CDrawDoc::Serialize(CArchive& ar)
{
        if (ar.IsStoring())
        {
                ar << m_paperColor;
                concount.Serialize(ar);
                ar.Write(&m_LogfontStandart, sizeof(m_LogfontStandart));
                ar.Write(&m_ActualKunden, sizeof(m_ActualKunden));
        }
        else
        {
                ar >> m_paperColor;
                concount.Serialize(ar);
                ar.Read(&m_LogfontStandart, sizeof(m_LogfontStandart));
                ar.Read(&m_ActualKunden, sizeof(m_ActualKunden));                
        }

        //m_PageServer.KillAnyConnectedObjects();
        m_PageServer.Serialize(ar);

        // By calling the base class COleDocument, we enable serialization
        //  of the container document's COleClientItem objects automatically.
        COleDocument::Serialize(ar);
}


/////////////////////////////////////////////////////////////////////////////
// CDrawDoc implementation

void CDrawDoc::Draw(CDC* pDC, CBaseView* pView)
{
    ASSERT_VALID(pView);
        //CObList* list = pView->GetPage();
          CObList* list = pView->GetPage();
        ASSERT_VALID(list);
        if (!list->IsKindOf(RUNTIME_CLASS(CObList)))
        {
            AfxMessageBox("Keine Liste!");
            if (list->IsKindOf(RUNTIME_CLASS(CPageServerElem)))
            {
                AfxMessageBox("Object ist PageServerElem!");
            }
        }
        POSITION pos = list->GetHeadPosition();
        while (pos != NULL)
        {
                CDrawObj* pObj = (CDrawObj*)list->GetNext(pos);

                // Bevor das Objekt gezeichnet wird, testen, ob es die View kennt !!!
                // (in aktuellem Pfad!!)

                if (pObj->FindView(pView) == NULL) pObj->SetView(pView);

                // Hier wird entschieden, ob sich das Objekt in einem Client zeichnen soll
                // z.B. in ein Tabellen - Element

                if (pObj->IsDrawInClient())
                    pObj->DrawInClient(pDC);
                else
                    pObj->Draw(pDC);

                if (pView->m_bActive && !pDC->IsPrinting() && pView->IsSelected(pObj))
                        pObj->DrawTracker(pDC, CDrawObj::selected);
        }
}

void CDrawDoc::AddHead(CDrawObj* pObj, CBaseView* pView)
{
    pView->GetPage()->AddHead(pObj);
    pObj->SetDoc(this);
    pObj->SetView(pView);
    SetModifiedFlag();
}

void CDrawDoc::Add(CDrawObj* pObj, CBaseView* pView)
{
    pView->GetPage()->AddTail(pObj);
    pObj->SetDoc(this);
    pObj->SetView(pView);
    SetModifiedFlag();
}


void CDrawDoc::Add(CDrawObj* pObj, CObList* pView)
{
    POSITION pos = pView->GetHeadPosition();

    while (pos != NULL) ((CBaseView*) pView->GetNext(pos))->GetPage()->AddTail(pObj);
    pObj->SetDoc(this);
    pObj->AddViewList(pView);
    SetModifiedFlag();
}

void CDrawDoc::Remove(CDrawObj* pObj, CBaseView* pView)
{
        //CBaseView* pView = (CBaseView*) pObj->GetViewList()->GetHead();
    
        // Find and remove from document
        POSITION pos = pView->GetPage()->Find(pObj);
        if (pos != NULL)
        {
            m_deletedobjects.AddHead((CDrawObj*)pView->GetPage()->GetAt(pos));
            pView->GetPage()->RemoveAt(pos);
        }
        // set document modified flag
        SetModifiedFlag();

        // call remove for each view so that the view can remove from m_selection
        pos = GetFirstViewPosition();
        while (pos != NULL)
                ((CBaseView*)GetNextView(pos))->Remove(pObj);
}

CBaseView* CDrawDoc::FindView(CString Name)
{
    return NULL;
}

CDrawObj* CDrawDoc::FindObject(WORD number)
{
    // Funktion muß sich voll auf Pageserver beziehen.
    // Am besten mit einer Funktion:

    return (CDrawObj*) GetPageServer()->FindObject(number);
}

// point is in logical coordinates
CDrawObj* CDrawDoc::ObjectAt(const CPoint& point, CBaseView* pView)
{
        CRect rect(point, CSize(1, 1));
        POSITION pos = pView->GetPage()->GetTailPosition();
        while (pos != NULL)
        {
                CDrawObj* pObj = (CDrawObj*)pView->GetPage()->GetPrev(pos);

                if (pObj->Intersects(rect)) 
                        return pObj;
        }

        return NULL;
}

void CDrawDoc::ComputePageSize()
{
        CPrintDialog dlg(FALSE);
        VERIFY(AfxGetApp()->GetPrinterDeviceDefaults(&dlg.m_pd));

        // GetPrinterDC returns a HDC so attach it
        CDC dc;
        HDC hDC= dlg.CreatePrinterDC();
        ASSERT(hDC != NULL);
        dc.Attach(hDC);

        // Get the size of the page in loenglish
        CSize new_size(MulDiv(dc.GetDeviceCaps(HORZSIZE),1000,254),
                MulDiv(dc.GetDeviceCaps(VERTSIZE),1000,254));

        // if size changed then iterate over views and reset
        if (new_size != m_size)
        {
                m_size = new_size;
                POSITION pos = GetFirstViewPosition();
                while (pos != NULL)
                        ((CBaseView*)GetNextView(pos))->SetPageSize(m_size);

        }
}


void CDrawDoc::OnViewPaperColor()
{
        CColorDialog dlg;
        if (dlg.DoModal() != IDOK)
                return;

        m_paperColor = dlg.GetColor();
        SetModifiedFlag();
        UpdateAllViews(NULL);
}

void CDrawDoc::UpdateIniFileWithDocPath(const char* pszPathName)
{
        theApp.UpdateIniFileWithDocPath(pszPathName);
}



void CDrawDoc::MakeChangesRegardingMe(CPageView* pView)
{
    CPageServer* pServer = pView->GetPageServer(); // Teil für View

    if (pServer != NULL)
    {
        // Vorerst kann sich nur die Seite ändern
        int p = pServer->GetPageNumber();

        char cp[20];
        itoa(p, cp, 10);
        
        CString s = CString("/") + CString(cp);
        
        pView->SetActualPage(s);
    }
    else
    {
        AfxMessageBox("PageServer nach Änderung nicht mehr gefunden!");
    }

    CString pfad = pServer->GetPfad();
}


void CDrawDoc::AddNewPage(CPageView* pView)
{
    CPageServer* pServer = pView->GetPageServer();
    CPageServer* pTargetServer = pServer->FindPfad(pView->GetActualPfad());

    if (pTargetServer != NULL)
    {
        pTargetServer->SetDirFlag(FALSE);
        pTargetServer->AddPage();
    }
    else
    {
        AfxMessageBox("AddNewPage mißglückt!");
    }
}
BOOL CDrawDoc::IsAnyPageLeft(CPageView* pView)
{
    return pView->GetPageServer()->IsAnyPageLeft();
}

void CDrawDoc::DeletePage(CPageView* pView)
{
    pView->GetPageServer()->DeletePage();
}
BOOL CDrawDoc::ExsistAnyPage(CPageView* pView)
{
    return pView->GetPageServer()->ExsistAnyPage();
}

void CDrawDoc::GetNextPage(CPageView* pView)
{
    pView->GetPageServer()->GetNextPage();
}
BOOL CDrawDoc::IsNextPageAvailable(CPageView* pView)
{
    return pView->GetPageServer()->IsNextPageAvailable();
}

void CDrawDoc::GetPrevPage(CPageView* pView)
{
    pView->GetPageServer()->GetPrevPage();
}
BOOL CDrawDoc::IsPrevPageAvailable(CPageView* pView)
{
    return pView->GetPageServer()->IsPrevPageAvailable();
}

void CDrawDoc::GetFirstPage(CPageView* pView)
{
    pView->GetPageServer()->GetFirstPage();
}
void CDrawDoc::GetLastPage(CPageView* pView)
{
    pView->GetPageServer()->GetLastPage();
}

void CDrawDoc::SelectFahrplan(CDrawObj* pObj)
{
    //m_tabellen.AddHead(pObj);
    //m_pActiveFahrplan = pObj;
}


/////////////////////////////////////////////////////////////////////////////
// CDrawDoc diagnostics

#ifdef _DEBUG
void CDrawDoc::AssertValid() const
{
        COleDocument::AssertValid();
}

void CDrawDoc::Dump(CDumpContext& dc) const
{
        COleDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDrawDoc commands

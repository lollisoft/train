// Implementation des Tabellen - Objekts

#include "stdafx.h"
#include "train.h"
#include "pageserv.h"
#include "drawdoc.h"

#include "basevw.h"

#include "drawobj.h"
#include "drawtool.h"
#include "mconnect.h"

#include "cntritem.h"
#include "rectdlg.h"

#include "tabobj.h"

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

IMPLEMENT_SERIAL(CTabelleConnect, CMConnect, 0)

CTabelleConnect::CTabelleConnect()
{
}

CTabelleConnect::CTabelleConnect(CDrawDoc* pConnHome)
               :CMConnect()
{
    ASSERT_VALID(pConnHome);
    Connected = FALSE;
}

CTabelleConnect::~CTabelleConnect()
{
    ASSERT_VALID(this);
}

void CTabelleConnect::Serialize(CArchive& ar)
{
    ASSERT_VALID(this);
    CMConnect::Serialize(ar);

    if (ar.IsStoring())
    {
        ar.Write(&Connected, sizeof(Connected));
    }
    else
    {
        ar.Read(&Connected, sizeof(Connected));
    }
}

IMPLEMENT_SERIAL(CTabelleObj, CDrawObj, 0)

CTabelleObj::CTabelleObj()
{
}

void CTabelleObj::MakeDummy(int Zeilen, int Spalten)
{
    // Erzeugen einer Dummy - Tabelle
    int i, j;
    for (i = 0; i < Spalten; ++i)
    {
        CObList* list = new CObList();
        m_TabellenZeilen.AddTail((CObject*) list); // !!!
        for (j = 0; j < Zeilen; ++j)
        {
            CTabelleConnect* Conn = new CTabelleConnect(m_pDocument);

            list->AddTail(Conn);
        }
    }

    for (i = 0; i < Spalten; ++i)
    {
        SizesOfSpalte.AddTail(new CInt(15));
    }

    for (j = 0; j < Zeilen; ++j)
    {
        SizesOfZeile.AddTail(new CInt(20));
    }
}

CTabelleObj::CTabelleObj(CPoint P, int Zeilen, int Spalten, CDrawDoc* pDoc)
           :CDrawObj(CRect(P, CSize(0, 0)))
{

    m_pDocument = pDoc;

    m_Zeilen = Zeilen;
    m_Spalten = Spalten;
    
    MakeDummy(Zeilen, Spalten);
 
    CalcRect();
}

CTabelleObj::~CTabelleObj()
{
    POSITION pos = m_TabellenZeilen.GetHeadPosition();
    while (pos != NULL)
    {
        CObList* list = (CObList*)m_TabellenZeilen.GetNext(pos);
        POSITION pos1 = list->GetHeadPosition();
        while (pos1 != NULL)
        {
            delete list->GetNext(pos1);
        }
        delete list;
    }
}

void CTabelleObj::Serialize(CArchive& ar)
{
    ASSERT_VALID(this);
    
    CDrawObj::Serialize(ar);

    SizesOfSpalte.Serialize(ar);
    SizesOfZeile.Serialize(ar);

    if (ar.IsStoring)
    {
        ar << m_Zeilen;
        ar << m_Spalten;
    }
    else
    {
        ar >> m_Zeilen;
        ar >> m_Spalten;
    }

    m_TabellenZeilen.Serialize(ar);
}

void CTabelleObj::DeleteSpalte(int s)
{
    if (!m_TabellenZeilen.IsEmpty())  // Tabelle nicht leer ?
    {
        POSITION pos = m_TabellenZeilen.GetHeadPosition();
        while(pos != NULL)
        {
            CObList* pList = (CObList*) m_TabellenZeilen.GetNext(pos);
            CObject* pObj = pList->GetAt(pList->FindIndex(s - 1));
            pList->RemoveAt(pList->FindIndex(s - 1));
            delete pObj;
        }

        m_Spalten--;
    }
}

void CTabelleObj::InsertSpalte(int s, CObList* Spalte)
{
    if (Spalte == NULL) // Dummy:
    {
    // Erzeugen einer Dummy - Tabelle

        if (m_TabellenZeilen.IsEmpty())
        {
            MakeDummy(++m_Zeilen, ++m_Spalten);
            
            return;
        }
    
        POSITION pos = m_TabellenZeilen.GetHeadPosition();

        POSITION pos1;

        while (pos != NULL)
        {
            CObList* pList = (CObList*) m_TabellenZeilen.GetNext(pos);
            
            //pos1 = pList->FindIndex(s);
            CTabelleConnect* pConn = new CTabelleConnect(m_pDocument);
            if (s >= m_Spalten)
            {
                pList->AddTail(pConn);
            }
            else
            {
                if (s < 0) s = 0;
                pList->InsertBefore(pList->FindIndex(s), pConn);
            }
        }

        pos = SizesOfSpalte.FindIndex(s);

        if (pos == NULL)
        {
            SizesOfSpalte.AddTail(new CInt(15));
        }
        else
        {
            SizesOfSpalte.InsertBefore(pos, new CInt(15));
        }

        m_Spalten++;
    }
    else // Liste mit Objekten einfügen:
    {
    }
}

void CTabelleObj::DeleteZeile(int z)
{
    if (!m_TabellenZeilen.IsEmpty())  // Tabelle nicht leer ?
    {
        POSITION pos = m_TabellenZeilen.FindIndex(z);
        if (pos != NULL)
        {
            CObject* pObj = m_TabellenZeilen.GetAt(pos);
            m_TabellenZeilen.RemoveAt(pos);
            delete pObj;
        }

        m_Spalten--;
    }
}

void CTabelleObj::InsertZeile(int z, CObList* Zeile)
{
    if (Zeile == NULL) // Dummy:
    {
    // Erzeugen einer Dummy - Tabelle
        if (z < 0) z = 0;
        POSITION pos = m_TabellenZeilen.FindIndex(z);

        int i, j;

        CObList* list = new CObList();
        if (pos != NULL)
        {
            m_TabellenZeilen.InsertBefore(pos, list);
        }
        else
        {
            m_TabellenZeilen.AddHead((CObject*) list);
        }
        for (j = 1; j <= m_Spalten; ++j)
        {
            CTabelleConnect* Conn = new CTabelleConnect(m_pDocument);

            list->AddTail(Conn);
        }

        pos = SizesOfZeile.FindIndex(z);

        if (pos!= NULL)
        {
            SizesOfZeile.InsertBefore(pos, new CInt(20));
        }
        else
        {
            SizesOfZeile.AddHead(new CInt(20));
        }

        m_Zeilen++;
    }
    else // Liste mit Objekten einfügen:
    {
    }
}

void CTabelleObj::SetElement(CDrawObj* pObj, int z, int s)
{
    POSITION pos = m_TabellenZeilen.FindIndex(z);
    ASSERT(pos != NULL);
    CObList* Spalte = (CObList*) m_TabellenZeilen.GetAt(pos);

    pos = Spalte->FindIndex(s);
    
    CTabelleConnect* Tc = (CTabelleConnect*) Spalte->GetAt(pos);

    if(Tc->SetActualConnection("CT_Tab_Field"))
        Tc->RemoveConnections(Tc->GetConnection()->GetDoc());
    Tc->SetConnectionTyp("CT_Tab_Field");
    Tc->SetConnection(pObj);
}

CDrawObj* CTabelleObj::GetElement(int z, int s)
{
    POSITION pos = m_TabellenZeilen.FindIndex(z);
    ASSERT(pos != NULL);
    CObList* Spalte = (CObList*) m_TabellenZeilen.GetAt(pos);

    pos = Spalte->FindIndex(s);
    
    CTabelleConnect* Tc = (CTabelleConnect*) Spalte->GetAt(pos);

    Tc->SetActualConnection("CT_Tab_Field");
    return (CDrawObj*) Tc->GetConnection();    
}

void CTabelleObj::SetElementSizes(int z, int s, int cz, int cs)
{
    // Ist die Spalte oder Zeile außerhalb dessen Bereiches,
    // wird sie einfach ignoriert.

    cz++;
    cs++;

    POSITION pos;

    if ((z <= (SizesOfZeile.GetCount() - 1)) &&
        (z >= 0))
    {
        pos = SizesOfZeile.FindIndex(z);

        CInt* i = (CInt*) SizesOfZeile.GetAt(pos);
        if (cz > i->Get()) i->Set(cz);
    }

    if ((s <= (SizesOfSpalte.GetCount() - 1)) &&
        (s >= 0))
    {
        pos = SizesOfSpalte.FindIndex(s);

        CInt* i = (CInt*) SizesOfSpalte.GetAt(pos);
        if (cs > i->Get()) i->Set(cs);
    }
}

void CTabelleObj::Draw(CDC* pDC)
{
    ASSERT_VALID(this);
    ASSERT_VALID(m_pDocument);

    int SizeX;
    int SizeY;

    CPoint LinePt1;
    CPoint LinePt2;

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

    int x,y,cx,cy;

    int faktorX, faktorY;

    x = rect.left - 1;
    y = rect.top - 1;

    if (rect.right < rect.left)
        faktorX = 1;
    else
        faktorX = - 1;

    if (rect.bottom < rect.top)
        faktorY = 1;
    else
        faktorY = - 1;
            

    POSITION pos = SizesOfSpalte.GetHeadPosition();

    pDC->MoveTo(x, rect.top);
    pDC->LineTo(x, rect.bottom);
    pDC->MoveTo(rect.left, y);
    pDC->LineTo(rect.right, y);


    while (pos != NULL) // Solange SizesOfSpalte nicht durchlaufen
    {
        x = x - faktorX * ((CInt*) SizesOfSpalte.GetNext(pos))->Get();
        pDC->MoveTo(x, rect.top);
        pDC->LineTo(x, rect.bottom);
    }

    pos = SizesOfZeile.GetHeadPosition();

    while (pos != NULL) // Solange SizesOfZeile nicht durchlaufen
    {
        y = y - faktorY * ((CInt*) SizesOfZeile.GetNext(pos))->Get();
        pDC->MoveTo(rect.left, y);
        pDC->LineTo(rect.right, y);
    }

    //pDC->Rectangle(rect);

    pDC->SelectObject(pOldBrush);
    pDC->SelectObject(pOldPen);
}

void CTabelleObj::KillConnections()
{
    ASSERT_VALID(m_pDocument);
    RemoveConnections(m_pDocument);
}

// point must be in logical
void CTabelleObj::MoveHandleTo(int nHandle, CPoint point)
{
    ASSERT_VALID(this);

    int faktorX, faktorY;

    int S = SizesOfSpalte.GetCount();
    int Z = SizesOfZeile.GetCount();

    CRect position = GetRect();

    if ((1 <= nHandle) && (nHandle <= (S + 1))) // 1 - 5
    {
        // Handles der oberen Zeile und Handle (Top Left)

        if ((nHandle == 1) || (nHandle == (S + 1)))
        {
            if (nHandle == 1)
            {
                faktorX = 1;
                faktorY = 1;
            }
            else
            {
                faktorX = - 1;
                faktorY = 1;
            }


            CPoint p = GetHandle(nHandle);
            if (abs(abs(point.x) - abs(p.x)) >= S)
            {
                POSITION pos = SizesOfSpalte.GetHeadPosition();
                while (pos != NULL)
                {
                    CInt* Num = (CInt*) SizesOfSpalte.GetNext(pos);
                    Num->Set(Num->Get() - faktorX * ((point.x - p.x)/S));
                }
            }
            if (abs(abs(point.y) - abs(p.y)) >= Z)
            {
                POSITION pos = SizesOfZeile.GetHeadPosition();
                while (pos != NULL)
                {
                    CInt* Num = (CInt*) SizesOfZeile.GetNext(pos);
                    Num->Set(Num->Get() - faktorY * ((point.y - p.y)/S));
                }
            }
            CalcRect();
            CRect r = position;
            position = GetRect();
            SetRect(r);

        }
        else
        {
            POSITION pos = SizesOfSpalte.GetHeadPosition();

            CPoint p = GetHandle(nHandle);

            pos = SizesOfSpalte.FindIndex(nHandle - 2);

            CInt* Num = (CInt*) SizesOfSpalte.GetAt(pos);

            pos = SizesOfSpalte.FindIndex(nHandle - 1);

            CInt* Num1 = (CInt*) SizesOfSpalte.GetAt(pos);

            if (((Num->Get() + (point.x - p.x)) >= 1) &&
                ((Num1->Get() - (point.x - p.x)) >= 1))
            {    
                Num->Set(Num->Get() + (point.x - p.x));
                Num1->Set(Num1->Get() - (point.x - p.x));
            }


            CalcRect();
            CRect r = position;
            position = GetRect();
            SetRect(r);
        }    
    }

    if (((S + 2) <= nHandle) && (nHandle <= ((S + 1) * 2))) // 6 - 10
    {
        // Handles der unteren Zeile und Handle (Bottom Left)

        if ((nHandle == (S + 2)) || (nHandle == (S + 1) * 2))
        {
            // Eckpunkte nicht behandeln
        }
        else
        {
            POSITION pos = SizesOfSpalte.GetHeadPosition();

            CPoint p = GetHandle(nHandle);

            pos = SizesOfSpalte.FindIndex(nHandle - (S + 2) - 1);

            CInt* Num = (CInt*) SizesOfSpalte.GetAt(pos);

            pos = SizesOfSpalte.FindIndex(nHandle - (S + 2));

            CInt* Num1 = (CInt*) SizesOfSpalte.GetAt(pos);

            if (((Num->Get() + (point.x - p.x)) >= 1) &&
                ((Num1->Get() - (point.x - p.x)) >= 1))
            {    
                Num->Set(Num->Get() + (point.x - p.x));
                Num1->Set(Num1->Get() - (point.x - p.x));
            }


            CalcRect();
            CRect r = position;
            position = GetRect();
            SetRect(r);
        }
    }
    


        if ((((S + 1) * 2 + 1) <= nHandle) && (nHandle <= ((S + 1) * 2 + Z - 1))) // 11 - 13 !
        {
            // Handles der linken Spalte (2 - GetCount() - 1)

            POSITION pos = SizesOfZeile.GetHeadPosition();

            CPoint p = GetHandle(nHandle);

            pos = SizesOfZeile.FindIndex(nHandle - (S + 1) * 2 - 1);

            CInt* Num = (CInt*) SizesOfZeile.GetAt(pos);

            pos = SizesOfZeile.FindIndex(nHandle - (S + 1) * 2);

            CInt* Num1 = (CInt*) SizesOfZeile.GetAt(pos);

            if (((Num->Get() + (point.y - p.y)) >= 1) &&
                ((Num1->Get() - (point.y - p.y)) >= 1))
            {    
                Num->Set(Num->Get() + (point.y - p.y));
                Num1->Set(Num1->Get() - (point.y - p.y));
            }


            CalcRect();
            CRect r = position;
            position = GetRect();
            SetRect(r);

        }

        if ((((S + 1) * 2 + Z) <= nHandle) && (nHandle <= (S + 1) * 2 + 2 * (Z - 1))) // 14 - 16
        {
            // Handles der rechten Spalte (2 - GetCount() - 1)

            POSITION pos = SizesOfZeile.GetHeadPosition();

            CPoint p = GetHandle(nHandle);

            pos = SizesOfZeile.FindIndex(nHandle - ((S + 1) * 2 + Z));

            CInt* Num = (CInt*) SizesOfZeile.GetAt(pos);

            pos = SizesOfZeile.FindIndex(nHandle - ((S + 1) * 2 + Z) + 1);

            CInt* Num1 = (CInt*) SizesOfZeile.GetAt(pos);

            if (((Num->Get() + (point.y - p.y)) >= 1) &&
                ((Num1->Get() - (point.y - p.y)) >= 1))
            {    
                Num->Set(Num->Get() + (point.y - p.y));
                Num1->Set(Num1->Get() - (point.y - p.y));
            }


            CalcRect();
            CRect r = position;
            position = GetRect();
            SetRect(r);
        }

    MoveTo(position);
}


void CTabelleObj::MoveTo(const CRect& position, BOOL NurSelbst)
{
        ASSERT_VALID(this);

        CRect OldPos = GetRect();

        if (Moved.Find(this) != NULL) return;

        if (position == GetRect())
                return;


        CRect rect = position;
        CRect r = GetRect();

        r.InflateRect(m_logpen.lopnWidth.x/2 + 1, m_logpen.lopnWidth.y/2 + 1);
        rect.InflateRect(m_logpen.lopnWidth.x/2 + 1, m_logpen.lopnWidth.y/2 + 1);


        SetRect(r);
        UpDateViews();
        SetRect(rect);
        UpDateViews();
        SetRect(position);

        ASSERT_VALID(m_pDocument);
        m_pDocument->SetModifiedFlag();

        // Der volgende Befehl bewegt alle angehängten Objekte
        // je nach MT - Option an die neue Position

        //if (pView->m_selection.Find(this) != NULL)
        MoveConnections(position, OldPos);
}

int CTabelleObj::GetHandleCount()
{
        ASSERT_VALID(this);
        return 2 * SizesOfSpalte.GetCount() + 2 * SizesOfZeile.GetCount();
}

// return rectange of handle in logical coords
CRect CTabelleObj::GetHandleRect(int nHandleID, CBaseView* pView)
{
        ASSERT_VALID(this);
        ASSERT(pView != NULL);

        CRect rect;
        // get the center of the handle in logical coords
        CPoint point = GetHandle(nHandleID);

        // convert to client/device coords
        pView->DocToClient(point);
       
        // return CRect of handle in device coords
        rect.SetRect(point.x-3, point.y-3, point.x+3, point.y+3);
        pView->ClientToDoc(rect);

        return rect;
}

// returns logical coords of center of handle
CPoint CTabelleObj::GetHandle(int nHandle)
{
        ASSERT_VALID(this);
        int x, y, xCenter, yCenter;
        int S = SizesOfSpalte.GetCount();
        int Z = SizesOfZeile.GetCount();

        CRect rect = GetRect();

        if ((1 <= nHandle) && (nHandle <= (S + 1))) // 1 - 5
        {
            // Handles der oberen Zeile und Handle (Top Left)

            if (nHandle == 1)
                return CPoint(rect.left, rect.top);
            else
            {
                int delta = - 1;
                POSITION pos = SizesOfSpalte.GetHeadPosition();
                int count = 1;
                while ((pos != NULL) && (count < nHandle))
                {
                    CInt* num = (CInt*) SizesOfSpalte.GetNext(pos);
                    count++;
                    delta += num->Get();
                }
                return CPoint(rect.left + delta, rect.top);
            }    
        }

        if (((S + 2) <= nHandle) && (nHandle <= ((S + 1) * 2))) // 6 - 10
        {
            // Handles der unteren Zeile und Handle (Bottom Left)

            if (nHandle == (S + 2))
                return CPoint(rect.left, rect.bottom);
            else
            {
                int delta = - 1;
                POSITION pos = SizesOfSpalte.GetHeadPosition();
                int count = S + 3;
                while ((pos != NULL) && (count <= nHandle))
                {
                    CInt* num = (CInt*) SizesOfSpalte.GetNext(pos);
                    count++;
                    delta += num->Get();
                }
                return CPoint(rect.left + delta, rect.bottom);
            }    
        }

        if ((((S + 1) * 2 + 1) <= nHandle) && (nHandle <= ((S + 1) * 2 + Z - 1))) // 11 - 13 !
        {
            // Handles der linken Spalte (2 - GetCount() - 1)

            {
                int delta = 0;
                POSITION pos = SizesOfZeile.GetHeadPosition();
                int count = (S + 1) * 2 + 1;
                while ((pos != NULL) && (count <= nHandle))
                {
                    CInt* num = (CInt*) SizesOfZeile.GetNext(pos);
                    count++;
                    delta += num->Get();
                }
                return CPoint(rect.left, rect.top + delta);
            }    
        }

        if ((((S + 1) * 2 + Z) <= nHandle) && (nHandle <= (S + 1) * 2 + 2 * (Z - 1))) // 14 - 16
        {
            // Handles der rechten Spalte (2 - GetCount() - 1)

            {
                int delta = 0;
                POSITION pos = SizesOfZeile.GetHeadPosition();
                int count = (S + 1) * 2 + Z;
                while ((pos != NULL) && (count <= nHandle))
                {
                    CInt* num = (CInt*) SizesOfZeile.GetNext(pos);
                    count++;
                    delta += num->Get();
                }
                return CPoint(rect.right, rect.top + delta);
            }    
        }
        AfxMessageBox("GetHandle nicht IO!");
        ASSERT(FALSE);
        return CPoint(0, 0);
}

HCURSOR CTabelleObj::GetHandleCursor(int nHandle)
{
        ASSERT_VALID(this);

        int S = SizesOfSpalte.GetCount();
        int Z = SizesOfZeile.GetCount();

        LPCSTR id;

        if ((1 <= nHandle) && (nHandle <= (S + 1))) // 1 - 5
            id = IDC_SIZEWE;

        if (nHandle == 1) id = IDC_SIZENWSE;
        if (nHandle == (S + 1)) id = IDC_SIZENESW;

        if (((S + 2) <= nHandle) && (nHandle <= ((S + 1) * 2))) // 6 - 10
            id = IDC_SIZEWE;

        if (nHandle == (S + 2)) id = IDC_SIZENESW;
        if (nHandle == ((S + 1) * 2)) id = IDC_SIZENWSE;

            
        if ((((S + 1) * 2 + 1) <= nHandle) && (nHandle <= ((S + 1) * 2 + Z - 1))) // 11 - 13 !
            id = IDC_SIZENS;
        if ((((S + 1) * 2 + Z) <= nHandle) && (nHandle <= (S + 1) * 2 + 2 * (Z - 1))) // 14 - 16
            id = IDC_SIZENS;

        return AfxGetApp()->LoadStandardCursor(id);
}


// point is in logical coordinates
int CTabelleObj::HitTest(CPoint point, CBaseView* pView, BOOL bSelected)
{
        ASSERT_VALID(this);
        ASSERT(pView != NULL);

        if (bSelected)
        {
                int nHandleCount = GetHandleCount();
                for (int nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
                {
                        // GetHandleRect returns in logical coords
                        CRect rc = GetHandleRect(nHandle,pView);
                        if (point.x >= rc.left && point.x < rc.right &&
                                point.y <= rc.top && point.y > rc.bottom)
                                return nHandle;
                }
        }
        else
        {
                if (point.x >= GetRect().left && point.x < GetRect().right &&
                        point.y <= GetRect().top && point.y > GetRect().bottom)
                        return 1;
        }
        return 0;
}


CPoint CTabelleObj::GetLocalPos(int Zeile, int Spalte)
{
    CPoint p;
    POSITION pos;
    p = CPoint(GetRect().left, GetRect().top);
    int xh = (((CInt*) SizesOfSpalte.GetHead())->Get() / 2);
    int yh = (((CInt*) SizesOfZeile.GetHead())->Get() / 2);
    
    int i;

    p.x += xh;
    p.y += yh;
    
    pos = SizesOfSpalte.GetHeadPosition();
    SizesOfSpalte.GetNext(pos);
    for (i = 0; i < Spalte; i++)
    {
        int x = ((CInt*) SizesOfSpalte.GetNext(pos))->Get() / 2;
        p.x += (xh + x + 1);
        xh = x; 
    }
    

    pos = SizesOfZeile.GetHeadPosition();
    for (i = 0; i < Zeile; i++)
    {
        int y = ((CInt*) SizesOfZeile.GetNext(pos))->Get() / 2;
        p.y += (yh + y + 1);
        yh = y;
    }

    return p;
}

CRect CTabelleObj::GetClientRect(CDrawObj* pObj)
{
    CRect rect;
    return rect;
}

CRect CTabelleObj::GetClientRect(int Zeile, int Spalte)
{
    CRect rect;

    int x = ((CInt*) SizesOfSpalte.GetAt(SizesOfSpalte.FindIndex(Spalte)))->Get();
    int y = ((CInt*) SizesOfZeile.GetAt(SizesOfZeile.FindIndex(Zeile)))->Get();

    rect = CRect(CPoint(GetRect().left, GetRect().top), CSize(x, y));

    CPoint p = GetLocalPos(Zeile, Spalte) - GetPos(rect);

    rect.OffsetRect(p);
   
    return rect;
}

void CTabelleObj::RecalcAllPositions()
{
    POSITION pos1 = m_TabellenZeilen.GetHeadPosition();

    int z, s;

    z = s = 0;

    CalcRect();
    Invalidate();

    while (pos1 != NULL)
    {

        CObList* Spalten = (CObList*) m_TabellenZeilen.GetNext(pos1);

        POSITION pos2 = Spalten->GetHeadPosition();

        while (pos2 != NULL)
        {
            CTabelleConnect* pConn = (CTabelleConnect*) Spalten->GetNext(pos2);

            if (pConn->SetActualConnection("CT_Tab_Field"))
            {
                CDrawObj* pObj = (CDrawObj*) pConn->GetConnection();
                SetElementSizes(z, s, pObj->GetRect().Height(), pObj->GetRect().Width());
                pObj->Invalidate();
                pObj->SetRect(GetClientRect(z, s));
                pObj->Invalidate();
            }           
            s++;
        }
        s = 0;
        z++;
    }
    Invalidate();
}

BOOL CTabelleObj::CalcRect()
{
    ASSERT_VALID(this);
    int HoleSizeX = 0;
    int HoleSizeY = 0;

    POSITION pos = SizesOfSpalte.GetHeadPosition();

    while (pos != NULL)
    {
        CInt* X = (CInt*) SizesOfSpalte.GetNext(pos);
        HoleSizeX += X->Get();
    }

    pos = SizesOfZeile.GetHeadPosition();

    while (pos != NULL)
    {
        CInt* Y = (CInt*) SizesOfZeile.GetNext(pos);
        HoleSizeY += Y->Get();
    }

    CRect rect = CRect(GetPos(GetRect()), CSize(0, 0));


    rect.InflateRect(((HoleSizeX - rect.Size().cx) / 2),
                     ((HoleSizeY - rect.Size().cy) / 2));
    SetRect(rect);
    return TRUE;
}


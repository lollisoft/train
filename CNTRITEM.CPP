// cntritem.h : interface of the CDrawItem class
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
#include "basevw.h"
#include "pagevw.h"
#include "pageserv.h"
#include "drawdoc.h"
#include "drawobj.h"
#include "tabvw.h"
#include "cntritem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrawItem implementation

IMPLEMENT_SERIAL(CDrawItem, COleClientItem, 0)

CDrawItem::CDrawItem(CDrawDoc* pContainer, CDrawOleObj* pDrawObj)
        : COleClientItem(pContainer)
{
        m_pDrawObj = pDrawObj;
}

CDrawItem::~CDrawItem()
{
        // TODO: add cleanup code here
}

void CDrawItem::OnChange(OLE_NOTIFICATION nCode, DWORD dwParam)
{
        ASSERT_VALID(this);

        COleClientItem::OnChange(nCode, dwParam);

        switch(nCode)
        {
        case OLE_CHANGED_STATE:
        case OLE_CHANGED_ASPECT:
                m_pDrawObj->Invalidate();
                break;
        case OLE_CHANGED:
                UpdateExtent(); // extent may have changed
                m_pDrawObj->Invalidate();
                break;
        }
}

BOOL CDrawItem::OnChangeItemPosition(const CRect& rectPos)
{
        ASSERT_VALID(this);

        CBaseView* pView = GetActiveView();
        ASSERT_VALID(pView);
        CRect rect = rectPos;
        pView->ClientToDoc(rect);

        if (rect != m_pDrawObj->GetRect())
        {
                // invalidate old rectangle
                m_pDrawObj->Invalidate();

                // update to new rectangle
                m_pDrawObj->GetRect() = rect;
                GetExtent(&m_pDrawObj->m_extent);

                // and invalidate new rectangle
                m_pDrawObj->Invalidate();

                // mark document as dirty
                GetDocument()->SetModifiedFlag();
        }
        return COleClientItem::OnChangeItemPosition(rectPos);
}

void CDrawItem::OnGetItemPosition(CRect& rPosition)
{
        ASSERT_VALID(this);

        // update to extent of item if m_position is not initialized
        if (m_pDrawObj->GetRect().IsRectEmpty())
                UpdateExtent();

        // copy m_position, which is in document coordinates
        CBaseView* pView = GetActiveView();
        ASSERT_VALID(pView);
        rPosition = m_pDrawObj->GetRect();
        pView->DocToClient(rPosition);
}

void CDrawItem::Serialize(CArchive& ar)
{
        ASSERT_VALID(this);

        // Call base class first to read in COleClientItem data.
        // Note: this sets up the m_pDocument pointer returned from
        //  CDrawItem::GetDocument, therefore it is a good idea
        //  to call the base class Serialize first.
        COleClientItem::Serialize(ar);

        // now store/retrieve data specific to CDrawItem
        if (ar.IsStoring())
        {
                // TODO: add storing code here
        }
        else
        {
                // TODO: add loading code here
        }
}

BOOL CDrawItem::UpdateExtent()
{
        CSize size;
        if (!GetExtent(&size) || size == m_pDrawObj->m_extent)
                return FALSE;       // blank

        // if new object (i.e. m_extent is empty) setup position
        if (m_pDrawObj->m_extent == CSize(0, 0))
        {
                m_pDrawObj->GetRect().right =
                        m_pDrawObj->GetRect().left + MulDiv(size.cx, 10, 254);
                m_pDrawObj->GetRect().bottom =
                        m_pDrawObj->GetRect().top - MulDiv(size.cy, 10, 254);
        }
        // else data changed so scale up rect as well
        else if (!IsInPlaceActive() && size != m_pDrawObj->m_extent)
        {
                m_pDrawObj->GetRect().right = m_pDrawObj->GetRect().left +
                        MulDiv(m_pDrawObj->GetRect().Width(), size.cx, m_pDrawObj->m_extent.cx);
                m_pDrawObj->GetRect().bottom = m_pDrawObj->GetRect().top +
                        MulDiv(m_pDrawObj->GetRect().Height(), size.cy, m_pDrawObj->m_extent.cy);
        }

        m_pDrawObj->m_extent = size;
        m_pDrawObj->Invalidate();   // redraw to the new size/position
        return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CDrawItem diagnostics

#ifdef _DEBUG
void CDrawItem::AssertValid() const
{
        COleClientItem::AssertValid();
}

void CDrawItem::Dump(CDumpContext& dc) const
{
        COleClientItem::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////

// CMapView:
// written by Gerry High
// 74750,2456 CIS
// for more info see the tabview.wri document

#include "stdafx.h"
#include "mapview.h"
#include <ctype.h>
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


#define BASE CView
#undef THIS
#define THIS CMapView
const int TAB_TABHEIGHT = 25;
const int TAB_MARGIN = 7;

#define BKPEN()pDC->SelectObject(&blackPen)
#define LTPEN()pDC->SelectObject(&lightPen)
#define DKPEN()pDC->SelectObject(&darkPen)
#define MARGIN  7
/////////////////////////////////////////////////////////////////////////////
// CMapView

IMPLEMENT_DYNCREATE(CMapView, CView)

BEGIN_MESSAGE_MAP(CMapView, CView)
        //{{AFX_MSG_MAP(CMapView)
        ON_WM_ERASEBKGND()
        ON_WM_SIZE()
        ON_WM_LBUTTONDOWN()
        ON_WM_SETFOCUS()
        ON_WM_MOUSEACTIVATE()
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapView construction/destruction

THIS::THIS()
{
        m_curTab = -1;
        m_nTabs = 0;
        m_tabHeight = TAB_TABHEIGHT;
        m_lookAndFeel = LAF_MSWORD;
        m_curView = NULL;
        m_position = TABSONTOP;
        m_lfEscapement = 0;
        m_margin = TAB_MARGIN;
        m_boldFont = NULL;
        m_normalFont = NULL;
}

THIS::~THIS()
{
        // free up tab info
        CObject* pTab;
        for(int i=0;i<m_nTabs;i++)
        {
                if( (pTab = m_tabArray.GetAt(0)) != NULL)
                {
                        m_tabArray.RemoveAt(0);
                        delete pTab;
                }
        }
        if(m_boldFont)
                delete m_boldFont;
        if(m_normalFont)
                delete m_normalFont;
}
/////////////////////////////////////////////////////////////////////////////
// CMapView configuration helper methods

void  THIS::setLAF(eLookAndFeel LAF)
{
        if(LAF == m_lookAndFeel)
                return;
                
        m_lookAndFeel = LAF;
        if(IsWindowVisible()) InvalidateRect(NULL);
}
void THIS::setMargin(int margin)
{
        if(margin == m_margin)
                return;
                
        m_margin = margin;
        repositionViews();
        if(IsWindowVisible()) InvalidateRect(NULL);
}

void THIS::setTabHeight(int height)
{
        if(height == m_tabHeight)
                return;
                
        m_tabHeight = height;
        repositionViews();
        if(IsWindowVisible()) InvalidateRect(NULL);
}

void THIS::setTabPosition(eTabPosition position)
{
        int i,j;

        if(position == m_position)
                return;
                        
        switch (position)
        {
                case TABSONLEFT:
                case TABSONLEFTBOT:
                case TABSONRIGHT:
                case TABSONRIGHTBOT:
                        m_position = position;
                        if(m_position == TABSONLEFT || m_position == TABSONLEFTBOT)
                                m_lfEscapement = 900;
                        else
                                m_lfEscapement = -900;
                        for(j=0;j<m_nTabs;j++)
                        {
                                CTabInfo* pTab = (CTabInfo*)m_tabArray[j];
                                int sLen = strlen(pTab->m_tabLabel);
                                for(i=0;i<sLen;i++)
                                {
                                        if(pTab->m_tabLabel[i] == '&' && i != (sLen -1))
                                        {
                                                memmove(&pTab->m_tabLabel[i],&pTab->m_tabLabel[i+1],sLen - i);
                                                break;
                                        }
                                }
                        }
                        break;

                default:
                        m_position = TABSONTOP;
                        m_lfEscapement = 0;
                        for(j=0;j<m_nTabs;j++)
                        {
                                CTabInfo* pTab = (CTabInfo*)m_tabArray[j];
                                int sLen = strlen(pTab->m_tabLabel);
                                for(i=0;i<sLen;i++)
                                {
                                        if((char)(DWORD)AnsiUpper((LPSTR)MAKELP(0,pTab->m_tabLabel[i])) == pTab->m_mnemonic
                                                && i != (sLen -1))
                                        {
                                                memmove(&pTab->m_tabLabel[i+1],&pTab->m_tabLabel[i],sLen - i);
                        pTab->m_tabLabel[i] = '&';
                        break;
                                        }
                                }
                        }
                        break;
        }

        if(m_nTabs != 0)
        {
                createFonts();
                repositionViews();
                if(IsWindowVisible()) InvalidateRect(NULL);
        }
}
void THIS::setFrameBorderOn(BOOL on)
{
        m_frameBorderOn = on;
}
void THIS::repositionViews()
{
        for(int i=0;i<m_nTabs;i++)
        {
                CTabInfo* pTab = (CTabInfo*)m_tabArray[i];
                switch (m_position)
                {
                        case TABSONLEFT:
                        case TABSONLEFTBOT:
                                pTab->m_pView->SetWindowPos(NULL,m_margin+5+m_tabHeight,m_margin+5,
                                        m_width-2*(m_margin+5)-m_tabHeight,     m_height-2*(m_margin+5), SWP_NOZORDER);
                                break;

                        case TABSONRIGHT:
                        case TABSONRIGHTBOT:
                                pTab->m_pView->SetWindowPos(NULL,m_margin+5,m_margin+5,
                                        m_width-2*(m_margin+5)-m_tabHeight,     m_height-2*(m_margin+5), SWP_NOZORDER);
                                break;

                        default:
                                pTab->m_pView->SetWindowPos(NULL, m_margin+5,m_margin+5+m_tabHeight,
                                        m_width-2*(m_margin+5), m_height-2*(m_margin+5)-m_tabHeight, SWP_NOZORDER);
                                break;
                }
        }
}
void THIS::createFonts()
{
        if(m_normalFont)
                delete m_normalFont;
        if(m_boldFont)
                delete m_boldFont;
                
        // generate fonts       
        LOGFONT fontRec;
        
        memset(&fontRec,0,sizeof(LOGFONT));
        fontRec.lfHeight = -13;
    fontRec.lfWeight = FW_NORMAL;
    fontRec.lfEscapement = m_lfEscapement;
    lstrcpy(fontRec.lfFaceName,"Arial");
    
    m_normalFont = new CFont;
    m_normalFont->CreateFontIndirect(&fontRec);
    
    fontRec.lfWeight = FW_BOLD;
    m_boldFont = new CFont;
    m_boldFont->CreateFontIndirect(&fontRec);
}


/////////////////////////////////////////////////////////////////////////////
// CMapView message handlers

BOOL THIS::OnEraseBkgnd(CDC* pDC)
{
        CBrush backBrush(GetSysColor(COLOR_BTNFACE));
        CBrush* pOldBrush = pDC->SelectObject(&backBrush);
        
        CRect rect;
        pDC->GetClipBox(&rect); //erase the area needed
        pDC->PatBlt(rect.left,rect.top,rect.Width(),rect.Height(),PATCOPY);
        pDC->SelectObject(pOldBrush);
        return TRUE;
}

void THIS::OnSize(UINT nType, int cx, int cy)
{
        CView::OnSize(nType, cx, cy);
        m_width = cx;
        m_height = cy;
        
        for(int i=0;i<m_nTabs;i++)
        {
                CTabInfo* pTab = (CTabInfo*)m_tabArray[i];
                switch (m_position)
                {
                        case TABSONLEFT:
                        case TABSONLEFTBOT:
                        case TABSONRIGHT:
                        case TABSONRIGHTBOT:
                                pTab->m_pView->SetWindowPos(NULL,0,0,cx-2*(m_margin+5)-m_tabHeight,cy-2*(m_margin+5),
                                        SWP_NOMOVE|SWP_NOZORDER);
                                break;

                        default:
                                pTab->m_pView->SetWindowPos(NULL,0,0,cx-2*(m_margin+5),cy-2*(m_margin+5)-m_tabHeight,
                                        SWP_NOMOVE|SWP_NOZORDER);
                                break;
                }
        }
}
void THIS::OnLButtonDown(UINT nFlags, CPoint point)
{
        switch (m_position)
        {
                case TABSONLEFT:
                case TABSONLEFTBOT:
                case TABSONRIGHT:
                case TABSONRIGHTBOT:
                        if ( switchVerticalTab(point) )
                                return;

                default:
                        if ( switchTopTab(point) )
                                return;
        }
        GetParentFrame()->SetActiveView(m_curView);
}

BOOL CMapView::switchVerticalTab(CPoint point)
{
        CRect rect;
        int x = (m_position == TABSONLEFT || m_position == TABSONLEFTBOT) ? m_margin : 
                (m_width - m_margin - m_tabHeight);
        int y = (m_position == TABSONLEFT || m_position == TABSONRIGHT) ? m_margin:(m_height - m_margin);

        rect.left = x;
        rect.right = rect.left + m_tabHeight;
        
        for(int i = 0; i < m_nTabs; i++)
        {
                CTabInfo* pTab = (CTabInfo*)m_tabArray[i];
                if(m_position == TABSONLEFT || m_position == TABSONRIGHT)
                {
                        rect.top        = y;
                        rect.bottom = y + pTab->m_tabWidth;
                }
                else
                {
                        rect.bottom = y;
                        rect.top = rect.bottom - pTab->m_tabWidth;
                }
                if(rect.PtInRect(point) && pTab->m_active)
                {
                        switchTab(i);
                        return TRUE;
                }
                if(m_position == TABSONLEFT || m_position == TABSONRIGHT)
                        y += pTab->m_tabWidth;
                else
                        y -= pTab->m_tabWidth;
        }
        return(FALSE);
}
BOOL CMapView::switchTopTab(CPoint point)
{
        CRect rect;
        int x = m_margin, y = m_margin;

        rect.top = y;
        rect.bottom = y + m_tabHeight;

        for(int i = 0; i < m_nTabs; i++)
        {
                CTabInfo* pTab = (CTabInfo*)m_tabArray[i];
                rect.left = x;
                rect.right = rect.left + pTab->m_tabWidth;
                if(rect.PtInRect(point) && pTab->m_active)
                {
                        switchTab(i);
                        return TRUE;
                }
                x += pTab->m_tabWidth;
        }
        return(FALSE);
}
void THIS::OnInitialUpdate()
{
        BASE::OnInitialUpdate();

        createFonts();
        
        CFrameWnd* pFrame = GetParentFrame();
        pFrame->RecalcLayout();
        m_curTab = 0;
        
        if(m_nTabs)
        {
                CTabInfo* pTab = (CTabInfo*)m_tabArray[0];
                m_curView = pTab->m_pView;
                m_curView->ShowWindow(SW_SHOW);
                pFrame->SetActiveView(m_curView);
        }
}
CView* THIS::addTabView(
        CRuntimeClass* viewClass,
        CDocument* document,
        char* tabLabel,
        BOOL border,
        BOOL show,
        int tabWidth
        )
{
        // Setup defaults

        if(!tabLabel)
                return NULL;

        CTabInfo* pTab = new CTabInfo;
        if(!pTab)
                return NULL;
                
        m_curTab++;
        pTab->m_pView = createTabView(viewClass,document,this,border,show);
        if(!pTab->m_pView)
        {
                delete pTab;
                m_curTab--;
                return NULL;
        }
        pTab->m_tabWidth = tabWidth;
        int sLen = strlen(tabLabel);
        pTab->m_tabLabel = (char *)new char[sLen + 1 ];
        strcpy(pTab->m_tabLabel,tabLabel);
        for(int i=0;i<sLen;i++)
        {
                if(tabLabel[i] == '&' && i != (sLen -1))
                {
                        pTab->m_mnemonic = (char)(DWORD)AnsiUpper((LPSTR)MAKELP(0,tabLabel[i+1]));
                        if(m_position == TABSONLEFT ||
                           m_position == TABSONLEFTBOT)
                                memmove(&tabLabel[i],&tabLabel[i+1],sLen - i-1);
                        break;
                }
        }
        
        m_tabArray.Add(pTab);
        m_nTabs++;

        return pTab->m_pView;
}
CView* THIS::createTabView(
        CRuntimeClass* pViewClass,
        CDocument* pDoc,
        CWnd*   parentWnd,
        BOOL border,
        BOOL show
        )
{
        CRect viewRect;

        switch (m_position)
        {
                case TABSONLEFT:
                case TABSONLEFTBOT:
                        viewRect.SetRect(m_margin+5+m_tabHeight,m_margin+5,m_width-(m_margin+5),m_height-(m_margin+5));
                        break;

                case TABSONRIGHT:
                case TABSONRIGHTBOT:
                        viewRect.SetRect(m_margin+5,m_margin+5,m_width-(m_margin+5),m_height-(m_margin+5));
                        break;

                default:
                        viewRect.SetRect(m_margin+5,m_margin+5+m_tabHeight,m_width-(m_margin+5),m_height-(m_margin+5));
                        break;
        }

        CView* pView = (CView*)pViewClass->CreateObject();
    ASSERT(pView != NULL);
    
        CCreateContext context;
        context.m_pNewViewClass         = pViewClass;
        context.m_pCurrentDoc           = pDoc;
        context.m_pNewDocTemplate       = NULL;
        context.m_pLastView                     = NULL;
        context.m_pCurrentFrame         = GetParentFrame();
    if (!pView->Create(NULL, NULL, WS_CHILD|(show ? WS_VISIBLE:0)|
        (border ? WS_BORDER : 0),viewRect,
                this, AFX_IDW_PANE_FIRST+1+m_curTab, &context))
        {
                TRACE0("Warning: couldn't create view for frame\n");
                return NULL;
        }

        return pView;
}
void THIS::switchTab(int viewIndex)
{
        CTabInfo* pTab = (CTabInfo*)m_tabArray[viewIndex];
        if(viewIndex < 0 || viewIndex >= m_nTabs)
                return;

        if(m_curView != pTab->m_pView)
        {
                pTab->m_pView->ShowWindow(SW_SHOW);
                m_curView->ShowWindow(SW_HIDE);
                m_curView = pTab->m_pView;
                m_curTab = viewIndex;
                InvalidateRect(NULL);
        }
        GetParentFrame()->SetActiveView(m_curView);
}
void THIS::enableView(int viewIndex, BOOL bEnable)
{
        CTabInfo* pTab = (CTabInfo*)m_tabArray[viewIndex];
        if(viewIndex < 0 || viewIndex >= m_nTabs)
                return;

//can't disable current view
        if(m_curView != pTab->m_pView && bEnable != pTab->m_active)
        {
                pTab->m_active = bEnable;
                if(IsWindowVisible()) InvalidateRect(NULL);
        }
}
BOOL THIS::doSysCommand(UINT nID,LONG lParam)
{
        WPARAM wCmdType = nID & 0xFFF0;
        WORD dummy = HIWORD(lParam);
        char mnemonic = (char)(DWORD)AnsiUpper((LPSTR)MAKELP(0,(char)LOWORD(lParam)));
        switch(wCmdType)
        {
                case SC_KEYMENU:
                        if(dummy != 0)
                                break;
                        for(int i = 0; i < m_nTabs; i++)
                        {
                                CTabInfo* pTab = (CTabInfo*)m_tabArray[i];
                                if(mnemonic == pTab->m_mnemonic && pTab->m_active)
                                {
                                        switchTab(i);
                                        return 1;
                                }
                        }
                        break;
        }
        return 0;
}
/////////////////////////////////////////////////////////////////////////////
// CMapView drawing

void THIS::OnDraw(CDC* pDC)
{
        CFont* oldFont = pDC->SelectObject(m_boldFont);
    int oldMode = pDC->SetBkMode(TRANSPARENT);
    
        CPen blackPen(PS_SOLID,0,GetSysColor(COLOR_WINDOWFRAME));
        CPen darkPen(PS_SOLID,0,GetSysColor(COLOR_BTNSHADOW));
        CPen lightPen(PS_SOLID,0,GetSysColor(COLOR_BTNHIGHLIGHT));
        CPen* pOldPen = pDC->SelectObject(&darkPen);

        if(m_lookAndFeel == LAF_CHICAGO && m_position == TABSONLEFT)
        {
                TRACE0("Warning: left tabs not supported for LAF = CHICAGO");
                m_lookAndFeel = LAF_MSWORD;
        }

        if(m_lookAndFeel == LAF_CHICAGO)
        {       
                drawChicagoTabs(pDC, blackPen, darkPen, lightPen, pOldPen);
        }
        else // MS WORD STYLE LOOK--compliments of David Hollifield
        {
                drawMSWordTabs(pDC, blackPen, darkPen, lightPen, pOldPen);
        }

        pDC->SelectObject(pOldPen);
        pDC->SetBkMode(oldMode);
        pDC->SelectObject(oldFont);
}
void THIS::drawChicagoTabs(CDC *pDC, CPen& blackPen, CPen& darkPen, CPen& lightPen, CPen *pOldPen)
{   
    COLORREF oldTextColor = pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));
        int x, y, width, height;
        CRect rect;
        pDC->GetClipBox(&rect);

        x = m_margin;
        y = m_margin + m_tabHeight;
        width = m_width - 2 * m_margin;
        height = m_height - 2 * m_margin;

        //
        //draw 3 outer lines of bounding rect
        //      
        DKPEN();
        pDC->MoveTo(x,y);
        pDC->LineTo(x,y+height-1-m_tabHeight);
        pDC->LineTo(x+width-1,y+height-1-m_tabHeight);
        pDC->LineTo(x+width-1,y);

        BKPEN();
        pDC->MoveTo(x,y+height-m_tabHeight);
        pDC->LineTo(x+width,y+height-m_tabHeight);
        pDC->LineTo(x+width,y);

        LTPEN();        
        pDC->MoveTo(x+1,y);
        pDC->LineTo(x+1,y+height-2-m_tabHeight);

        //
        // now draw each tab
        //
        x = m_margin;
        y = m_margin;
        for(int i=0;i<m_nTabs;i++)
        {
                CTabInfo* pTab = (CTabInfo*)m_tabArray[i];
                if(i == m_curTab)
                {
                        DKPEN();
                        pDC->MoveTo(x,y+1);
                        pDC->LineTo(x,y+3+m_tabHeight-3);

                        LTPEN();
                        pDC->MoveTo(x+1,y+1);
                        pDC->LineTo(x+1,y+3+m_tabHeight-3);
                
                        pDC->MoveTo(x+2,y);     //1 pixel spot
                        pDC->LineTo(x+3,y+1);
                
                        pDC->MoveTo(x+3,y-1);
                        pDC->LineTo(x+pTab->m_tabWidth-2,y-1);

                        DKPEN();
                        pDC->MoveTo(x+3,y-2);
                        pDC->LineTo(x+pTab->m_tabWidth-2,y-2);
                        pDC->LineTo(x+pTab->m_tabWidth-2,y+m_tabHeight);
                
                        BKPEN();
                        pDC->MoveTo(x+pTab->m_tabWidth-1,y+1);
                        pDC->LineTo(x+pTab->m_tabWidth-1,y+m_tabHeight);
//              
                        LTPEN();
                        if(i != 0)
                        {
                                pDC->MoveTo(x-1,y+3+m_tabHeight-4);
                                pDC->LineTo(m_margin+1,y+3+m_tabHeight-4);
                        }
                        pDC->MoveTo(x+pTab->m_tabWidth,y+3+m_tabHeight-4);
                        pDC->LineTo(m_width-m_margin-2,y+3+m_tabHeight-4);

                        DKPEN();
                        pDC->MoveTo(x+pTab->m_tabWidth,y+3+m_tabHeight-5);
                        pDC->LineTo(m_width-m_margin-2,y+3+m_tabHeight-5);
                        pDC->SelectObject(m_boldFont);
                }
                else
                {
                        DKPEN();
                        pDC->MoveTo(x,y+3);
                        pDC->LineTo(x,y+3+m_tabHeight-3);

                        LTPEN();
                        pDC->MoveTo(x+1,y+3);
                        pDC->LineTo(x+1,y+3+m_tabHeight-3);
                
                        pDC->MoveTo(x+2,y+2);   //1 pixel spot
                        pDC->LineTo(x+3,y+3);
                
                        pDC->MoveTo(x+3,y+1);
                        pDC->LineTo(x+pTab->m_tabWidth-2,y+1);
                
                        DKPEN();
                        pDC->MoveTo(x+3,y);
                        pDC->LineTo(x+pTab->m_tabWidth-2,y);
                        pDC->LineTo(x+pTab->m_tabWidth-2,y+m_tabHeight);
                
                        BKPEN();
                        pDC->MoveTo(x+pTab->m_tabWidth-1,y+3);
                        pDC->LineTo(x+pTab->m_tabWidth-1,y+m_tabHeight);
                        pDC->SelectObject(m_normalFont);
                }

                CRect rect(x,y,x+pTab->m_tabWidth,y+m_tabHeight);
            pDC->SetTextColor(GetSysColor(pTab->m_active ? COLOR_BTNTEXT:COLOR_GRAYTEXT));
                pDC->DrawText(pTab->m_tabLabel,lstrlen(pTab->m_tabLabel),&rect,
                                DT_CENTER|DT_VCENTER|DT_SINGLELINE);
                x += pTab->m_tabWidth;
                
        }

        pDC->SetTextColor(oldTextColor);
}
void THIS::drawMSWordTabs(CDC *pDC, CPen& blackPen, CPen& darkPen, CPen& lightPen, CPen *pOldPen)
{
        switch (m_position)
        {
                case TABSONLEFT:
                case TABSONLEFTBOT:
                        drawMSWordLeftTabs(pDC, blackPen, darkPen, lightPen, pOldPen);
                        break;

                case TABSONRIGHT:
                case TABSONRIGHTBOT:
                        drawMSWordRightTabs(pDC, blackPen, darkPen, lightPen, pOldPen);
                        break;

                default:
                        drawMSWordTopTabs(pDC, blackPen, darkPen, lightPen, pOldPen);
                        break;
        }
}
void THIS::drawMSWordTopTabs(CDC *pDC, CPen& blackPen, CPen& darkPen, CPen& lightPen, CPen *pOldPen)
{
    COLORREF oldTextColor = pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));
        int x, y, width, height;
        CRect rect;
        pDC->GetClipBox(&rect);

        x = m_margin;
        y = m_margin + m_tabHeight;
        width = m_width - 2 * m_margin;
        height = m_height - 2 * m_margin;

        // draw 3 outer lines of bounding rect
        //
        BKPEN();
        pDC->MoveTo(x                           , y);
        pDC->LineTo(x                           , y + height - m_tabHeight);
        pDC->LineTo(x + width           , y + height - m_tabHeight);
        pDC->LineTo(x + width           , y - 1);

        LTPEN();
        pDC->MoveTo(x + 1                       , y);
        pDC->LineTo(x + 1                       , y + height - m_tabHeight);
        pDC->MoveTo(x + 2                       , y);
        pDC->LineTo(x + 2                       , y + height - 1 - m_tabHeight);

        DKPEN();
        pDC->MoveTo(x + 2                       , y + height - 1 - m_tabHeight);
        pDC->LineTo(x + width - 1       , y + height - 1 - m_tabHeight);
        pDC->LineTo(x + width - 1       , y);
        pDC->MoveTo(x + 3                       , y + height - 2 - m_tabHeight);
        pDC->LineTo(x + width - 2       , y + height - 2 - m_tabHeight);
        pDC->LineTo(x + width - 2       , y + 1);

        // now draw each tab
        //
        x = m_margin;
        y = m_margin;

        for (int i = 0; i < m_nTabs; i++)
        {
                CTabInfo *pTab = (CTabInfo *) m_tabArray[i];

                if (i == m_curTab)
                {
                        // Outline
                        BKPEN();
                        pDC->MoveTo(m_margin                                    , y + m_tabHeight);
                        pDC->LineTo(x                                           , y + m_tabHeight);     // 1
                        pDC->LineTo(x                                           , y + 4);                       // 2
                        pDC->LineTo(x + 4                                       , y);                           // 3
                        pDC->LineTo(x + pTab->m_tabWidth - 4, y);                               // 4
                        pDC->LineTo(x + pTab->m_tabWidth        , y + 4);                       // 5
                        pDC->LineTo(x + pTab->m_tabWidth        , y + m_tabHeight);     // 6
                        pDC->LineTo(m_margin + width            , y + m_tabHeight);     // 7

                        // Hilite
                        for (int j = 0; j < 2; j++)
                        {
                                LTPEN();
                                pDC->MoveTo(m_margin + 1                                , y + m_tabHeight + 1 + j);
                                pDC->LineTo(x + 1 + j                           , y + m_tabHeight + 1 + j);     // 1
                                pDC->LineTo(x + 1 + j                           , y + 4 + j);                           // 2
                                pDC->LineTo(x + 4 + j                           , y + 1 + j);                           // 3
                                pDC->LineTo(x + pTab->m_tabWidth - 3, y + 1 + j);                               // 4
                
                                pDC->MoveTo(x + pTab->m_tabWidth - 2, y + m_tabHeight + 1 + j); 
                                pDC->LineTo(m_margin + width - j        , y + m_tabHeight + 1 + j);     // 7
        
                                if (j)
                                {
                                        pDC->MoveTo(x + 1 + j, y + 4 + j - 1);                          // fill corner gap
                                        pDC->LineTo(x + 4 + j, y + 1 + j - 1);                          //
                                }

                                // Shadow
                                DKPEN();
                                if(j)
                                {
                                        pDC->MoveTo(x + pTab->m_tabWidth - 4, y + 2);
                                        pDC->LineTo(x + pTab->m_tabWidth - 1, y + 5);
                                }

                                pDC->MoveTo(x + pTab->m_tabWidth - 3 - j, y + 2 + j);
                                pDC->LineTo(x + pTab->m_tabWidth - 1 - j, y + 4 + j);                           // 5
                                pDC->LineTo(x + pTab->m_tabWidth - 1 - j, y + m_tabHeight + 2 + j);     // 6
                        }
                
                        // Font
                        pDC->SelectObject(m_boldFont);
                }
                else
                {
                        // Outline
                        BKPEN();
                        pDC->MoveTo(x                                           , y + m_tabHeight);
                        pDC->LineTo(x                                           , y + 4);                       // 1
                        pDC->LineTo(x + 4                                       , y);                           // 2
                        pDC->LineTo(x + pTab->m_tabWidth - 4, y);                               // 3
                        pDC->LineTo(x + pTab->m_tabWidth        , y + 4);                       // 4
                        pDC->LineTo(x + pTab->m_tabWidth        , y + m_tabHeight);     // 5
        
                        // Hilite
                        LTPEN();
                        pDC->MoveTo(x + 1                                       , y + m_tabHeight - 1);
                        pDC->LineTo(x + 1                                       , y + 4);                       // 1
                        pDC->LineTo(x + 4                                       , y + 1);                       // 2
                        pDC->LineTo(x + pTab->m_tabWidth - 3, y + 1);                   // 3
        
                        // Shadow
                        DKPEN();
                        pDC->MoveTo(x + pTab->m_tabWidth - 3, y + 2);
                        pDC->LineTo(x + pTab->m_tabWidth - 1, y + 4);                   // 4
                        pDC->LineTo(x + pTab->m_tabWidth - 1, y + m_tabHeight); // 5
                
                        // Font
                        pDC->SelectObject(m_normalFont);
                }

            pDC->SetTextColor(GetSysColor(pTab->m_active ? COLOR_BTNTEXT:COLOR_GRAYTEXT));
                CRect rect(x, y, x + pTab->m_tabWidth, y + m_tabHeight);
                pDC->DrawText(pTab->m_tabLabel, lstrlen(pTab->m_tabLabel), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        
                x += pTab->m_tabWidth;
        }
        pDC->SetTextColor(oldTextColor);
}
void THIS::drawMSWordLeftTabs(CDC *pDC, CPen& blackPen, CPen& darkPen, CPen& lightPen, CPen *pOldPen)
{
    COLORREF oldTextColor = pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));
        int x, y, width, height;
        CRect rect;
        pDC->GetClipBox(&rect);

        width = m_width - 2 * m_margin;
        height = m_height - 2 * m_margin;

        x = m_margin;
        y = m_margin + height;
        if (m_frameBorderOn)
        {
                // draw 3 outer lines of bounding rect
                //
                BKPEN();
                pDC->MoveTo(x + m_tabHeight                             , y);
                pDC->LineTo(x + width                                   , y);
                pDC->LineTo(x + width                                   , y - height);
                pDC->LineTo(x + m_tabHeight-1                   , y - height);

                DKPEN();
                pDC->MoveTo(x + m_tabHeight                             , y - 1);
                pDC->LineTo(x + width                                   , y - 1);
                pDC->MoveTo(x + m_tabHeight                             , y - 2);
                pDC->LineTo(x + width - 1                               , y - 2);

                pDC->MoveTo(x + width - 1                               , y - 1);
                pDC->LineTo(x + width - 1                               , y - height);
                pDC->MoveTo(x + width - 2                               , y - 1);
                pDC->LineTo(x + width - 2                               , y - height + 1);

                LTPEN();
                pDC->MoveTo(x + width - 2                               , y - height + 1);
                pDC->LineTo(x + m_tabHeight + 1                 , y - height + 1);
                pDC->MoveTo(x + width - 3                               , y - height + 2);
                pDC->LineTo(x + m_tabHeight + 2                 , y - height + 2);
        }

        if(m_position == TABSONLEFT)
        {
                y = m_margin;
        }
        // now draw each tab
        //
        for (int i = 0; i < m_nTabs; i++)
        {
                CTabInfo *pTab = (CTabInfo *) m_tabArray[i];
                if(m_position == TABSONLEFT && i == 0)
                {
                        y += pTab->m_tabWidth;
                }

                if (i == m_curTab)
                {
                        // Outline
                        BKPEN();
                        pDC->MoveTo(x + m_tabHeight                     , m_margin + height);
                        pDC->LineTo(x + m_tabHeight                     , y);                                           // 1
                        pDC->LineTo(x + 4                                       , y);                                           // 2
                        pDC->LineTo(x                                           , y - 4);                                       // 3
                        pDC->LineTo(x                                           , y - pTab->m_tabWidth + 4);// 4
                        pDC->LineTo(x + 4                                       , y - pTab->m_tabWidth);        // 5
                        pDC->LineTo(x + m_tabHeight                     , y - pTab->m_tabWidth);        // 6
                        pDC->LineTo(x + m_tabHeight                     , m_margin);                            // 7

                        // Hilite
                        for (int j = 0; j < 2; j++)
                        {
                                LTPEN();
                                pDC->MoveTo(x + m_tabHeight + 1 + j     , m_margin + height - j - 1);
                                pDC->LineTo(x + m_tabHeight + 1 + j     , y - j - 1);                           // 1
                                DKPEN();
                                pDC->LineTo(x + 4 + j                           , y - j - 1);                           // 2
                                pDC->LineTo(x + 1 + j                           , y - j - 4);                           // 3
                                if(j)
                                {
                                        pDC->MoveTo(x + 4, y - 2);
                                        pDC->LineTo(x + 1, y - 5);
                                        pDC->MoveTo(x + 2, y - 5);
                                }
                                LTPEN();
                                pDC->LineTo(x + 1 + j                           , y - pTab->m_tabWidth + 3);// 4
                
                                pDC->MoveTo(x + m_tabHeight + 1 + j     , y - pTab->m_tabWidth +1 + j);
                                pDC->LineTo(x + m_tabHeight + 1 + j     , m_margin + j);                        // 7
        
                                if (j)
                                {
                                        pDC->MoveTo(x + 1               , y - pTab->m_tabWidth + 5);            // fill corner gap
                                        pDC->LineTo(x + 5               , y - pTab->m_tabWidth + 1);            //
                                }

                                pDC->MoveTo(x + 1 + j,y - pTab->m_tabWidth + 4 + j);
                                pDC->LineTo(x + 4 + j,y - pTab->m_tabWidth + 1 + j);                                    // 5
                                pDC->LineTo(x + m_tabHeight + 2 + j, y - pTab->m_tabWidth + 1 + j);     // 6
                        }
                
                        // Font
                        pDC->SelectObject(m_boldFont);
                }
                else
                {
                        // Outline
                        BKPEN();
                        pDC->MoveTo(x + m_tabHeight                     , y);
                        pDC->LineTo(x + 4                                       , y);                                                   // 1
                        pDC->LineTo(x                                           , y - 4);                                               // 2
                        pDC->LineTo(x                                           , y - pTab->m_tabWidth + 4);    // 3
                        pDC->LineTo(x + 4                                       , y - pTab->m_tabWidth);                // 4
                        pDC->LineTo(x + m_tabHeight                     , y - pTab->m_tabWidth);                // 5
        
                        // Hilite
                        DKPEN();
                        pDC->MoveTo(x + m_tabHeight     - 1             , y - 1);
                        pDC->LineTo(x + 4                                       , y - 1);                                               // 1
                        pDC->LineTo(x + 1                                       , y - 4);                                               // 2
                        LTPEN();
                        pDC->LineTo(x + 1                                       , y - pTab->m_tabWidth + 3);    // 3
        
                        // Shadow
                        pDC->MoveTo(x + 2                                       , y - pTab->m_tabWidth + 3);
                        pDC->LineTo(x + 4                                       , y - pTab->m_tabWidth + 1);    // 4
                        pDC->LineTo(x  + m_tabHeight            , y - pTab->m_tabWidth + 1);    // 5
                
                        // Font
                        pDC->SelectObject(m_normalFont);
                }

                // DrawText inside clipping rect doesn't seem to work for vertical orientation.
                // TextOut takes the (x,y) coordinates given, and rotates the string about that point.
                // So for our 90 degree rotation, (x,y) becomes the bottom left.
                CSize size = pDC->GetTextExtent(pTab->m_tabLabel, strlen(pTab->m_tabLabel));
                int left = x + (m_tabHeight-size.cy)/2 + 1;
                int bottom  = y - (pTab->m_tabWidth - size.cx)/2;
                int oldMode = pDC->SetBkMode(TRANSPARENT);

            pDC->SetTextColor(GetSysColor(pTab->m_active ? COLOR_BTNTEXT:COLOR_GRAYTEXT));
                pDC->TextOut(left, bottom, pTab->m_tabLabel, strlen(pTab->m_tabLabel));
                pDC->SetBkMode(oldMode);

                if(m_position == TABSONLEFT)
                        y += pTab->m_tabWidth;
                else
                        y -= pTab->m_tabWidth;
        }
        pDC->SetTextColor(oldTextColor);
}
void THIS::drawMSWordRightTabs(CDC *pDC, CPen& blackPen, CPen& darkPen, CPen& lightPen, CPen *pOldPen)
{
    COLORREF oldTextColor = pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));
        int x, y, width, height;
        CRect rect;
        pDC->GetClipBox(&rect);

        width = m_width - 2 * m_margin;
        height = m_height - 2 * m_margin;

        x = m_margin + width;
        y = m_margin + height;
        if (m_frameBorderOn)
        {
                // draw 3 outer lines of bounding rect
                //
                BKPEN();
                pDC->MoveTo(x - m_tabHeight                             , y);
                pDC->LineTo(x - width                                   , y);
                pDC->LineTo(x - width                                   , y - height);
                pDC->LineTo(x - m_tabHeight - 1                 , y - height);

                DKPEN();
                pDC->MoveTo(x - m_tabHeight                             , y - 1);
                pDC->LineTo(x - width                                   , y - 1);
                pDC->MoveTo(x - m_tabHeight                             , y - 2);
                pDC->LineTo(x - width + 1                               , y - 2);

                pDC->MoveTo(x - width + 1                               , y);
                pDC->LineTo(x - width + 1                               , y - height);
                pDC->MoveTo(x - width + 2                               , y);
                pDC->LineTo(x - width + 2                               , y - height + 1);

                LTPEN();
                pDC->MoveTo(x - width + 1                               , y - height + 1);
                pDC->LineTo(x - m_tabHeight - 2                 , y - height + 1);
                pDC->MoveTo(x - width + 1                               , y - height + 2);
                pDC->LineTo(x - m_tabHeight - 3                 , y - height + 2);
        }

        if(m_position == TABSONRIGHT)
        {
                y = m_margin;
        }
        x = m_margin + width - m_tabHeight;
        // now draw each tab
        //
        for (int i = 0; i < m_nTabs; i++)
        {
                CTabInfo *pTab = (CTabInfo *) m_tabArray[i];
                if(m_position == TABSONRIGHT && i == 0)
                {
                        y += pTab->m_tabWidth;
                }

                if (i == m_curTab)
                {
                        // Outline
                        BKPEN();
                        pDC->MoveTo(x                                   , m_margin + height);
                        pDC->LineTo(x                                           , y);                                           // 1
                        pDC->LineTo(x + m_tabHeight - 4         , y);                                           // 2
                        pDC->LineTo(x + m_tabHeight                     , y - 4);                                       // 3
                        pDC->LineTo(x + m_tabHeight                     , y - pTab->m_tabWidth + 4);// 4
                        pDC->LineTo(x + m_tabHeight - 4         , y - pTab->m_tabWidth);        // 5
                        pDC->LineTo(x                                           , y - pTab->m_tabWidth);        // 6
                        pDC->LineTo(x                                           , m_margin);                            // 7


                        // Hilite
                        for (int j = 0; j < 2; j++)
                        {
                                LTPEN();
                                pDC->MoveTo(x - 1 - j                           , m_margin + height - j - 1);
                                pDC->LineTo(x - 1 - j                           , y - j - 1);                           // 1
                                DKPEN();
                                pDC->LineTo(x + m_tabHeight - 4 - j     , y - j - 1);                           // 2
                                pDC->LineTo(x + m_tabHeight - 1 - j     , y - 4 - j);                           // 3
                                LTPEN();
                                pDC->LineTo(x + m_tabHeight - 1 - j     , y - pTab->m_tabWidth + 3);// 4
                
                                pDC->MoveTo(x - 1 - j                           , y - pTab->m_tabWidth +1 + j);
                                pDC->LineTo(x - 1 - j                           , m_margin + j);                        // 7
        
                                if (j)
                                {
                                        pDC->MoveTo(x + m_tabHeight - 3 - j     , y - 1 - j);                           // fill corner gap
                                        pDC->LineTo(x + m_tabHeight - j         , y - 4 - j);                           //
                                }

                                pDC->MoveTo(x + m_tabHeight - 2,        y - pTab->m_tabWidth + 3 + j);
                                pDC->LineTo(x + m_tabHeight - 4,        y - pTab->m_tabWidth + 1 + j);                                  // 5
                                pDC->LineTo(x - 2 - j, y - pTab->m_tabWidth + 1 + j);   // 6
                        }
                
                        // Font
                        pDC->SelectObject(m_boldFont);
                }
                else
                {
                        // Outline
                        BKPEN();
                        pDC->MoveTo(x                                           , y);
                        pDC->LineTo(x + m_tabHeight - 4         , y);                                                   // 1
                        pDC->LineTo(x + m_tabHeight                     , y - 4);                                               // 2
                        pDC->LineTo(x + m_tabHeight                     , y - pTab->m_tabWidth + 4);    // 3
                        pDC->LineTo(x + m_tabHeight - 4         , y - pTab->m_tabWidth);                // 4
                        pDC->LineTo(x                                           , y - pTab->m_tabWidth);                // 5
        
                        // Hilite
                        DKPEN();
                        pDC->MoveTo(x + 1                                       , y - 1);
                        pDC->LineTo(x + m_tabHeight - 5         , y - 1);                                               // 1
                        pDC->LineTo(x + m_tabHeight - 1         , y - 5);                                               // 2
                        LTPEN();
                        pDC->LineTo(x + m_tabHeight - 1         , y - pTab->m_tabWidth + 5);    // 3
        
                        // Shadow
                        LTPEN();
                        pDC->MoveTo(x + m_tabHeight - 2         , y - pTab->m_tabWidth + 3);
                        pDC->LineTo(x + m_tabHeight - 4         , y - pTab->m_tabWidth + 1);    // 4
                        pDC->LineTo(x                                           , y - pTab->m_tabWidth + 1);    // 5
                
                        // Font
                        pDC->SelectObject(m_normalFont);
                }

                // DrawText inside clipping rect doesn't seem to work for vertical orientation.
                // TextOut takes the (x,y) coordinates given, and rotates the string about that point.
                // So for our 90 degree rotation, (x,y) becomes the bottom left.
                CSize size = pDC->GetTextExtent(pTab->m_tabLabel, strlen(pTab->m_tabLabel));
                int left = x + m_tabHeight - (m_tabHeight-size.cy)/2 + 1;
                int top  = y - pTab->m_tabWidth + (pTab->m_tabWidth - size.cx)/2;
                int oldMode = pDC->SetBkMode(TRANSPARENT);

            pDC->SetTextColor(GetSysColor(pTab->m_active ? COLOR_BTNTEXT:COLOR_GRAYTEXT));
                pDC->TextOut(left, top, pTab->m_tabLabel, strlen(pTab->m_tabLabel));
                pDC->SetBkMode(oldMode);

                if(m_position == TABSONRIGHT)
                        y += pTab->m_tabWidth;
                else
                        y -= pTab->m_tabWidth;
        }
        pDC->SetTextColor(oldTextColor);
}
/////////////////////////////////////////////////////////////////////////////
// CMapView diagnostics

#ifdef _DEBUG
void THIS::AssertValid() const
{
        BASE::AssertValid();
}

void THIS::Dump(CDumpContext& dc) const
{
        CView::Dump(dc);
}
#endif //_DEBUG


void THIS::OnSetFocus(CWnd* pOldWnd)
{
        CView::OnSetFocus(pOldWnd);
        
        if (m_curView)
        {
                // Set the keyboard focus to this view
                m_curView->SetFocus();
                GetParentFrame()->SetActiveView(m_curView);
                return;
        }
        
}

int THIS::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
        return MA_ACTIVATE;
}

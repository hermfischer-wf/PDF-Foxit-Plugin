#include "StdAfx.h"
#include "arellepanelview.h"

CArellePanelView::CArellePanelView(FPD_Document pPDFDoc)
{
    m_pPDFDoc = pPDFDoc;
    m_pArellePanel = nullptr;
    m_hOwner = NULL;
    m_bFirstActive = FALSE;
}

FS_HWND CArellePanelView::GetHwnd()
{
    if(nullptr == m_pArellePanel)
    {
        this->Create(m_hOwner);
    }

    FS_BOOL bResult = IsWindow(m_pArellePanel->GetSafeHwnd());
    if (!bResult)
    {
        return NULL;
    }
    return m_pArellePanel->GetSafeHwnd();
}

void CArellePanelView::OnPanelActive()
{
    if (FALSE == m_bFirstActive)
    {
        ASSERT(m_hOwner);
        if(nullptr == m_pArellePanel)
        {
            this->Create(m_hOwner);
        }
        m_bFirstActive = TRUE;
    }
}

void CArellePanelView::Init(FPD_Document pDoc, FS_HWND hParent)
{
    if(!pDoc)
        return;
    m_pPDFDoc = pDoc;
    m_hOwner = hParent;
}

FS_BOOL CArellePanelView::Create(FS_HWND hParent)
{
    if(!m_pPDFDoc) return FALSE;
    int nCount = FRAppCountDocsOfPDDoc();
    FR_Document pTempDoc = NULL;
    for(int i = 0; i < nCount; i++)
    {
        pTempDoc = FRAppGetDocOfPDDoc(i);
        if(FRDocGetPDDoc(pTempDoc) == m_pPDFDoc)
        {
            break;
        }
    }
    if(!pTempDoc)
    {
        return FALSE;
    }

    m_pArellePanel = new CNavPage(FRAppGetActiveDocOfPDDoc());
    if (!m_pArellePanel->Create(NULL,
        L"Bookmark",
        FWS_ADDTOTITLE | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        CRect(0, 0, 500, 500), CWnd::FromHandle(hParent),
        ID_PAGE))
    {
        delete m_pArellePanel;
        m_pArellePanel = NULL;
        return FALSE;
    }


    m_hOwner = hParent;
    return TRUE;
}

FPD_Document CArellePanelView::GetPDFDoc()
{
    return m_pPDFDoc;
}

FS_HWND CArellePanelView::GetParentWnd()
{
    return m_hOwner;
}

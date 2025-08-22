#include "StdAfx.h"
#include "arelletoolhander.h"
#include "resource.h"

FS_DIBitmap      CArelleToolHander::m_pPanelIcon = NULL;

std::vector<CArellePanelView*> CArelleToolHander::m_PanelViewArray;

/*Get the icon from file.*/
FS_DIBitmap GetBmpFromRes(UINT uID)
{
    HINSTANCE hInstOld = AfxGetResourceHandle();
    AfxSetResourceHandle(AfxGetInstanceHandle());
    HRSRC hRes = ::FindResource(AfxGetInstanceHandle(), MAKEINTRESOURCE(uID), _T("pdf"));
    if (hRes == NULL) return NULL;
    DWORD dwLength = ::SizeofResource(AfxGetInstanceHandle(), hRes);
    HGLOBAL hGlobal = ::LoadResource(AfxGetInstanceHandle(), hRes);
    if (hGlobal == NULL) return NULL;
    LPVOID pData = ::LockResource(hGlobal);
    if (pData == NULL) return NULL;
    AfxSetResourceHandle(hInstOld);

    //create bmp
    FS_DIBitmap pBmp = FSDIBitmapNew();
    FSDIBitmapCreate(pBmp, 24, 24, FS_DIB_Argb, NULL, 0);
    FSDIBitmapClear(pBmp, 0xffffff);
    FPD_RenderDevice pDC = FPDFxgeDeviceNew();
    FPDFxgeDeviceAttach(pDC, pBmp, 0);
    FPD_Document pPDFDoc = FPDDocOpenMemDocument(pData, dwLength, NULL);
    FPD_Object pDict = FPDDocGetPage(pPDFDoc, 0);
    FPD_Page pPage = FPDPageNew();
    FPDPageLoad(pPage, pPDFDoc, pDict, TRUE);
    FPDPageParseContent(pPage, NULL);
    FS_AffineMatrix matrix = FPDPageGetDisplayMatrix(pPage, 0, 0, 24, 24, 0);
    FPD_RenderContext pContext = FPDRenderContextNew(pPage, TRUE);
    FPDRenderContextAppendPage(pContext, pPage, matrix);
    FPDRenderContextRender(pContext, pDC, NULL, NULL);
    FPDRenderContextDestroy(pContext);
    FPDPageDestroy(pPage);
    FPDDocClose(pPDFDoc);
    FPDFxgeDeviceDestroy(pDC);

    return pBmp;
}

CArelleToolHander::CArelleToolHander()
{
}

CArelleToolHander::~CArelleToolHander()
{
    if(m_pPanelIcon)
        FSDIBitmapDestroy(m_pPanelIcon);
}

FS_LPSTR CArelleToolHander::Tool_GetName(FS_LPVOID clientData)
{
    return "Arelle_TestName";
}

FS_LPCWSTR CArelleToolHander::Tool_GetTitle(FS_LPVOID clientData)
{
    return L"Arelle_TestTitle";
}

void CArelleToolHander::Tool_InitNewView(FS_LPVOID clientData, FPD_Document pDocument, FS_HWND hParent)
{
    CArellePanelView* pView = new CArellePanelView(pDocument);
    if (pView)
    {
        pView->Init(pDocument, hParent);
        m_PanelViewArray.push_back(pView);
    }
    return;
}

void CArelleToolHander::Tool_CloseView(FS_LPVOID clientData, FS_HWND window, FPD_Document doc)
{
    int nCount = m_PanelViewArray.size();
    for (int i=0; i<nCount; i++)
    {
        CArellePanelView* pPanelView = m_PanelViewArray.at(i);
        if(pPanelView->GetPDFDoc() == doc)
        {
            m_PanelViewArray.erase(m_PanelViewArray.begin() + i);
            return;
        }
    }
}

void CArelleToolHander::Tool_SetActiveView(FS_LPVOID clientData, FPD_Document doc, FS_HWND window)
{

}

void CArelleToolHander::Tool_GetButtonTip(FS_LPVOID clientData, FS_WideString *csOutTip)
{
    FSWideStringFill(*csOutTip, L"Arelle_TestButtonTip");
}

FS_DIBitmap CArelleToolHander::Tool_PanelViewGetButtonIconUnion(FS_LPVOID clientData, FS_APPMODE mode, FS_BTNSTATE state, FS_ICONSIZE size)
{
    if (m_pPanelIcon == NULL)
    {
        m_pPanelIcon = GetBmpFromRes(IDR_ICON_ARELLE);
    }

    return m_pPanelIcon;
}

FS_HWND CArelleToolHander::Tool_PanelViewOnGetHwnd(FS_LPVOID clientData, FPD_Document doc, FS_HWND parent)
{
    CArellePanelView* pView = Tool_GetView(doc, parent);
    if(pView)
      return  pView->GetHwnd();
    return NULL;
}

FS_HWND CArelleToolHander::Tool_PanelViewOnPanelActive(FS_LPVOID clientData, FPD_Document doc, FS_HWND parent)
{
    CArellePanelView* pView = Tool_GetView(doc, parent);
    if(pView)
    {
       pView->OnPanelActive();
       return  pView->GetHwnd();
    }
    return NULL;
}

CArellePanelView *CArelleToolHander::Tool_GetView(FPD_Document pDocument, FS_HWND hParent)
{
    CArellePanelView* pView = NULL;
    int count = m_PanelViewArray.size();
    for(int i = 0; i < count; i ++)
    {
        CArellePanelView* ArellePanelView = m_PanelViewArray.at(i);
        if( ArellePanelView->GetPDFDoc() == pDocument && ArellePanelView->GetParentWnd()  == hParent)
        {
            pView = ArellePanelView;
            break;
        }
    }
    return pView;
}


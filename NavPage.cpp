#include "StdAfx.h"
#include "NavPage.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMyTreeCtrl, CTreeCtrl)	
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_MESSAGE(SAMPLE_ARELLE_USERMSG_SHOWBOOKMARK, &CMyTreeCtrl::OnShowBookmark)
	ON_MESSAGE(SAMPLE_ARELLE_USERMSG_DELETEBOOKMARK, &CMyTreeCtrl::OnDeleteBookmark)
END_MESSAGE_MAP()

CMyTreeCtrl::CMyTreeCtrl(FR_Document pDoc)
{
	m_pDoc = pDoc;
	m_pBookmarkOperation = NULL;

	m_mapItemsObject.clear();
	m_mapItemsBookmark.clear();
}

CMyTreeCtrl::~CMyTreeCtrl()
{
	if (m_pBookmarkOperation != NULL)
	{
		std::map<HTREEITEM, FPD_Bookmark>::iterator it = m_mapItemsBookmark.begin();
		while (it != m_mapItemsBookmark.end())
		{
			m_pBookmarkOperation->DestroyBookmark((*it).second);
			++it;
		}
	}
}

void CMyTreeCtrl::AddChildBookmark()
{
	HTREEITEM hItem = this->GetSelectedItem();
	if (hItem == NULL)
	{
		m_pBookmarkOperation->AddChildBookmark(nullptr, "Untitled");
	}
	else
	{
		FPD_Bookmark pBookmark = GetCorrespondBookmark(hItem);
		m_pBookmarkOperation->AddChildBookmark(pBookmark, "Untitled");
	}
	this->Invalidate();
}

void CMyTreeCtrl::AddSiblingBookmark()
{
	HTREEITEM hItem = this->GetSelectedItem();
	if (hItem == nullptr)
	{
		::AfxMessageBox(L"No sibling item selected, please select sibling item first.");
	}
	else
	{
		FPD_Bookmark pBookmark = GetCorrespondBookmark(hItem);
		m_pBookmarkOperation->AddSiblingBookmark(pBookmark, "Untitled");
	}
	this->Invalidate();
}

void CMyTreeCtrl::DeleteBookmark()
{
	HTREEITEM hItem = this->GetSelectedItem();
	if (hItem == nullptr)
	{
		::AfxMessageBox(L"No item selected, please select item first.");
	}
	else
	{
		FPD_Bookmark pBookmark = GetCorrespondBookmark(hItem);
		m_pBookmarkOperation->DeleteBookmarkAndChild(pBookmark);
	}
	this->Invalidate();
}

HTREEITEM CMyTreeCtrl::GetCorrespondItem(FPD_Object hBookmarkDic)
{
	if (hBookmarkDic == nullptr)
	{
		return nullptr;
	}

	std::map<FPD_Object, HTREEITEM>::iterator it = m_mapItemsObject.find(hBookmarkDic);
	if (it != m_mapItemsObject.end())
	{
		return (*it).second;
	}
	else
	{
		return nullptr;
	}

}

FPD_Bookmark CMyTreeCtrl::GetCorrespondBookmark(HTREEITEM hItem)
{
	if (hItem == nullptr)
	{
		return nullptr;
	}


	std::map<HTREEITEM, FPD_Bookmark>::iterator it = m_mapItemsBookmark.find(hItem);
	if (it != m_mapItemsBookmark.end())
	{
		return (*it).second;
	}
	else
	{
		return nullptr;
	}

}

void CMyTreeCtrl::OnSize(UINT nType, int cx, int cy)
{
	CTreeCtrl::OnSize(nType, cx, cy);
}

void CMyTreeCtrl::OnDestroy()
{
	CTreeCtrl::OnDestroy();
}

void CMyTreeCtrl::OnPaint()
{
	CTreeCtrl::OnPaint();
}

void CMyTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CTreeCtrl::OnLButtonDown(nFlags, point);

	if (m_pBookmarkOperation != NULL)
	{
		HTREEITEM hItem = this->GetSelectedItem();
		FPD_Bookmark pBookmark = GetCorrespondBookmark(hItem);
		m_pBookmarkOperation->GotoBookmarkDest(pBookmark);
	}
}

int  CMyTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(-1 == CTreeCtrl::OnCreate(lpCreateStruct))
		return -1;
	
	m_pBookmarkOperation = new Bookmark_Operation(this->m_hWnd);
	m_pBookmarkOperation->InitBookMark(m_pDoc);
	return 0;
	
}
LRESULT CMyTreeCtrl::OnShowBookmark(WPARAM wParam, LPARAM lParam)
{
	//Get dictionary entry of input bookmark
	FPD_Bookmark hBookmark = (FPD_Bookmark)wParam;
	FPD_Object hBookmarkDic = FPDBookmarkGetDictionary(hBookmark);
    //Get the parent bookmark dictionary item of the input bookmark
	FPD_Object hParentDic = FPDDictionaryGetDict(hBookmarkDic, "Parent");
    //Get the last bookmark dictionary item of the input bookmark
	FPD_Object hPreDic = FPDDictionaryGetDict(hBookmarkDic, "Prev");

    //Get the title of the input bookmark
	FS_WideString wsTitle = FSWideStringNew();
	FPDBookmarkGetTitle(hBookmark, &wsTitle);
	CString strTitle = FSWideStringCastToLPCWSTR(wsTitle);
	FSWideStringDestroy(wsTitle);

	//Create a new item
	TVINSERTSTRUCT tvInsert;
	tvInsert.item.mask = TVIF_TEXT;
	//set item title
	tvInsert.item.pszText = strTitle.GetBuffer();
	
    //Get the item corresponding to the parent bookmark
	HTREEITEM pParentItem = GetCorrespondItem(hParentDic);
    //Get the item corresponding to the previous bookmark
	HTREEITEM pPreItem = GetCorrespondItem(hPreDic);
	//Set the item's parent object and the previous sibling object
	if (pParentItem == nullptr && pPreItem == nullptr)
	{
		tvInsert.hParent = NULL;
		tvInsert.hInsertAfter = NULL;
	}
	else if (pParentItem == nullptr)
	{
		tvInsert.hParent = NULL;
		tvInsert.hInsertAfter = pPreItem;
	}
	else if (pPreItem == nullptr)
	{
		tvInsert.hParent = pParentItem;
		tvInsert.hInsertAfter = NULL;
	}
	else
	{
		tvInsert.hParent = pParentItem;
		tvInsert.hInsertAfter = pPreItem;
	}
	
	//Add item to the tree structure
	HTREEITEM hItem = this->InsertItem(&tvInsert);
	
	//Update and record the bookmark record map
	m_mapItemsObject.insert(make_pair(hBookmarkDic, hItem));
	m_mapItemsBookmark.insert(make_pair(hItem, hBookmark));

	return 0;
}
LRESULT CMyTreeCtrl::OnDeleteBookmark(WPARAM wParam, LPARAM lParam)
{
	//Get dictionary entry of input bookmark
	FPD_Bookmark hBookmark = (FPD_Bookmark)wParam;
	FPD_Object hBookmarkDic = FPDBookmarkGetDictionary(hBookmark);
    //Get the item corresponding to the bookmark dictionary
	HTREEITEM pItem = GetCorrespondItem(hBookmarkDic);

	//Delete item from the tree structure
	if (pItem != nullptr)
	{
		this->DeleteItem(pItem);

        //Update and record the bookmark record map
		m_mapItemsObject.erase(hBookmarkDic);
		m_mapItemsBookmark.erase(pItem);
	}

    //Remove data layer bookmark information
	m_pBookmarkOperation->DestroyBookmark(hBookmark);

	return 0;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CNavPage, CWnd)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_MYBTN_ADDCHILD, &CNavPage::AddChildBookmark)
	ON_BN_CLICKED(IDC_MYBTN_ADDSIBLING, &CNavPage::AddSiblingBookmark)
	ON_BN_CLICKED(IDC_MYBTN_DELETE, &CNavPage::DeleteBookmark)
END_MESSAGE_MAP()

CNavPage::CNavPage(FR_Document pDoc)
{
	m_pAddChildBtn = new CButton();
	m_pAddSiblingBtn = new CButton();
	m_pDeleteBtn = new CButton();
	m_pTreeCtrl = new CMyTreeCtrl(pDoc);
}

CNavPage::~CNavPage()
{
	delete m_pAddChildBtn;
	delete m_pAddSiblingBtn;
	delete m_pDeleteBtn;
	delete m_pTreeCtrl;
}

void CNavPage::OnSize(UINT nType, int cx, int cy)
{
	m_pAddChildBtn->MoveWindow(20, 10, cx-40, 20);
	m_pAddSiblingBtn->MoveWindow(20, 40, cx - 40, 20);
	m_pDeleteBtn->MoveWindow(20, 70, cx - 40, 20);
	m_pTreeCtrl->MoveWindow(0, 100, cx, cy-100);
	CWnd::OnSize(nType, cx, cy)	;
}

void CNavPage::OnDestroy()
{
	m_pAddChildBtn->DestroyWindow();
	m_pAddSiblingBtn->DestroyWindow();
	m_pDeleteBtn->DestroyWindow();
	m_pTreeCtrl->DestroyWindow();
	CWnd::OnDestroy();
}

void CNavPage::OnPaint()
{
	CPaintDC dc(this);
	CRect rc;
	this->GetClientRect(&rc);
	dc.FillSolidRect(&rc, RGB(255,255,255));
}

int  CNavPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(-1 == CWnd::OnCreate(lpCreateStruct))
		return -1;

	m_pAddChildBtn->Create(_T("AddChild"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		CRect(0, 0, 0, 0), this, IDC_MYBTN_ADDCHILD);

	m_pAddSiblingBtn->Create(_T("AddSibling"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		CRect(0, 0, 0, 0), this, IDC_MYBTN_ADDSIBLING);

	m_pDeleteBtn->Create(_T("Delete"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		CRect(0, 0, 0, 0), this, IDC_MYBTN_DELETE);

	m_pTreeCtrl->Create(WS_VISIBLE | WS_TABSTOP | WS_CHILD | WS_BORDER
		| TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES
		| TVS_DISABLEDRAGDROP,
		CRect(0, 0, 0, 0), this, IDC_MYTREECTRL);
}

void CNavPage::AddChildBookmark()
{
	m_pTreeCtrl->AddChildBookmark();
}

void CNavPage::AddSiblingBookmark()
{
	m_pTreeCtrl->AddSiblingBookmark();
}

void CNavPage::DeleteBookmark()
{
	m_pTreeCtrl->DeleteBookmark();
}




#ifndef _NAVPAGE_H_
#define _NAVPAGE_H_

#include "arelle_operation.h"
#include <map>

using namespace std;

//BOOL __declspec(dllexport) FX_InitBookmarkPanel(HMODULE hModule, IFX_PanelToolMgr* pMgr )
#define  ID_PAGE 65003
class CMyTreeCtrl;
class CNavPage;

class CMyTreeCtrl : public CTreeCtrl
{
virtual ULONG GetGestureStatus(CPoint ptTouch) { return 0;}
	
public:
	CMyTreeCtrl(FR_Document pDoc);
	virtual ~CMyTreeCtrl();
	
	void AddChildBookmark();
	void AddSiblingBookmark();
	void DeleteBookmark();

	HTREEITEM GetCorrespondItem(FPD_Object hBookmarkDic);
	FPD_Bookmark GetCorrespondBookmark(HTREEITEM hItem);

	DECLARE_MESSAGE_MAP()
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg LRESULT OnShowBookmark(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeleteBookmark(WPARAM wParam, LPARAM lParam);
private:
	FR_Document m_pDoc;
	Arelle_Operation* m_pArelleOperation;

	std::map<FPD_Object, HTREEITEM> m_mapItemsObject;
	std::map<HTREEITEM, FPD_Bookmark> m_mapItemsBookmark;
};

#define IDC_MYBTN_ADDCHILD		0x1001
#define IDC_MYBTN_ADDSIBLING	0x1002
#define IDC_MYBTN_DELETE		0x1003
#define IDC_MYTREECTRL			0x1004

class CNavPage : public CWnd
{
virtual ULONG GetGestureStatus(CPoint ptTouch) { return 0;}
	friend class CMyTreeCtrl;
public:
	CNavPage(FR_Document pDoc);
	virtual ~CNavPage();

	DECLARE_MESSAGE_MAP()
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	
	afx_msg void AddChildBookmark();
	afx_msg void AddSiblingBookmark();
	afx_msg void DeleteBookmark();
private:
	CButton *m_pAddChildBtn;
	CButton *m_pAddSiblingBtn;
	CButton *m_pDeleteBtn;
	CMyTreeCtrl	*m_pTreeCtrl;

};

#endif
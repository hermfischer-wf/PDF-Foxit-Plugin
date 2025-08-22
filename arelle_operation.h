#ifndef ARELLE_OPERATION_H
#define ARELLE_OPERATION_H

#include "stdafx.h"

#define SAMPLE_ARELLE_USERMSG_SHOWBOOKMARK (WM_USER+100)
#define SAMPLE_ARELLE_USERMSG_DELETEBOOKMARK (WM_USER+200)

class Arelle_Operation 
{
public:
    Arelle_Operation(HWND hNavPage);

    void InitBookMark(FR_Document pDoc);
    FPD_Bookmark InitDocRootBookmark();
    void DestroyBookmark(FPD_Bookmark hBookmark);
    FPD_Bookmark AddChildBookmark(FPD_Bookmark hParent, FS_LPCSTR csTitle);
    FPD_Bookmark AddSiblingBookmark(FPD_Bookmark hPreBookmark, FS_LPCSTR csTitle);
    FPD_Bookmark FindBookmark(FPD_Bookmark hParent, FS_LPCSTR csTitle);
    void GotoBookmarkDest(FPD_Bookmark hBookmark);

    void DeleteBookmark(FPD_Bookmark hDeleteBookmark);
    void ShowAllChildBookmark(FPD_Bookmark hParent);
    void DeleteBookmarkAndChild(FPD_Bookmark hParent);

    FPD_Object AddNewBookmarkObject(FPD_Object hParentDic, FS_LPCSTR csTitle);
    void AddParentBookmarkCount(FPD_Object hParentDic);
    void DeleteParentBookmarkCount(FPD_Object hParentDic);

private:
	HWND				m_hNavPage;
    FR_Document         m_pDoc;
    FPD_Document        m_pDestDoc;
    FPD_Bookmark        m_pRootBookmark;
};

#endif // ARELLE_OPERATION_H

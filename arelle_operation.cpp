#include "stdafx.h"
#include "arelle_operation.h"
#include <atlbase.h>

Arelle_Operation::Arelle_Operation(HWND hNavPage)
{
	m_hNavPage = hNavPage;
	m_pDoc = nullptr;
	m_pDestDoc = nullptr;
	m_pRootBookmark = nullptr;
}

void Arelle_Operation::InitBookMark(FR_Document pDoc)
{
    m_pDoc = pDoc;;
    m_pDestDoc = FRDocGetPDDoc(m_pDoc);

    //Get the root bookmark of the document, this item is not displayed on the page
    m_pRootBookmark = InitDocRootBookmark();

    if(m_pDestDoc == nullptr || m_pRootBookmark == nullptr)
    {
        return;
    }

    //Show all bookmarks in the document
    ShowAllChildBookmark(NULL);
}

FPD_Bookmark Arelle_Operation::InitDocRootBookmark()
{
    //Get the root bookmark of the document
    FPD_Object hRootDic = FPDDocGetRoot(m_pDestDoc);
    //Determine whether the bookmark-related dictionary item of the root bookmark exists, if it does not exist, initialize it
    if(FALSE == FPDDictionaryKeyExist(hRootDic, "Outlines"))
    {
        FPD_Object hParentDic = FPDDictionaryNew();
        FS_DWORD ParentObjNum = FPDDocAddIndirectObject(m_pDestDoc, hParentDic);
        FPDDictionarySetAtReferenceToDoc(hRootDic, "Outlines", m_pDestDoc, ParentObjNum);

        FPDDictionarySetAtName(hParentDic, "Type", "Outlines");
        FPDDictionarySetAtNumber(hParentDic, "Count", 0);

        return FPDBookmarkNew(hParentDic);
    }
    else
    {
        return FPDBookmarkNew(hRootDic);
    }
}

void Arelle_Operation::DestroyBookmark(FPD_Bookmark hBookmark)
{
    //Obtain the dictionary item information according to the input bookmark object
    FPD_Object hDeleteBookmarkDic = FPDBookmarkGetDictionary(hBookmark);

    //Release and remove the dictionary item related information recorded in the document
    FPDDocReleaseIndirectObject(m_pDestDoc, FPDObjectGetobjNum(hDeleteBookmarkDic));
    FPDDocDeleteIndirectObject(m_pDestDoc, FPDObjectGetobjNum(hDeleteBookmarkDic));

    //Delete bookmark object
    FPDBookmarkDestroy(hBookmark);
}

//Add a new bookmark to the end of the parent bookmark
FPD_Bookmark Arelle_Operation::AddChildBookmark(FPD_Bookmark hParent, FS_LPCSTR csTitle)
{
    //Create an empty bookmark object
    FPD_Bookmark pChildBookmark = FPDBookmarkNew(NULL);
    if(m_pDoc == nullptr ||m_pDestDoc == nullptr)
    {
        return pChildBookmark;
    }

    //If there is no parent bookmark object, it defaults to the first-level bookmark, which is the child of the document root bookmark
    if(hParent == nullptr)
    {
        hParent = m_pRootBookmark;
    }

    //Get dictionary information of parent bookmark
    FPD_Object hParentDic = FPDBookmarkGetDictionary(hParent);

    //Dictionary information for creating new bookmarks
    FPD_Object hNewDic = AddNewBookmarkObject(hParentDic, csTitle);
    //Add the dictionary information of the new bookmark to the document
    FS_DWORD NewBMObjnum = FPDDocAddIndirectObject(m_pDestDoc, hNewDic);

    //Get the index of the parent bookmark dictionary item in the document
    FS_DWORD ParentObjNum = FPDDocAddIndirectObject(m_pDestDoc, hParentDic);
    //Bind the parent bookmark dictionary to the new bookmark dictionary
    FPDDictionarySetAtReferenceToDoc(hNewDic, "Parent", m_pDestDoc, ParentObjNum);

    //Get the first bookmark dictionary information of the parent bookmark
    FPD_Object hFirstChildDic = FPDDictionaryGetDict(hParentDic, "First");
    if(hFirstChildDic == nullptr)
    {
        //If it does not exist, the new bookmark is the first bookmark, and the dictionary information of the parent bookmark is updated
        FPDDictionarySetAtReferenceToDoc(hParentDic, "First", m_pDestDoc, NewBMObjnum);
    }

    //Get the last bookmark dictionary information of the parent bookmark
    FPD_Object hLastChildDic = FPDDictionaryGetDict(hParentDic, "Last");
    if(hLastChildDic != nullptr)
    {
        //If it exists, the next bookmark bound to the original last bookmark is the new bookmark
        FS_DWORD LastObjNum = FPDDocAddIndirectObject(m_pDestDoc, hLastChildDic);
        FPDDictionarySetAtReferenceToDoc(hLastChildDic, "Next", m_pDestDoc, NewBMObjnum);

        //The last bookmark bound to the new bookmark is the original last bookmark
        FPDDictionarySetAtReferenceToDoc(hNewDic, "Prev", m_pDestDoc, LastObjNum);
    }

    //Update the last bookmark of the parent bookmark as a new bookmark
    FPDDictionarySetAtReferenceToDoc(hParentDic, "Last", m_pDestDoc, NewBMObjnum);

    //Update the number of child bookmarks of the parent bookmark
    AddParentBookmarkCount(hParentDic);
    //Set the document has been changed flag
    FRDocSetChangeMark(m_pDoc);
    //Construct a new bookmark according to the created new bookmark dictionary object
    pChildBookmark = FPDBookmarkNew(hNewDic);
    //Send a signal to the interface to display a new bookmark
	::SendMessage(m_hNavPage, SAMPLE_ARELLE_USERMSG_SHOWBOOKMARK, (WPARAM)pChildBookmark, NULL);

    return pChildBookmark;
}

//Add a bookmark of the same level after the entered bookmark object
FPD_Bookmark Arelle_Operation::AddSiblingBookmark(FPD_Bookmark hPreBookmark, FS_LPCSTR csTitle)
{
    //Create an empty bookmark object
    FPD_Bookmark pSiblingBookmark = FPDBookmarkNew(NULL);
    if(hPreBookmark == nullptr || m_pDoc == nullptr ||m_pDestDoc == nullptr)
    {
        return pSiblingBookmark;
    }

    //Get dictionary entry of input bookmark
    FPD_Object hPreDic = FPDBookmarkGetDictionary(hPreBookmark);
    //Get the next bookmark dictionary entry of the input bookmark
    FPD_Object hNextDic = FPDDictionaryGetDict(hPreDic, "Next");
    //Get the parent bookmark dictionary item of the input bookmark
    FPD_Object hParentDic = FPDDictionaryGetDict(hPreDic, "Parent");

    //Create a new bookmark dictionary entry
    FPD_Object hNewDic = AddNewBookmarkObject(hParentDic, csTitle);
    //Write the new bookmark dictionary item to the document and get its index
    FS_DWORD NewBMObjnum = FPDDocAddIndirectObject(m_pDestDoc, hNewDic);

    //Get the index of the parent bookmark dictionary item in the document
    FS_DWORD ParentObjNum = FPDDocAddIndirectObject(m_pDestDoc, hParentDic);
    //Bind the parent bookmark dictionary to the new bookmark dictionary
    FPDDictionarySetAtReferenceToDoc(hNewDic, "Parent", m_pDestDoc, ParentObjNum);

    //Get the index of the input bookmark dictionary item in the document
    FS_DWORD PreObjNum = FPDDocAddIndirectObject(m_pDestDoc, hPreDic);
    //Update the next bookmark entered as a new bookmark
    FPDDictionarySetAtReferenceToDoc(hPreDic, "Next", m_pDestDoc, NewBMObjnum);
    //Update the previous bookmark of the new bookmark as the input bookmark
    FPDDictionarySetAtReferenceToDoc(hNewDic, "Prev", m_pDestDoc, PreObjNum);

    if(hNextDic)
    {
        //If there is the original next bookmark dictionary item, get its index in the document
        FS_DWORD NextObjNum = FPDDocAddIndirectObject(m_pDestDoc, hNextDic);
        //Update the previous bookmark of the original next bookmark to the new bookmark
        FPDDictionarySetAtReferenceToDoc(hNextDic, "Prev", m_pDestDoc, NewBMObjnum);
        //Update the next bookmark of the new bookmark to the original next bookmark
        FPDDictionarySetAtReferenceToDoc(hNewDic, "Next", m_pDestDoc, NextObjNum);
    }
    else
    {
        //If the original next bookmark does not exist, the new bookmark is the last bookmark of the parent bookmark, and the last bookmark dictionary information of the parent bookmark is updated
        FPDDictionarySetAtReferenceToDoc(hParentDic, "Last", m_pDestDoc, NewBMObjnum);
    }

    //Update the number of child bookmarks of the parent bookmark
    AddParentBookmarkCount(hParentDic);
    //Set the document has been changed flag
    FRDocSetChangeMark(m_pDoc);
    //Construct a new bookmark according to the created new bookmark dictionary object
    pSiblingBookmark = FPDBookmarkNew(hNewDic);
    //Send a signal to the interface to display a new bookmark
	::SendMessage(m_hNavPage, SAMPLE_ARELLE_USERMSG_SHOWBOOKMARK, (WPARAM)pSiblingBookmark, NULL);

    return pSiblingBookmark;
}

FPD_Bookmark Arelle_Operation::FindBookmark(FPD_Bookmark hParent, FS_LPCSTR csTitle)
{
    USES_CONVERSION;

    FS_WideString wsTitle = FSWideStringNew();
    FPD_Bookmark pChild = FPDBookmarkNew(NULL);
    //Get the first bookmark of the input bookmark
    FS_BOOL bGet = FPDBookmarkGetFirstChild(m_pDestDoc, hParent, &pChild);
    if(bGet)
    {
        //Get the bookmark name and compare it
        FPDBookmarkGetTitle(pChild, &wsTitle);
        CString strTitle = FSWideStringCastToLPCWSTR(wsTitle);
        if(strTitle.Compare(A2W(csTitle)) == 0)
        {
            return pChild;
        }
        else
        {
            //Recursive call, traverse the sub-items to find
            FPD_Bookmark pBookmark = FindBookmark(pChild, csTitle);
            if(FPDBookmarkIsVaild(pBookmark))
            {
                return pBookmark;
            }
        }

        while(1)
        {
            //Get the next bookmark of the same level
            FPD_Bookmark pNextChild = FPDBookmarkNew(NULL);
            bGet = FPDBookmarkGetNextSibling(m_pDestDoc, pChild, &pNextChild);
            if(bGet)
            {
                //Get the bookmark name and compare it
                FPDBookmarkGetTitle(pNextChild, &wsTitle);
                strTitle = FSWideStringCastToLPCWSTR(wsTitle);
                if(strTitle.Compare(A2W(csTitle)) == 0)
                {
                    return pNextChild;
                }
                else
                {
                    //Recursive call, traverse the sub-items to find
                    FPD_Bookmark pBookmark = FindBookmark(pNextChild, csTitle);
                    if(FPDBookmarkIsVaild(pBookmark))
                    {
                        return pBookmark;
                    }
                }

                pChild = pNextChild;
            }
            else
            {
                break;
            }
        }
    }

    FSWideStringDestroy(wsTitle);
    return pChild;
}

void Arelle_Operation::GotoBookmarkDest(FPD_Bookmark hBookmark)
{
    if(m_pDoc == nullptr || m_pDestDoc == nullptr)
    {
        return;
    }

    //Judging the validity of the entered bookmark
    if(FALSE == FPDBookmarkIsVaild(hBookmark))
    {
        return;
    }

    //Get bookmark target information
    FPD_Dest hDest = FPDDestNew(NULL);
    FPDBookmarkGetDest(hBookmark, m_pDestDoc, &hDest);
    //Jump document view to target information location
    FRDocViewGotoPageByDest(FRDocGetCurrentDocView(m_pDoc), hDest);
}

void Arelle_Operation::DeleteBookmark(FPD_Bookmark hDeleteBookmark)
{
    //Judging the validity of the entered bookmark
    if(FALSE == FPDBookmarkIsVaild(hDeleteBookmark))
    {
        return;
    }

    //Get dictionary entry of input bookmark
    FPD_Object hDeleteBookmarkDic = FPDBookmarkGetDictionary(hDeleteBookmark);
    //Get the last bookmark dictionary item of the input bookmark
    FPD_Object hPreBookmarkDic = FPDDictionaryGetDict(hDeleteBookmarkDic, "Prev");
    //Get the next bookmark dictionary entry of the input bookmark
    FPD_Object hNextBookmarkDic = FPDDictionaryGetDict(hDeleteBookmarkDic, "Next");
    //Get the parent bookmark dictionary item of the input bookmark
    FPD_Object hParentDic = FPDDictionaryGetDict(hDeleteBookmarkDic, "Parent");

    if(hPreBookmarkDic == nullptr  && hNextBookmarkDic == nullptr)
    {
        //If the input bookmark is the only item of the parent bookmark, remove the child item information of the parent bookmark
        FPDDictionaryRemoveAt(hParentDic, "First");
        FPDDictionaryRemoveAt(hParentDic, "Last");
        FPDDictionaryRemoveAt(hParentDic, "Count");
    }
    else
    {
        if (hPreBookmarkDic == nullptr) //If the input bookmark is the first item of the parent bookmark
        {
            //Remove the previous bookmark dictionary information of the original next bookmark
            FPDDictionaryRemoveAt(hNextBookmarkDic, "Prev");

            //Get the index of the original next bookmark dictionary item in the document
            FS_DWORD NextObjNum = FPDDocAddIndirectObject(m_pDestDoc, hNextBookmarkDic);
            //Update the first bookmark of the parent bookmark to the original next bookmark
            FPDDictionarySetAtReferenceToDoc(hParentDic, "First", m_pDestDoc, NextObjNum);
        }
        else if(hNextBookmarkDic == nullptr) //If the input bookmark is the last item of the parent bookmark
        {
            //Remove the next bookmark dictionary information of the previous bookmark
            FPDDictionaryRemoveAt(hPreBookmarkDic, "Next");
            //Get the index of the previous bookmark dictionary item in the document
            FS_DWORD PreObjNum = FPDDocAddIndirectObject(m_pDestDoc, hPreBookmarkDic);
            //Update the last bookmark in the parent bookmark to the original previous bookmark
            FPDDictionarySetAtReferenceToDoc(hParentDic, "Last", m_pDestDoc, PreObjNum);
        }
        else //If the input bookmark is the middle child of the parent bookmark
        {
            //Update the next bookmark in the original previous bookmark to the next bookmark of the input bookmark
            FS_DWORD PreObjNum = FPDDocAddIndirectObject(m_pDestDoc, hPreBookmarkDic);
            FPDDictionarySetAtReferenceToDoc(hNextBookmarkDic, "Prev", m_pDestDoc, PreObjNum);

            //Update the previous bookmark in the original next bookmark to the previous bookmark of the input bookmark
            FS_DWORD NextObjNum = FPDDocAddIndirectObject(m_pDestDoc, hNextBookmarkDic);
            FPDDictionarySetAtReferenceToDoc(hPreBookmarkDic, "Next", m_pDestDoc, NextObjNum);
        }

        //Update the number of child bookmarks of the parent bookmark
        DeleteParentBookmarkCount(hParentDic);
    }

    //Set the document has been changed flag
    FRDocSetChangeMark(m_pDoc);
	//Send a signal to the interface to remove the bookmark
	::SendMessage(m_hNavPage, SAMPLE_ARELLE_USERMSG_SHOWBOOKMARK, (WPARAM)hDeleteBookmark, NULL);
}

void Arelle_Operation::ShowAllChildBookmark(FPD_Bookmark hParent)
{
    if(hParent != NULL)
    {
        //Judging the validity of the entered bookmark
        if(FALSE == FPDBookmarkIsVaild(hParent))
        {
            return;
        }
    }

    //Get the first bookmark of the input bookmark
    //When the input bookmark is NULL, it represents the root bookmark
    FPD_Bookmark pChild = FPDBookmarkNew(NULL);
    FS_BOOL bGet = FPDBookmarkGetFirstChild(m_pDestDoc, hParent, &pChild);
    if(bGet)
    {
		//Send a signal to the interface to display a new bookmark
		::SendMessage(m_hNavPage, SAMPLE_ARELLE_USERMSG_SHOWBOOKMARK, (WPARAM)pChild, NULL);
        //Recursive call to show children
        ShowAllChildBookmark(pChild);
	
        while(bGet)
        {
			bGet = FALSE;
			//Get the next bookmark of the same level
            FPD_Bookmark pNextChild = FPDBookmarkNew(NULL);
            bGet = FPDBookmarkGetNextSibling(m_pDestDoc, pChild, &pNextChild);
            if(bGet)
            {
                pChild = pNextChild;
				//Send a signal to the interface to display a new bookmark
				::SendMessage(m_hNavPage, SAMPLE_ARELLE_USERMSG_SHOWBOOKMARK, (WPARAM)pNextChild, NULL);
                //Recursive call to show children
                ShowAllChildBookmark(pNextChild);
            }
            else
            {
                break;
            }
        }
    }
}

void Arelle_Operation::DeleteBookmarkAndChild(FPD_Bookmark hParent)
{
    //Judging the validity of the entered bookmark
    if(FALSE == FPDBookmarkIsVaild(hParent))
    {
        return;
    }

    //Get the first bookmark of the input bookmark
    FPD_Bookmark pChild = FPDBookmarkNew(NULL);
    FS_BOOL bGet = FPDBookmarkGetFirstChild(m_pDestDoc, hParent, &pChild);
    if(bGet)
    {
        while(1)
        {
            //Get the next bookmark of the same level
            FPD_Bookmark pNextChild = FPDBookmarkNew(NULL);
            bGet = FPDBookmarkGetNextSibling(m_pDestDoc, pChild, &pNextChild);
            //Recursive call to delete children
            DeleteBookmarkAndChild(pChild);

            if(bGet)
            {
                pChild = pNextChild;
            }
            else
            {
                break;
            }
        }
    }

    //Delete bookmark related information
    DeleteBookmark(hParent);
}

FPD_Object Arelle_Operation::AddNewBookmarkObject(FPD_Object hParentDic, FS_LPCSTR csTitle)
{
    //Gets the state of the curren page
    FS_INT32 nPage = -1;
    FR_PAGESTATE state;
    FRDocGetTopPageState(m_pDoc, &nPage, &state);

    //Construct bookmark target dictionary information according to page state
    FPD_Object hNewDicDest = FPDArrayNew();
    FS_DWORD PageObjNum = FPDDocAddIndirectObject(m_pDestDoc, FPDDocGetPage(m_pDestDoc, nPage));
    FPDArrayAddReferenceToDoc(hNewDicDest, m_pDestDoc, PageObjNum);
    switch (state.nFitType)
    {
        case FPD_ZOOM_XYZ:
            {
                FPDArrayAddName(hNewDicDest, "XYZ");
                FPDArrayAddNumber(hNewDicDest, FSFloatArrayGetAt(state.dest, 0));
                FPDArrayAddNumber(hNewDicDest, FSFloatArrayGetAt(state.dest, 1));
                FPDArrayAddNumber(hNewDicDest, FSFloatArrayGetAt(state.dest, 2));
            }
            break;
        case FPD_ZOOM_FITPAGE:
            {
                FPDArrayAddName(hNewDicDest, "Fit");
            }
            break;
        case FPD_ZOOM_FITHORZ:
            {
                FPDArrayAddName(hNewDicDest, "FitH");
                FPDArrayAddNumber(hNewDicDest, FSFloatArrayGetAt(state.dest, 0));
            }
            break;
        case FPD_ZOOM_FITVERT:
            {
                FPDArrayAddName(hNewDicDest, "FitV");
                FPDArrayAddNumber(hNewDicDest, FSFloatArrayGetAt(state.dest, 0));
            }
            break;
        case FPD_ZOOM_FITRECT:
            {
                FPDArrayAddName(hNewDicDest, "FitR");
                FPDArrayAddNumber(hNewDicDest, FSFloatArrayGetAt(state.dest, 0));
                FPDArrayAddNumber(hNewDicDest, FSFloatArrayGetAt(state.dest, 1));
                FPDArrayAddNumber(hNewDicDest, FSFloatArrayGetAt(state.dest, 2));
                FPDArrayAddNumber(hNewDicDest, FSFloatArrayGetAt(state.dest, 3));
            }
            break;
        case FPD_ZOOM_FITBHORZ:
            {
                FPDArrayAddName(hNewDicDest, "FitBH");
                FPDArrayAddNumber(hNewDicDest, FSFloatArrayGetAt(state.dest, 0));
            }
            break;
        default:
            break;
    }

    //Construct bookmark action dictionary information according to bookmark target dictionary information
    FPD_Object hNewDicAction = FPDDictionaryNew();
    FPDDictionarySetAtName(hNewDicAction, "Type", "Action");
    FPDDictionarySetAtName(hNewDicAction, "S", "GoTo");
    FS_DWORD NewDestObjNum = FPDDocAddIndirectObject(m_pDestDoc, hNewDicDest);
    FPDDictionarySetAtReferenceToDoc(hNewDicAction, "D", m_pDestDoc, NewDestObjNum);

    //Create a new bookmark dictionary object
    FPD_Object hNewDic = FPDDictionaryNew();

    //Set new bookmark title
    FS_ByteString bsTitle = FSByteStringNew();;
    FSByteStringFill(bsTitle, csTitle);
    FPDDictionarySetAtString(hNewDic,"Title", bsTitle);
    FSByteStringDestroy(bsTitle);
    //Set new bookmark action dictionary information
    FS_DWORD NewActionObjNum = FPDDocAddIndirectObject(m_pDestDoc, hNewDicAction);
    FPDDictionarySetAtReferenceToDoc(hNewDic, "A", m_pDestDoc, NewActionObjNum);
    //Set new bookmark parent dictionary information
    FS_DWORD ParentObjNum = FPDDocAddIndirectObject(m_pDestDoc, hParentDic);
    FPDDictionarySetAtReferenceToDoc(hNewDic, "Parent", m_pDestDoc, ParentObjNum);

    return hNewDic;
}

void Arelle_Operation::AddParentBookmarkCount(FPD_Object hParentDic)
{
    //Get the dictionary information of the number of children of the input object
    //The plus or minus of the number of children indicates whether the bookmark is in the expanded state
    int count = FPDDictionaryGetInteger(hParentDic, "Count");
    if(FPDDictionaryKeyExist(hParentDic, "Title"))
    {
        if (count <= 0)
        {
            count -= 1;
        }
        else
        {
            count += 1;
        }
    }
    else
    {
        if (count < 0)
        {
            count -= 1;
        }
        else
        {
            count += 1;
        }
    }

    FPDDictionarySetAtNumber(hParentDic, "Count", count);
}

void Arelle_Operation::DeleteParentBookmarkCount(FPD_Object hParentDic)
{
    //Get the dictionary information of the number of children of the input object
    //The plus or minus of the number of children indicates whether the bookmark is in the expanded state
    int count = FPDDictionaryGetInteger(hParentDic, "Count");
    if(count <= 0)
    {
        count += 1;
    }
    else
    {
        count -= 1;
    }

    FPDDictionarySetAtNumber(hParentDic, "Count", count);
}

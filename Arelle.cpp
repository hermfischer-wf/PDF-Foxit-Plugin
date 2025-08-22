/*********************************************************************

 Copyright (C) 2025 Foxit Corporation
 All rights reserved.

 NOTICE: Foxit permits you to use, modify, and distribute this file
 in accordance with the terms of the Foxit license agreement
 accompanying it. If you have received this file from a source other
 than Foxit, then your use, modification, or distribution of it
 requires the prior written permission of Foxit.

 ---------------------------------------------------------------------

Bookmark.cpp

 - Defines the entry point for the DLL application, the entry point is PlugInMain.

*********************************************************************/


#include "stdafx.h"
#include "Arelle.h"
#include "arelletoolhander.h"
#include "NavPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CBookmarkApp

BEGIN_MESSAGE_MAP(CArelleApp, CWinApp)
	//{{AFX_MSG_MAP(CArelleApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBookmarkApp construction

CArelleApp::CArelleApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CArelleApp object

CArelleApp theApp;

/** 
	This sample gives an explanation about how to create the bookmark 
	of the PDF document, and display the bookmark on the panel view.
*/

/********************************************************************/
/* The basic procedure of plug-in.                                   
/********************************************************************/

/* PIExportHFTs
** ------------------------------------------------------
**/
/** 
** Create and Add the extension HFT's.
**
** @return true to continue loading plug-in,
** false to cause plug-in loading to stop.
*/
FS_BOOL PIExportHFTs(void)
{
	return TRUE;
}

/** 
The application calls this function to allow it to
<ul>
<li> Import plug-in supplied HFTs.
<li> Replace functions in the HFTs you're using (where allowed).
<li> Register to receive notification events.
</ul>
*/
FS_BOOL PIImportReplaceAndRegister(void)
{
	return TRUE;
}

////////////////////////////////////////////////////////////////////
/* Plug-ins can use their Initialization procedures to hook into Foxit Reader's 
	 * user interface by adding menu items, toolbar buttons, windows, and so on.
	 * It is also acceptable to modify Foxit Reader's user interface later when the plug-in is running.
	 */
void PILoadMenuBarUI(void* pParentWnd)
{

}

void PIReleaseMenuBarUI(void* pParentWnd)
{

}
//////////////////////////////////////////////////////////
void PILoadToolBarUI(void* pParentWnd)
{

}

void PIReleaseToolBarUI(void* pParentWnd)
{

}

void PILoadRibbonUI(void* pParentWnd)
{
	FR_PanelViewCallbacksRec FRPanelViewCallbacks;
	memset(&FRPanelViewCallbacks, 0, sizeof(FR_PanelViewCallbacksRec));
	FRPanelViewCallbacks.lStructSize = sizeof(FR_PanelViewCallbacksRec);
	FRPanelViewCallbacks.FRPanelViewGetName = CArelleToolHandler::Tool_GetName;
	FRPanelViewCallbacks.FRPanelViewGetTitle = CArelleToolHandler::Tool_GetTitle;
	FRPanelViewCallbacks.FRPanelViewInitNewView = CArelleToolHandler::Tool_InitNewView;
	FRPanelViewCallbacks.FRPanelViewOnGetHwnd = CArelleToolHandler::Tool_PanelViewOnGetHwnd;
	FRPanelViewCallbacks.FRPanelViewOnPanelActive2 = CArelleToolHandler::Tool_PanelViewOnPanelActive;
	FRPanelViewCallbacks.FRPanelViewOnActive = CArelleToolHandler::Tool_SetActiveView;
	FRPanelViewCallbacks.FRPanelViewOnDestroyView = CArelleToolHandler::Tool_CloseView;
	FRPanelViewCallbacks.FRPanelViewGetButtonTip = CArelleToolHandler::Tool_GetButtonTip;
	FRPanelViewCallbacks.FRPanelViewGetButtonIconUnion = CArelleToolHandler::Tool_PanelViewGetButtonIconUnion;

	FRAppAddNavPanelView(&FRPanelViewCallbacks, 0);
}

void PILoadStatusBarUI(void* pParentWnd)
{

}
/* PIInit
** ------------------------------------------------------
**/
/** 
	The main initialization routine.
	
	@return true to continue loading the plug-in, 
	false to cause plug-in loading to stop.
*/
FS_BOOL PIInit(void)
{
	return TRUE;
}

/* PIUnload
** ------------------------------------------------------
**/
/** 
	The unload routine.
	Called when your plug-in is being unloaded when the application quits.
	Use this routine to release any system resources you may have
	allocated.

	Returning false will cause an alert to display that unloading failed.
	@return true to indicate the plug-in unloaded.
*/
FS_BOOL PIUnload(void)
{	
	return TRUE;
}

/** PIHandshake
	function provides the initial interface between your plug-in and the application.
	This function provides the callback functions to the application that allow it to 
	register the plug-in with the application environment.

	Required Plug-in handshaking routine:
	
	@param handshakeVersion the version this plug-in works with. 
	@param handshakeData OUT the data structure used to provide the primary entry points for the plug-in. These
	entry points are used in registering the plug-in with the application and allowing the plug-in to register for 
	other plug-in services and offer its own.
	@return true to indicate success, false otherwise (the plug-in will not load).
*/
FS_BOOL PIHandshake(FS_INT32 handshakeVersion, void *handshakeData)
{
	if(handshakeVersion != HANDSHAKE_V0100)
		return FALSE;
	
	/* Cast handshakeData to the appropriate type */
	PIHandshakeData_V0100* pData = (PIHandshakeData_V0100*)handshakeData;
	
	/* Set the name we want to go by */
	pData->PIHDRegisterPlugin(pData, "FoxitSamplePluginBookmark", (FS_LPCWSTR)L"Bookmark");

	/* If you export your own HFT, do so in here */
	pData->PIHDSetExportHFTsCallback(pData, &PIExportHFTs);
		
	/*
	** If you import plug-in HFTs, replace functionality, and/or want to register for notifications before
	** the user has a chance to do anything, do so in here.
	*/
	pData->PIHDSetImportReplaceAndRegisterCallback(pData, &PIImportReplaceAndRegister);

	/* Perform your plug-in's initialization in here */
	pData->PIHDSetInitDataCallback(pData, &PIInit);

	PIInitUIProcs initUIProcs;
	INIT_CALLBACK_STRUCT(&initUIProcs, sizeof(PIInitUIProcs));
	initUIProcs.lStructSize = sizeof(PIInitUIProcs);
	initUIProcs.PILoadMenuBarUI = PILoadMenuBarUI;
	initUIProcs.PIReleaseMenuBarUI = PIReleaseMenuBarUI;
	initUIProcs.PILoadToolBarUI = PILoadToolBarUI;
	initUIProcs.PIReleaseToolBarUI = PIReleaseToolBarUI;
	initUIProcs.PILoadRibbonUI = PILoadRibbonUI;
	initUIProcs.PILoadStatusBarUI = PILoadStatusBarUI;
	pData->PIHDSetInitUICallbacks(pData, &initUIProcs);

	/* Perform any memory freeing or state saving on "quit" in here */
	pData->PIHDSetUnloadCallback(pData, &PIUnload);

	return TRUE;
}

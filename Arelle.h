// Arelle.h : main header file for the ARELLE DLL
//

#if !defined(AFX_ARELLE_H__8BD1A6DA_7546_4A66_9558_AEC6D76EEC14__INCLUDED_)
#define AFX_ARELLE_H__8BD1A6DA_7546_4A66_9558_AEC6D76EEC14__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CArelleApp
// See Arelle.cpp for the implementation of this class
//

class CArelleApp : public CWinApp
{
public:
	CArelleApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArelleApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CBookmarkApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARELLE_H__8BD1A6DA_7546_4A66_9558_AEC6D76EEC14__INCLUDED_)

// EDAHelper.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "EDAHelper.h"
#include "EDAHelperDlg.h"
#include "Hook.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEDAHelperApp

BEGIN_MESSAGE_MAP(CEDAHelperApp, CWinApp)
	//{{AFX_MSG_MAP(CEDAHelperApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEDAHelperApp construction

CEDAHelperApp::CEDAHelperApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CEDAHelperApp object

CEDAHelperApp theApp;

BOOL AlreadyRunning()
{
   BOOL bFound = FALSE;

   // Try to create a mutex with the app's name
   HANDLE hMutexOneInstance = CreateMutex(NULL,TRUE,"EDAHelper");

   // Already there...means that we are already running an instance
   if(GetLastError() == ERROR_ALREADY_EXISTS)
   {
      bFound = TRUE;
      MessageBox(NULL, _T("进程已存在一个实例，退出执行"), _T("温馨提示"), MB_OK | MB_ICONINFORMATION);
   }
   // Release the mutex
   if(hMutexOneInstance)
      ReleaseMutex(hMutexOneInstance);

   return(bFound);
}

/////////////////////////////////////////////////////////////////////////////
// CEDAHelperApp initialization

BOOL CEDAHelperApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	if(AlreadyRunning())
	{
		return FALSE;
	}


	CEDAHelperDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CEDAHelperApp::ExitInstance() 
{
	return CWinApp::ExitInstance();
}

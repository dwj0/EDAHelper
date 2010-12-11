#include "stdafx.h"
#include <afxinet.h>
#include "Update.h"
#include "resource.h"


DWORD WINAPI UpdateTask( LPVOID lpParameter )
{
	BOOL	*bRunning = (BOOL *)lpParameter;
	*bRunning = 1;
	UpdateCheck(1);
	*bRunning = 0;
	ExitThread(0);
	return 0;
}

void UpdateCheck(bool IsQuiet)
{
    CInternetSession session("HttpClient");
    char * server_url = "http://www.edahelper.com/edahelper/update.ini";
	char	title[512];
	char	ver[32];
	char	url[512];
	char	urltype[32];
	char	desc[8192];
	CString	msg;

    CHttpFile* pfile = (CHttpFile *)session.OpenURL(server_url);

    DWORD dwStatusCode;
    pfile->QueryInfoStatusCode(dwStatusCode);
    if(dwStatusCode == HTTP_STATUS_OK)
    {
        while(1)
		{
			if (!pfile->ReadString(title, 512))
			{
				goto found_nonew;
			}
			if(!strncmp(title, "[LastVersion]", strlen("[LastVersion]")))
			{
				break;
			}
		}
		if(!pfile->ReadString(ver, 32) || !strncmp(ver+4, CURRENT_VERSION, strlen(CURRENT_VERSION)))
		{
			goto found_nonew;
		}
		if(pfile->ReadString(url, 512) && pfile->ReadString(urltype, 32) &&pfile->ReadString(desc, 8192))
		{
			TRACE("%s\n%s\n%s\n", ver, url, desc);
		}
		else
		{
			goto found_nonew;
		}
    }
	else
	{
		goto found_nonew;
	}
	msg.Format("%s%s%s", NEW_VERSION_TIP, ver+4, "点击确定进入下载页面！");
	if(MessageBox(NULL, msg, "EDAHelper", MB_OKCANCEL|MB_ICONINFORMATION|MB_SETFOREGROUND) == IDOK)
	{
		ShellExecute(NULL,_T("open"), url+4, _T(""), NULL, 0);
	}
	goto label_end;

found_nonew:
	if(!IsQuiet)
	{
		MessageBox(NULL, NONEW_VERSION_TIP, "EDAHelper", MB_OK|MB_ICONINFORMATION);
	}
label_end:
    pfile -> Close();
    delete pfile;
    session.Close();

 //   return 0;
}

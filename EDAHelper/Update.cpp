#include "stdafx.h"
#include <afxinet.h>
#include "Update.h"
#include "resource.h"
#include "DialogUpdate.h"

#define BYTES_PER_READ	1024
static char Buffer[BYTES_PER_READ];
static int	nBufferCnt;
static char*pstrChar;

int GetMachineID(TCHAR* machineid);


static void ResetBuffer()
{
	nBufferCnt = 0;
}

static BOOL ReadString(CHttpFile *pfile, char* pstrTarget, int	nMax)
{
	if(nMax > BYTES_PER_READ)
	{
		return FALSE;
	}
	if (nBufferCnt < nMax)
	{
		if(!(nBufferCnt += pfile->Read(Buffer+nBufferCnt, nMax - nBufferCnt)))
		{
			return FALSE;
		}
	}
	pstrChar = Buffer;
	while (nBufferCnt)
	{
		if ((*pstrChar != '\r') && (*pstrChar != '\n'))
			*pstrTarget++ = *pstrChar;
		
		nBufferCnt--;
		if (*pstrChar++ == '\n' || nBufferCnt == 0)
		{
			memmove(Buffer, pstrChar, nBufferCnt);
			*pstrTarget = '\0';
			return TRUE;
		}
	}
	return FALSE;
}

DWORD WINAPI UpdateTask( LPVOID lpParameter )
{
	BOOL	*bRunning = (BOOL *)lpParameter;
	*bRunning = 1;
	UpdateCheck(1);
	*bRunning = 0;
//	ExitThread(0);
	AfxEndThread(0);
	return 0;
}

BOOL IsNewer(char *szVerCur, char *szVerSrv)
{
	UINT	nVerCur, nVerSrv;

	nVerCur = ((szVerCur[0]-0x30)<<16) + ((szVerCur[2]-0x30) << 8) + ((szVerCur[4]-0x30)<<4)+(szVerCur[5]-0x30);
	nVerSrv = ((szVerSrv[0]-0x30)<<16) + ((szVerSrv[2]-0x30) << 8) + ((szVerSrv[4]-0x30)<<4)+(szVerSrv[5]-0x30);
	if (nVerSrv > nVerCur)
	{
		return TRUE;
	}
	return FALSE;
}

CString CharToString(char * str)
{
	CString szRet = _T("");
#ifdef	_UNICODE
	TCHAR	*pstr;
	int	nLenOfWideBytes = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	if (nLenOfWideBytes)
	{
		pstr = (TCHAR *)malloc(nLenOfWideBytes* sizeof(TCHAR));
		if(!pstr) return szRet;
		MultiByteToWideChar(CP_ACP, 0, str, -1, pstr, nLenOfWideBytes);
		szRet += pstr;
		free(pstr);
	}
#else
	szRet += str;
#endif
	return szRet;
}

void StringToChar(CString *str, char *szTarget)
{
	CString szRet = _T("");
#ifdef	_UNICODE
	int	nLenOfWideBytes = WideCharToMultiByte(CP_ACP, NULL, *str, -1, szTarget, str->GetLength()+1, NULL, NULL);
#else
	strcpy(szTarget, *str);
#endif
}

CString GetUpdateInfo(CHttpFile *pfile, char *szVerCur)
{
	char	ver[33];
	char	url[513];
	char	urltype[33];
	char	desc[513];
	CString	szUpdateInfo = _T("更新内容：\r\n\r\n");

	ReadString(pfile, ver, 32);
	while(1)
	{
		if (ver[0] != '[' || ver[7] != ']') return szUpdateInfo;
		if(IsNewer(szVerCur, ver+1))
		{
			szUpdateInfo += CharToString(ver);
			szUpdateInfo += _T("\r\n");
			if(ReadString(pfile, url, 512) && ReadString(pfile, urltype, 32))
			{
				while(1)
				{
					if(ReadString(pfile, desc, 512))
					{
						if(strncmp(desc, "desc", 4))
						{
							strncpy(ver, desc, 32);
							break;
						}
						szUpdateInfo += CharToString(desc+7);
						szUpdateInfo += _T("\r\n");
					}
					else
					{
						return szUpdateInfo;
					}
				}
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	return szUpdateInfo;
}

void UpdateCheck(bool IsQuiet)
{
	TCHAR machineid[12];
	CString	szClientName = "EDAHelper";//AfxGetAppName();
	szClientName += _T("-");
	szClientName += _T(CURRENT_VERSION);

    CInternetSession session(szClientName);
#ifdef	_DEBUG
    TCHAR  server_url[256] = _T("http://www.edahelper.com/edahelper/update_debug.ini?");
#else
	TCHAR  server_url[256] = _T("http://www.edahelper.com/edahelper/update.ini?");
#endif
	char	title[512];
	char	ver[32];
	char	ver_ref[32];
	char	url[512];
	char	urltype[32];
	char	desc[1024];
	CString	msg;

    CHttpFile* pfile;// = (CHttpFile *)session.OpenURL(server_url);

	if(GetMachineID(machineid))
	{
	}
	
	wcscat(server_url, machineid);

	try
	{
		pfile = (CHttpFile *)session.OpenURL(server_url);
	}
	catch(CInternetException *e)
	{
		e = e;	// remove warning
		if (!IsQuiet)
		{
			AfxMessageBox(_T("您可能不能上网，无法检测更新"));
		}
		return;
	}
    DWORD dwStatusCode;
	if (pfile == NULL)
	{
		return;
	}
	ResetBuffer();
    pfile->QueryInfoStatusCode(dwStatusCode);
    if(dwStatusCode == HTTP_STATUS_OK)
    {
        while(1)
		{
			if (!ReadString(pfile, title, 512))
			{
				goto found_nonew;
			}
			if(!strncmp((char*)title, "[LastVersion]", strlen("[LastVersion]")))
			{
				break;
			}
		}
		if(IsQuiet)
		{
			CWinApp *pApp = AfxGetApp();
			msg = pApp->GetProfileString(CONFIG_ENTRY, _T("NoCheckver"), _T(CURRENT_VERSION));
			StringToChar(&msg, ver_ref);
		}
		else
		{
			strcpy(ver_ref, CURRENT_VERSION);
		}
		if(!ReadString(pfile, ver, 32) || !IsNewer(ver_ref, ver+4))
		{
			goto found_nonew;
		}
		if(ReadString(pfile, url, 512) && ReadString(pfile, urltype, 32) && ReadString(pfile, desc, 1024))
		{
		//	TRACE(_T("%s\n%s\n%s\n"), ver, url, desc);
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
	
	msg.Format(_T("EDAHelper%s%s"), NEW_VERSION_TIP, CharToString(ver+4));
	{
		CDialogUpdate dlg;
		dlg.szUpdateInfo = GetUpdateInfo(pfile, CURRENT_VERSION);
		dlg.szCaption = msg;
		if (IsQuiet)
		{
			dlg.nDialogPos = SCREEN_RIGHT_DOWN;
		}
		else
		{
			dlg.nDialogPos = SCREEN_CENTER;
		}
		switch (dlg.DoModal())
		{
		case	 IDOK:
			ShellExecute(NULL, _T("open"), CharToString(url+4), _T(""), NULL, 0);
			break;
		case	 IDIGNORE:
			// Set server version to registry
			{
				CWinApp *pApp = AfxGetApp();
				pApp->WriteProfileString(CONFIG_ENTRY, _T("NoCheckver"), CharToString(ver+4));

			}
			break;
		case	 IDCANCEL:
		default:
			break;
		}

	}
	goto label_end;

found_nonew:
	if(!IsQuiet)
	{
		MessageBox(NULL, NONEW_VERSION_TIP, _T("EDAHelper"), MB_OK|MB_ICONINFORMATION);
	}
label_end:
    pfile -> Close();
    delete pfile;
    session.Close();
}
#include "stdafx.h"  
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>  
#include <windows.h>  
#include <WinIoCtl.h>  
//#include "macid.h"  
#include <intrin.h>  
//#include "crc.h"  
//#include "xxtea.h"  
#include <nb30.h>  
  
#pragma comment (lib,"netapi32.lib")  
  
typedef struct _ASTAT_  
{  
    ADAPTER_STATUS adapt;  
    NAME_BUFFER    NameBuff [30];  
}ASTAT, * PASTAT;  
   
ASTAT Adapter;   
  
#define OID_802_3_PERMANENT_ADDRESS        0x01010101  
//#define OID_802_3_CURRENT_ADDRESS        0x01010102  
#define IOCTL_NDIS_QUERY_GLOBAL_STATS    0x00170002  
  
  
int FindStr(TCHAR *source,TCHAR *substr){  
    size_t lensource=_tcslen(source);  
    size_t lensub=_tcslen(substr);  
    for(int i=0;i<lensource-lensub;i++){  
        bool finded=true;  
        for(int j=0;j<lensub;j++){  
            if(source[i+j]!=substr[j]){  
                finded=false;  
                break;  
            }  
        }  
        if(finded) return i;  
    }  
    return -1;  
}   
  
int GetMacFromTcpip(unsigned char *szMac)  
{  
    int nError = -1;  
    if ( szMac==NULL){  
        return nError;  
    }  
  
    HKEY hKey = NULL;  
    //HKEY hKey2 = NULL;  
    TCHAR szBuffer[MAX_PATH];  
    TCHAR szFileName[MAX_PATH] = {0};  
    DWORD dwRet = 0;  
    DWORD dwType = 0;  
    DWORD cbData = 0;  
    DWORD cbData2 = 0;  
    DWORD cName = _countof(szBuffer);  
    if ( RegOpenKey(HKEY_LOCAL_MACHINE,_T("SYSTEM\\ControlSet001\\services\\Tcpip\\Parameters\\Adapters"),&hKey)!=ERROR_SUCCESS ){  
        return nError;  
    }  
  
    for ( int i=0; RegEnumKeyEx(hKey,i,szBuffer,&cName,NULL,NULL,NULL,NULL)==ERROR_SUCCESS; ++i, cName = _countof(szBuffer) ){  
            _tcscpy_s(szFileName,MAX_PATH,_T("\\\\.\\"));             
            _tcscat_s(szFileName,MAX_PATH,szBuffer);  
            //MessageBox(0,szFileName,L"Path",48);  
            HANDLE hFile = CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);  
            if ( hFile!=INVALID_HANDLE_VALUE ){  
                DWORD dwInBuff = OID_802_3_PERMANENT_ADDRESS;  
                BYTE outBuff[MAX_PATH];  
                dwRet = DeviceIoControl(hFile,IOCTL_NDIS_QUERY_GLOBAL_STATS,&dwInBuff,sizeof(dwInBuff),outBuff,sizeof(outBuff),&cbData,NULL);  
  
                //无论成功失败关闭文件句柄  
                CloseHandle(hFile);  
                hFile = INVALID_HANDLE_VALUE;  
  
                if ( dwRet ){  
                    memcpy(szMac,outBuff,6);  
                    nError = 0;  
                    break;  
                }  
            }else{  
                nError=GetLastError();  
                //打开设备失败  
            }       
  
    }//end for  
  
    if ( hKey!=NULL ){  
        RegCloseKey(hKey);  
    }  
    return nError;  
}  
  
  
int GetMac(unsigned char *szMac,bool diswifi)  
{  
    int nError = -1;  
    if ( szMac==NULL){  
        return nError;  
    }  
  
    HKEY hKey = NULL;  
    HKEY hKey2 = NULL;  
    TCHAR szKey[MAX_PATH],szBuffer[MAX_PATH];  
    TCHAR szServiceName[MAX_PATH];  
    TCHAR szDescription[MAX_PATH];  
    TCHAR szFileName[MAX_PATH] = {0};  
    DWORD dwRet = 0;  
    DWORD dwType = 0;  
    DWORD cbData = 0;  
    DWORD cbData2 = 0;  
    DWORD cName = _countof(szBuffer);  
    if ( RegOpenKey(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NetworkCards\\"),&hKey)!=ERROR_SUCCESS ){  
        return nError;  
    }  
  
    for ( int i=0; RegEnumKeyEx(hKey,i,szBuffer,&cName,NULL,NULL,NULL,NULL)==ERROR_SUCCESS; ++i, cName = _countof(szBuffer) ){  
        _tcscpy_s(szKey,MAX_PATH,_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NetworkCards\\"));  
        _tcscat_s(szKey,MAX_PATH,szBuffer);  
        if ( RegOpenKey(HKEY_LOCAL_MACHINE,szKey,&hKey2)!=ERROR_SUCCESS ){  
            continue;  
        }  
  
        dwType = REG_SZ;  
        cbData = MAX_PATH*sizeof(TCHAR);  
        if ( RegQueryValueEx(hKey2,_T("ServiceName"),NULL,&dwType,(LPBYTE)szServiceName,&cbData)==ERROR_SUCCESS ){  
            cbData2 = MAX_PATH*sizeof(TCHAR);  
            RegQueryValueEx(hKey2,_T("Description"),NULL,&dwType,(LPBYTE)szDescription,&cbData2);  
            //读取成功后关闭句柄  
            RegCloseKey(hKey2);  
            if(FindStr(szDescription,_T("Wifi"))>=0 && diswifi){  
                continue;  
            }  
  
            if(FindStr(szDescription,_T("Wireless"))>=0 && diswifi){  
                continue;  
            }  
  
            if(FindStr(szDescription,_T("USB"))>=0 && diswifi){  
                continue;  
            }  
  
            if(FindStr(szDescription,_T("VPN"))>=0 && diswifi){  
                continue;  
            }             
  
            _tcscpy_s(szFileName,MAX_PATH,_T("\\\\.\\"));             
            _tcscat_s(szFileName,MAX_PATH,szServiceName);  
            //MessageBox(0,szFileName,L"Path",48);  
            HANDLE hFile = CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);  
            if ( hFile!=INVALID_HANDLE_VALUE ){  
                DWORD dwInBuff = OID_802_3_PERMANENT_ADDRESS;  
                BYTE outBuff[MAX_PATH];  
                dwRet = DeviceIoControl(hFile,IOCTL_NDIS_QUERY_GLOBAL_STATS,&dwInBuff,sizeof(dwInBuff),outBuff,sizeof(outBuff),&cbData,NULL);  
  
                //无论成功失败关闭文件句柄  
                CloseHandle(hFile);  
                hFile = INVALID_HANDLE_VALUE;  
  
                if ( dwRet ){  
                    memcpy(szMac,outBuff,6);  
                    nError = 0;  
                    break;  
                }  
            }else{  
                nError=GetLastError();  
                //打开设备失败  
            }  
        }else{  
            //读取失败关闭句柄  
            RegCloseKey(hKey2);  
        }          
  
    }//end for  
  
    if ( hKey!=NULL ){  
        RegCloseKey(hKey);  
    }  
    return nError;  
} 

void HexToStr(BYTE *pbDest, BYTE *pbSrc, int nLen)
{
	char ddl,ddh;
	int i;

	for (i=0; i<nLen; i++)
	{
		ddh = 48 + pbSrc[i] / 16;
		ddl = 48 + pbSrc[i] % 16;
		if (ddh > 57) ddh = ddh + 7;
		if (ddl > 57) ddl = ddl + 7;
		pbDest[i*2] = ddh;
		pbDest[i*2+1] = ddl;
	}

	pbDest[nLen*2] = '\0';
}
int GetMachineID(TCHAR* machineid){  
  
    BYTE NetCardID[16];
	char  sztmp[13];
	int	i;
    int res=0;  
    res=GetMac(NetCardID,true);  
    if(res!=0){  
        res=GetMac(NetCardID,false);  
    }  
    if(res!=0){  
        res=GetMacFromTcpip(NetCardID);  
    }  
    if(res!=0)return res;  
	
	HexToStr((BYTE*)sztmp, NetCardID, 6);

	swprintf(machineid, 12, L"%hs", sztmp);
	return 0;  
}  
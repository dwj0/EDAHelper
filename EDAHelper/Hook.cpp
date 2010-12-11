// Hook.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Hook.h"
#include "winuser.h"
#include <tlhelp32.h>
#include <Winuser.h>

static HHOOK hkb = NULL;

#define	PIXEL_PER_STEP		32

static	POINT	CurPosPre={0,0};
static	BOOL	rbtnDown = FALSE;
static	BOOL	rbtnMove = FALSE;
static	BOOL	mbtnDown = FALSE;
static	BOOL	mbtnMove = FALSE;

typedef DWORD (CALLBACK * GetModuleBaseName_t)(
					HANDLE hProcess,	// handle to the process
					HMODULE hModule,	// handle to module to find filename for
					LPTSTR lpFilename,	// pointer to buffer to receive module path
					DWORD nSize 		// size of buffer, in characters
					);
typedef BOOL (CALLBACK *EnumProcessModules_t)(
					HANDLE hProcess,	  // handle to the process
					HMODULE * lphModule,  // array to receive the module handles
					DWORD cb,			  // size of the array
					LPDWORD lpcbNeeded	  // receives the number of bytes returned
					);//定义回调函数的地址 

GetModuleBaseName_t pGetModuleBaseName;
EnumProcessModules_t pEnumProcessModules;


WindowType_t CheckProcess(void)
{
	static DWORD		pid;			//全部static是为了减少访问堆栈的时间
	static HANDLE		hProcess;
	static char			szProcessName[256];
	static char			szClassName[16];
	static HMODULE		hMod;
	static DWORD		cbNeeded;
	static WindowType_t	RetCode;

//	HWND hWnd = GetForegroundWindow();
	POINT	pt;
	HWND hWnd;
	GetCursorPos(&pt);
	hWnd = WindowFromPoint(pt);

	RetCode = WIN_EDA_NONE;
	GetWindowThreadProcessId(hWnd, &pid);
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (NULL != hProcess )
	{
		if ( pEnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
		{
			if(pGetModuleBaseName( hProcess, hMod, szProcessName, 
							   sizeof(szProcessName)/sizeof(TCHAR)))
			{
				_strlwr(szProcessName);
				GetClassName(hWnd, szClassName, 16);
				if(strstr(szProcessName, "client99se") || strstr(szProcessName, "quickpcb"))
				{
					if	(!strncmp(szClassName, "Sch Window", 3))
					{
						RetCode = WIN_PROTEL_SCH;
					}
					else if(!strncmp(szClassName, "PcbView", 3))
					{
						RetCode = WIN_PROTEL_PCB;
					}
					else if(!strncmp(szClassName, "TScrollBox", 10))
					{
						RetCode = WIN_PROTEL_PCB;
					}
//					else if(!strncmp(szClassName, "msctls_statusba", 10))
					{
//						TRACE("szClassName = %s\n", szClassName);
//						RetCode = WIN_QUICKPCB;
					}
				}
				else if(strstr(szProcessName, "aim"))
				{
					if	(!strncmp(szClassName, "TkChild", 6))
					{
						RetCode = WIN_SABER_SKETCH;
					}
				}
				else if(strstr(szProcessName, "dxp"))
				{
					if (!strncmp(szClassName, "Sch", 3))
					{
						RetCode = WIN_DXP_SCH;
					}
					else if(!strncmp(szClassName, "View_Graphical", 3))
					{
						RetCode = WIN_DXP_PCB;
					}
					else if(!strncmp(szClassName, "TPreviewPaintBox", 10))
					{
						RetCode = WIN_DXP_PCB;
					}
				}
				else if((strstr(szProcessName, "powerlogic") || strstr(szProcessName, "powerl~1.exe")) && !strncmp(szClassName, "AfxFrameOrView", 14))
				{
					RetCode = WIN_PADS_LOGIC;
				}
				else if((strstr(szProcessName, "powerpcb")) && !strncmp(szClassName, "AfxFrameOrView", 14))
				{
					RetCode = WIN_PADS_PCB;
				}
				else if((strstr(szProcessName, "blazerouter")) && !strncmp(szClassName, "AfxFrameOrView", 14))
				{
					RetCode = WIN_PADS_ROUTER;
				}
				else if((strstr(szProcessName, "capture")) && !strncmp(szClassName, "OrRandomView", 12))
				{
					RetCode = WIN_ORCAD_CAPTURE;
				}
				else if(strstr(szProcessName, "camnt"))
				{
					RetCode = WIN_CAM350;
				}
				else if(strstr(szProcessName, "allegro"))
				{
					RetCode = WIN_CADENCE_ALLEGRO;
				}
			}
		}
	}
	CloseHandle(hProcess);
	return RetCode;
}

LRESULT CALLBACK LowLevelMouseProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	WindowType_t	eWinType;

	if(nCode == HC_ACTION)
	{
		PMSLLHOOKSTRUCT pMSLLHook = (PMSLLHOOKSTRUCT)lParam;
		if(wParam == WM_MOUSEMOVE)
		{
			eWinType = CheckProcess();
			if(!(eWinType 
				& (WIN_PADS_PCB 
				| WIN_PADS_LOGIC
				| WIN_PADS_ROUTER
				| WIN_PROTEL_SCH
				| WIN_SABER_SKETCH
				| WIN_CAM350
				| WIN_QUICKPCB
				| WIN_ORCAD_CAPTURE)))
			{
				return CallNextHookEx(hkb, nCode, wParam, lParam );
			}
			if(rbtnDown)
			{
				POINT	pt;
				HWND	hWnd;
				long	lparam;
				int		step = PIXEL_PER_STEP;
				int		distance;

				GetCursorPos(&pt);
				hWnd = WindowFromPoint(pt);
#if 0
				HDC		hDc;
				RECT	Rect;
				hDc = GetDC(hWnd);
				GetWindowRect(hWnd, &Rect);
//				LineTo(hDc, 100, 1000);
				BitBlt(
					hDc,
					-10,
					-10,
					Rect.right - Rect.left,
					Rect.bottom - Rect.top,
					hDc,
					0,
					0,
					SRCCOPY);
#endif

				if (eWinType & WIN_ORCAD_CAPTURE)
				{
					HWND   hStatusBar; 
					
					DWORD	dwProcessID;
					void *	Pointer;
					HANDLE	hProcess;
					char	status_buf[64];
					ULONG	NumberOfBytesRead;
					int		scale;

					hStatusBar = GetDlgItem(GetForegroundWindow(), AFX_IDW_STATUS_BAR);
					::GetWindowThreadProcessId(hStatusBar, &dwProcessID);
					hProcess = OpenProcess(PROCESS_VM_OPERATION|PROCESS_VM_READ|PROCESS_VM_WRITE, false, dwProcessID);
					Pointer = VirtualAllocEx(hProcess, NULL, 64, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE); 
					::SendMessage(hStatusBar, SB_GETTEXT, 2, (LPARAM)Pointer);
					if(::ReadProcessMemory(hProcess, Pointer, status_buf, sizeof(status_buf), &NumberOfBytesRead))
					{
						status_buf[9] = 0;
						scale = atoi(status_buf + 6);
						if (scale == 0)
						{
							return CallNextHookEx(hkb, nCode, wParam, lParam );
						}
						step = (int)(step * (scale/400.0));
					}
					VirtualFreeEx(hProcess, Pointer, 64, MEM_RELEASE);
				}
				distance = pMSLLHook->pt.x - CurPosPre.x;
				while( abs(distance) > step)
				{
					lparam = (long )GetDlgItem(GetParent(hWnd), 0xea00);	// 仅用于Orcad
					if(distance > 0)
					{
						PostMessage(hWnd, WM_HSCROLL, SB_LINELEFT, lparam);
					}
					else
					{
						PostMessage(hWnd, WM_HSCROLL, SB_LINERIGHT, lparam);
					}
#if 1
					if(eWinType & (WIN_PADS_PCB | WIN_PADS_LOGIC | WIN_PADS_ROUTER))
					{
						PostMessage(hWnd, WM_KEYDOWN, VK_END, 0);
						PostMessage(hWnd, WM_KEYUP, VK_END, 0);
					}	
#else
					PostMessage(hWnd, WM_KEYDOWN, VK_SHIFT, 0);
					PostMessage(hWnd, 
								WM_MOUSEWHEEL,
								((pMSLLHook->pt.x - CurPosPre.x)<<16|MK_SHIFT),
								((pMSLLHook->pt.y << 16) + pMSLLHook->pt.x));
					PostMessage(hWnd, WM_KEYUP, VK_SHIFT, 0);
#endif
					CurPosPre.x = pMSLLHook->pt.x;
					rbtnMove = TRUE;
					distance = distance > 0 ? distance - step: distance + step;
				}
				distance = pMSLLHook->pt.y - CurPosPre.y;
				while(abs(distance) > step)
				{
#if 1
					lparam = (long )GetDlgItem(GetParent(hWnd), 0xea10);
					if(distance > 0)
						PostMessage(hWnd, WM_VSCROLL, SB_LINEUP, lparam);
					else
						PostMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, lparam);
					
#else
					if(eWinType & (WIN_PADS_PCB | WIN_PADS_LOGIC | WIN_PADS_ROUTER))
					{
						PostMessage(hWnd, WM_KEYDOWN, VK_END, 0);
						PostMessage(hWnd, WM_KEYUP, VK_END, 0);
					}	
					PostMessage(hWnd, WM_KEYDOWN, VK_MENU, 0);
					PostMessage(hWnd, 
								WM_MOUSEWHEEL,
								((pMSLLHook->pt.y - CurPosPre.y)<<16),
								((pMSLLHook->pt.y << 16) + pMSLLHook->pt.x));
					PostMessage(hWnd, WM_KEYUP, VK_MENU, 0);
#endif
					CurPosPre.y = pMSLLHook->pt.y;
					rbtnMove = TRUE;
					{
						int		x, y;
						RECT	rect;
						SCROLLINFO	si;
						GetScrollRange(hWnd, SB_VERT, &x, &y);
						TRACE("x = %d, y = %d\n", x, y);
						y = GetScrollPos(hWnd, SB_VERT);
						TRACE("y_Pos = %d\n", y);

						ZeroMemory(&si, sizeof(si));
						si.cbSize = sizeof(si);
						si.fMask = SIF_TRACKPOS | SIF_PAGE | SIF_POS | SIF_RANGE;

						if(GetScrollInfo(hWnd, SB_VERT, (SCROLLINFO*)&si))
						{
							TRACE("GetScrollInfo ok\n");
						}
						else
						{
							TRACE("Error = 0x%x\n", GetLastError());
						}

						GetWindowRect(hWnd, &rect);
						TRACE(" height = %d, width = %d\n", rect.top-rect.bottom, rect.left-rect.right);
					}
					distance = distance > 0 ? distance - step: distance + step;
				}

				return CallNextHookEx(hkb, nCode, wParam, lParam );
			}
			else if(mbtnDown)
			{
				mbtnMove = TRUE;
			}
				
			return CallNextHookEx(hkb, nCode, wParam, lParam );
		}
		{
			HWND hWnd = GetForegroundWindow();
			eWinType = CheckProcess();
			if(wParam == WM_MOUSEWHEEL)
			{
				if(!eWinType)
				{
					return CallNextHookEx(hkb, nCode, wParam, lParam );
				}
				if(eWinType & WIN_ORCAD_CAPTURE)
				{
					if((short)HIWORD(pMSLLHook->mouseData) > 0)
					{
						PostMessage(hWnd, WM_KEYDOWN, 'I', 0);
						PostMessage(hWnd, WM_KEYUP, 'I', 0);
						return TRUE;
					}
					else
					{
						PostMessage(hWnd, WM_KEYDOWN, 'O', 0);
						PostMessage(hWnd, WM_KEYUP, 'O', 0);
						return TRUE;
					}
				}
				else if(eWinType & WIN_CAM350)
				{
					if((short)HIWORD(pMSLLHook->mouseData) > 0)
					{
						PostMessage(hWnd, WM_KEYDOWN, VK_ADD, 0);
						PostMessage(hWnd, WM_KEYUP, VK_ADD, 0);
					}
					else
					{
						PostMessage(hWnd, WM_KEYDOWN, VK_SUBTRACT, 0);
						PostMessage(hWnd, WM_KEYUP, VK_SUBTRACT, 0);
					}
					return TRUE;
				}
				else if(eWinType & WIN_CADENCE_ALLEGRO)
				{}
				else if(eWinType & WIN_QUICKPCB)
				{
					POINT	pt;

					GetCursorPos(&pt);
					hWnd = WindowFromPoint(pt);
					if((short)HIWORD(pMSLLHook->mouseData) > 0)
					{
						PostMessage(hWnd, WM_KEYDOWN, 0x21, 0);
					}
					else
					{
						PostMessage(hWnd, WM_KEYDOWN, 0x22, 0);
					}
					return TRUE;
				}
				else
				{
					if((GetAsyncKeyState(VK_CONTROL) || GetAsyncKeyState(VK_SHIFT) || GetAsyncKeyState(VK_MENU)) && 0x8000)
					{
						return CallNextHookEx(hkb, nCode, wParam, lParam );
					}
					TRACE("pMSLLHook->mouseData = %x\n", pMSLLHook->mouseData);
					if((short)HIWORD(pMSLLHook->mouseData) > 0)
					{
						PostMessage(hWnd, WM_KEYDOWN, 0x21, 0);
					}
					else
					{
						PostMessage(hWnd, WM_KEYDOWN, 0x22, 0);
					}
					return TRUE;
				}
			}
#if 0
			else if(wParam == WM_LBUTTONDOWN)
			{
				if(eWinType & (WIN_PROTEL_PCB | WIN_PROTEL_SCH | WIN_DXP_PCB | WIN_DXP_SCH))
				{
					PostMessage(hWnd, WM_SYSDEADCHAR, VK_BACK, 0);
					return TRUE;
				}
			}
#endif
			else if(wParam == WM_MBUTTONDOWN)
			{
				if(eWinType & (WIN_PADS_LOGIC|WIN_PADS_PCB|WIN_PADS_ROUTER))
				{
					mbtnMove = FALSE;
					mbtnDown = TRUE;
				}
			}
			else if(wParam == WM_MBUTTONUP)
			{
				if(eWinType & (WIN_PADS_LOGIC | WIN_PADS_PCB| WIN_PADS_ROUTER))
				{
					mbtnDown = FALSE;
					if(mbtnMove)
					{
						return CallNextHookEx(hkb, nCode, wParam, lParam );
					}
					PostMessage(hWnd, WM_KEYDOWN, VK_F4, 0);
					PostMessage(hWnd, WM_KEYUP, VK_F4, 0);
				}
				if(!(eWinType & (WIN_PROTEL_PCB | WIN_PROTEL_SCH | WIN_DXP_PCB | WIN_DXP_SCH)))
				{
					return CallNextHookEx(hkb, nCode, wParam, lParam );
				}
				PostMessage(hWnd, WM_KEYDOWN, 106, 0);
				PostMessage(hWnd, WM_KEYUP, 106, 0);
				PostMessage(hWnd, WM_KEYDOWN, VK_END, 0);
			}
			else if(wParam == WM_RBUTTONDOWN)
			{
				if((eWinType & (WIN_PROTEL_PCB | WIN_PROTEL_SCH | WIN_DXP_PCB | WIN_DXP_SCH)) 
					&& (GetAsyncKeyState(VK_LBUTTON) & 0x8000))
				{
					PostMessage(hWnd, WM_KEYDOWN, ' ', 0);
					return TRUE;
				}
				if(eWinType 
					& (WIN_PADS_LOGIC
					| WIN_PADS_PCB
					| WIN_PADS_ROUTER
					| WIN_PROTEL_SCH
					| WIN_ORCAD_CAPTURE
					| WIN_QUICKPCB
					|WIN_CAM350))
				{
					rbtnDown = TRUE;
					CurPosPre = pMSLLHook->pt;
					return TRUE;
				}
			}
			else if(wParam == WM_RBUTTONUP)
			{
				if((eWinType & (WIN_PROTEL_PCB | WIN_PROTEL_SCH | WIN_DXP_PCB | WIN_DXP_SCH)) 
					&& (GetAsyncKeyState(VK_LBUTTON) & 0x8000))
				{
					return TRUE;
				}

				rbtnDown = FALSE;
				if(eWinType & (WIN_PADS_LOGIC | WIN_PADS_PCB
					| WIN_PADS_ROUTER | WIN_PROTEL_SCH
					| WIN_ORCAD_CAPTURE |WIN_CAM350))
				{
					if(rbtnMove)
					{
						rbtnMove = FALSE;
						return TRUE;
//						return CallNextHookEx(hkb, nCode, wParam, lParam );
					}

					POINT	p;
					GetCursorPos(&p);
					hWnd = WindowFromPoint(p);

					ScreenToClient(hWnd, &p);
					lParam = MAKELPARAM(p.x, p.y);
					PostMessage(hWnd, WM_RBUTTONDOWN, MK_LBUTTON, lParam);
					return CallNextHookEx(hkb, nCode, wParam, lParam );
				}
					
			}
		}
	}

	return CallNextHookEx(hkb, nCode, wParam, lParam );	
}

LRESULT CALLBACK CallWndProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		if (wParam == WM_ACTIVATEAPP)
		{
			TRACE("WM_ACTIVATEAPP\n");
		}
	}
	return CallNextHookEx(hkb, nCode, wParam, lParam );

}

BOOL installhook()// 用WM_ACTIVATEAPP消息判断当前窗口进程可能效率更高，但必需用DLL
{
	hkb = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)LowLevelMouseProc, AfxGetInstanceHandle(), 0);
//	hkb = SetWindowsHookEx(WH_CALLWNDPROC, (HOOKPROC)CallWndProc, GetModuleHandle (NULL), 0);
	return TRUE;
}



BOOL UnHook()
{   	
	BOOL unhooked = UnhookWindowsHookEx(hkb);
	return unhooked;
}

BOOL HookInit() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HINSTANCE hPsDll = LoadLibrary("PSAPI.DLL");
	pGetModuleBaseName =
			(GetModuleBaseName_t)GetProcAddress(hPsDll, "GetModuleBaseNameA");
	pEnumProcessModules =
			(EnumProcessModules_t)GetProcAddress(hPsDll, "EnumProcessModules");
	return TRUE;
}

#include "stdafx.h"
#include "Hook.h"
#include "EDAHelper.h"
#include "pads.h"
#include "winuser.h"
#include <tlhelp32.h>
#include <Windows.h>
#include <Winuser.h>

typedef BOOL (CALLBACK *PrintWindow_t)(
									   HWND hwnd,               // Window to copy
									   HDC  hdcBlt,             // HDC to print into
									   UINT nFlags              // Optional flags
									   );//定义回调函数的地址 
PrintWindow_t		pPrintWindow;

static const TCHAR FuncAppName[]=_T("Pads");

static UINT gEnableConfig;
static bool mbtnSendMsgFlag = FALSE;
static HDC	hDc, hDcCompatible;
static HBITMAP hbmCompatible;
static bool rbtnDragFlag = FALSE;
static int dist_x, dist_y;		// 右键移动的时候使用，表现移动的距离
static CRect	Rect;

UINT	PadsGetConf(void)
{
	return gEnableConfig;
}

void	PadsSetConf(UINT value)
{
	CWinApp *pApp = AfxGetApp();
	gEnableConfig = value;
	pApp->WriteProfileInt(CONFIG_ENTRY, FuncAppName, gEnableConfig);
}

static void SwitchLayer(HWND hWnd)
{
	PostMessage(hWnd, WM_KEYDOWN, VK_F4, 0);
	PostMessage(hWnd, WM_KEYUP, VK_F4, 0);
}

static void RefreshScreen(HWND hWnd)
{
	PostMessage(hWnd, WM_KEYDOWN, VK_END, 0);
	PostMessage(hWnd, WM_KEYUP, VK_END, 0);
}


LRESULT PadsProc(int nWinType, int nCode, WPARAM wParam, LPARAM lParam)
{
	if(!(gEnableConfig & PADS_GLOBAL_ENABLE))
	{
		return CallNextHookEx(hkb, nCode, wParam, lParam );
	}
	PMSLLHOOKSTRUCT pMSLLHook = (PMSLLHOOKSTRUCT)lParam;

	if((gEnableConfig & PADS_RIGBTN_DRAG) && (wParam == WM_MOUSEMOVE))
	{
		POINT	pt;
		HWND	hWnd;
		GetCursorPos(&pt);
		hWnd = WindowFromPoint(pt);
		TRACE2("px = %d, py = %d\n",pMSLLHook->pt.x, pMSLLHook->pt.y);
		TRACE2("ptx = %d, pty = %d\n",pt.x, pt.y);
		if(rbtnDown)
		{
			int		step = PIXEL_PER_STEP;

			if(gEnableConfig & PADS_RIGBTN_DRAG_SMOOTH)
			{
				if (FALSE == rbtnDragFlag)
				{
					dist_x = 0;
					dist_y = 0;
					rbtnDragFlag = TRUE;

					hDc = GetDC(hWnd);
					GetClientRect(hWnd, &Rect);
					hDcCompatible = CreateCompatibleDC(hDc);
					if(!hDcCompatible)
					{
						TRACE1("hDcCompatible Error Code = %d\n", GetLastError());
					}
					hbmCompatible = CreateCompatibleBitmap(hDc, 
						Rect.right - Rect.left,
						Rect.bottom - Rect.top);
					if(!hbmCompatible)
					{
						TRACE1("hbmCompatible Error Code = %d\n", GetLastError());
					}
					SelectObject(hDcCompatible, hbmCompatible);
					/*pPrintWindow(hWnd, hDcCompatible, 1);
					*/
					BitBlt(
						hDcCompatible,
						0,
						0,
						Rect.Width(),
						Rect.Height(),
						hDc,
						0,
						0,
						SRCCOPY);
				}
				dist_x += pMSLLHook->pt.x - CurPosPre.x;
				dist_y += pMSLLHook->pt.y - CurPosPre.y;
//	 			dist_x = pMSLLHook->pt.x - CurPosPre.x;
//	 			dist_y = pMSLLHook->pt.y - CurPosPre.y;
				TRACE2("x = %d, y = %d\n",dist_x, dist_y);
				TRACE2("px = %d, y = %d\n",pMSLLHook->pt.x, pMSLLHook->pt.y);
				TRACE2("x = %d, y = %d\n",CurPosPre.x, CurPosPre.y);
				BitBlt(
					hDc,
					dist_x,
					dist_y,
					Rect.Width(),
					Rect.Height(),
					hDcCompatible,
					0,
					0,
 					SRCCOPY);

				CBrush	hbr;
				CRect	rcTmp = Rect;
				hbr.CreateSolidBrush(0);
				if(dist_x > 0)
				{
					rcTmp.right = rcTmp.left + dist_x; 
				}
				else
				{
					rcTmp.left = rcTmp.right + dist_x;
				}
				FillRect(hDc, &rcTmp, (HBRUSH)hbr);
				rcTmp = Rect;
				if(dist_y > 0)
				{
					rcTmp.bottom = rcTmp.top + dist_y; 
				}
				else
				{
					rcTmp.top = rcTmp.bottom + dist_y;
				}
				FillRect(hDc, &rcTmp, (HBRUSH)hbr);

				rbtnMove = TRUE;
				return TRUE;
//				return CallNextHookEx(hkb, nCode, wParam, lParam );
			}

			else
			{
				int		distance;
				distance = pMSLLHook->pt.x - CurPosPre.x;
				while( abs(distance) > step)
				{
					if(distance > 0)
					{
						PostMessage(hWnd, WM_HSCROLL, SB_LINELEFT, NULL);
					}
					else
					{
						PostMessage(hWnd, WM_HSCROLL, SB_LINERIGHT, NULL);
					}
					
					CurPosPre.x = pMSLLHook->pt.x;
					rbtnMove = TRUE;
					distance = distance > 0 ? distance - step: distance + step;
				}
				distance = pMSLLHook->pt.y - CurPosPre.y;
				while(abs(distance) > step)
				{
					if(distance > 0)
						PostMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);
					else
						PostMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
					
					CurPosPre.y = pMSLLHook->pt.y;
					rbtnMove = TRUE;
					
					distance = distance > 0 ? distance - step: distance + step;
				}
				
				return CallNextHookEx(hkb, nCode, wParam, lParam );
			}
		}
		else if(mbtnDown)
		{
			mbtnMove = TRUE;
			if (!mbtnSendMsgFlag)
			{
				ScreenToClient(hWnd, &pt);
				keybd_event(VK_MBUTTON, 1, 0, 0);
				PostMessage(hWnd, WM_MBUTTONDOWN, MK_MBUTTON, MAKELONG(pt.x,pt.y));
				mbtnSendMsgFlag = TRUE;
				return TRUE;
			}
		}
			
		return CallNextHookEx(hkb, nCode, wParam, lParam );
	}
	{
		POINT	pt;
		GetCursorPos(&pt);
		HWND hWnd = WindowFromPoint(pt);
		if((gEnableConfig & PADS_MIDBTN_ZOOM) && (wParam == WM_MOUSEWHEEL))
		{
			if((GetAsyncKeyState(VK_CONTROL) || GetAsyncKeyState(VK_SHIFT) || GetAsyncKeyState(VK_MENU)) && 0x8000)
			{
				return CallNextHookEx(hkb, nCode, wParam, lParam );
			}
			TRACE1("pMSLLHook->mouseData = %x\n", pMSLLHook->mouseData);
			if((short)HIWORD(pMSLLHook->mouseData) > 0)
			{

#ifndef _DEBUG
				PostMessage(hWnd, WM_KEYDOWN, 0x21, 0);
#else
				PostMessage(hWnd, WM_KEYDOWN, VK_CONTROL, 0);
				PostMessage(hWnd, 
					WM_MOUSEWHEEL,
					(pMSLLHook->mouseData | MK_CONTROL),
					((pMSLLHook->pt.y << 16) + pMSLLHook->pt.x));
				PostMessage(hWnd, WM_KEYUP, VK_CONTROL, 0);
#endif
			}
			else
			{
#ifndef _DEBUG
				PostMessage(hWnd, WM_KEYDOWN, 0x22, 0);
#else
				PostMessage(hWnd, WM_KEYDOWN, VK_CONTROL, 0);
				PostMessage(hWnd, 
					WM_MOUSEWHEEL,
					(pMSLLHook->mouseData | MK_CONTROL),
					((pMSLLHook->pt.y << 16) + pMSLLHook->pt.x));
				PostMessage(hWnd, WM_KEYUP, VK_CONTROL, 0);
#endif
			}
			return TRUE;
		}
		else if((gEnableConfig & PADS_MIDBTN_SWITCH) && (wParam == WM_MBUTTONDOWN))
		{
			if(nWinType & (WIN_PADS_LOGIC|WIN_PADS_PCB|WIN_PADS_ROUTER))
			{
				mbtnMove = FALSE;
				mbtnDown = TRUE;
				mbtnSendMsgFlag = 0;
				return TRUE;
			}
		}
		else if((gEnableConfig & PADS_MIDBTN_SWITCH) && (wParam == WM_MBUTTONUP))
		{
			if(nWinType & (WIN_PADS_LOGIC | WIN_PADS_PCB| WIN_PADS_ROUTER))
			{
				mbtnDown = FALSE;
				if(!mbtnMove)
				{
					SwitchLayer(hWnd);
					return TRUE;
				}
			}
			return CallNextHookEx(hkb, nCode, wParam, lParam );
		}
		else if((gEnableConfig & PADS_RIGBTN_DRAG) && (wParam == WM_RBUTTONDOWN))
		{
			if((gEnableConfig & PADS_RIGBTN_ROTATE) && (GetAsyncKeyState(VK_LBUTTON) & 0x8000))
			{
				PostMessage(hWnd, WM_KEYDOWN, VK_TAB, 0);
				PostMessage(hWnd, WM_KEYUP, VK_TAB, 0);
				return TRUE;
			}
			rbtnDown = TRUE;
			CurPosPre = pMSLLHook->pt;
			rbtnDragFlag = FALSE;
			return TRUE;
		}
		else if((gEnableConfig & PADS_RIGBTN_DRAG) && (wParam == WM_RBUTTONUP))
		{
			rbtnDown = FALSE;
			if((gEnableConfig & PADS_RIGBTN_ROTATE) && (GetAsyncKeyState(VK_LBUTTON) & 0x8000))
			{
				return TRUE;
			}
			if(rbtnMove)
			{
				CRect	Rect;
				POINT	pttmp;
				rbtnMove = FALSE;
				if(gEnableConfig & PADS_RIGBTN_DRAG_SMOOTH)
				{
					GetClientRect(hWnd, &Rect);
					
					keybd_event(VK_MBUTTON, 1, 0, 0);
					lParam = MAKELPARAM(Rect.CenterPoint().x - dist_x, Rect.CenterPoint().y - dist_y);
					PostMessage(hWnd, WM_MBUTTONDOWN, MK_MBUTTON, lParam);
					PostMessage(hWnd, WM_MBUTTONUP, MK_MBUTTON, lParam);
					keybd_event(VK_MBUTTON, 1, KEYEVENTF_KEYUP, 0);
					
					pttmp.x = CurPosPre.x + dist_x;
					pttmp.y = CurPosPre.y + dist_y;
					TRACE2("pttmp.x = %d, pttmp.y = %d\n", pttmp.x, pttmp.y);
					if(!SetCursorPos(pttmp.x, pttmp.y))
					{
						TRACE0("Error SetCursorPos\n");
					}
					
					if (!DeleteObject(hbmCompatible))
					{
						TRACE1("deleteobject error %d\n", GetLastError());
					}
					if (!DeleteDC(hDcCompatible))
					{
						TRACE1("DeleteDC error %d\n", GetLastError());
					}
					if (!ReleaseDC(hWnd, hDc))
					{
						TRACE1("ReleaseDC error %d\n", GetLastError());
					}
				}

				return TRUE;
			}
			POINT	p;
			GetCursorPos(&p);
			hWnd = WindowFromPoint(p);
  
			ScreenToClient(hWnd, &p);
			lParam = MAKELPARAM(p.x, p.y);
			PostMessage(hWnd, WM_RBUTTONDOWN, MK_RBUTTON, lParam);
			return CallNextHookEx(hkb, nCode, wParam, lParam );
		}
	}
	return CallNextHookEx(hkb, nCode, wParam, lParam );
}

static HookList_t	PadsHook = {NULL,
	(WIN_PADS_PCB | WIN_PADS_LOGIC | WIN_PADS_ROUTER),
	PadsProc};

BOOL PadsInit(void)
{
	CWinApp *pApp = AfxGetApp();
	gEnableConfig = pApp->GetProfileInt(CONFIG_ENTRY, FuncAppName, 0xFFFFFFFF);

	HINSTANCE hPsDll = LoadLibrary(_T("USER32.DLL"));
	pPrintWindow =
		(PrintWindow_t)GetProcAddress(hPsDll, "PrintWindow");

	return HookRegister(&PadsHook);
}


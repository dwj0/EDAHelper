#include "stdafx.h"
#include "Hook.h"
#include "pads.h"
#include "winuser.h"
#include <tlhelp32.h>
#include <Windows.h>

static const TCHAR FuncAppName[]=_T("Pads");

static UINT gEnableConfig;

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
		if(rbtnDown)
		{
			POINT	pt;
			HWND	hWnd;
			int		step = PIXEL_PER_STEP;
			int		distance;

			GetCursorPos(&pt);
			hWnd = WindowFromPoint(pt);
#if 0
			SCROLLINFO	si;
			int	x, y;

			ZeroMemory(&si, sizeof(si));
			si.cbSize = sizeof(si);
			si.fMask = SIF_TRACKPOS | SIF_POS | SIF_PAGE |  SIF_RANGE;
			
			GetScrollInfo(hWnd, SB_VERT, (SCROLLINFO*)&si);
			BYTE cpMapTbl[256];
			ScreenToClient(hWnd, &pt);
			::GetKeyboardState(cpMapTbl);
			BYTE cMask = 1 << (CHAR_BIT - 1);
			cpMapTbl[VK_LBUTTON] |= cMask;
			::SetKeyboardState(cpMapTbl);
//			keybd_event(VK_LBUTTON, 0, 0, 0);
			PostMessage(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(pt.x,pt.y));
			Sleep(10);
//			PostMessage(hWnd, WM_VSCROLL, MAKELONG(SB_THUMBTRACK, 100), 0);
//			PostMessage(hWnd, WM_LBUTTONUP, 0, 0);
			PostMessage(hWnd, WM_MOUSEMOVE, MK_LBUTTON, MAKELONG(pt.x, 200));
			::GetKeyboardState(cpMapTbl);
			cMask = (1 << (CHAR_BIT - 1));
			cpMapTbl[VK_LBUTTON] &= ~cMask;
			::SetKeyboardState(cpMapTbl);

//			keybd_event(VK_LBUTTON, 0,  KEYEVENTF_KEYUP, 0);
			return CallNextHookEx(hkb, nCode, wParam, lParam );


#endif
#if 0
			SCROLLINFO	si;
			int	x, y;
			GetScrollRange(hWnd, SB_VERT, &x, &y);
			TRACE2("x = %d, y = %d\n", x, y);
			y = GetScrollPos(hWnd, SB_VERT);
			TRACE2("y_Pos = %d\n", y);
			
			ZeroMemory(&si, sizeof(si));
			si.cbSize = sizeof(si);
			si.fMask = SIF_TRACKPOS | SIF_POS | SIF_PAGE |  SIF_RANGE;
			
//			SetScrollPos(hWnd, SB_VERT, y+1, TRUE);
			if(GetScrollInfo(hWnd, SB_VERT, (SCROLLINFO*)&si))
			{
 				TRACE2("GetScrollInfo nPos = %d\n", si.nPos);
			}
			else
			{
				TRACE2("Error = 0x%x\n", GetLastError());
			}
			si.nPos =670;
//			ScrollWindow(hWnd, 0, 1, NULL, NULL);
// 			TRACE2("GetScrollInfo nMax = %d\n", si.nMax);
// 			TRACE2("GetScrollInfo nMin = %d\n", si.nMin);
// 			TRACE2("GetScrollInfo nPos = %d\n", si.nPos);
//			PostMessage(hWnd, WM_VSCROLL, SB_THUMBPOSITION, NULL);
			si.nTrackPos = 671;
			si.fMask = SIF_TRACKPOS | SIF_PAGE | SIF_POS | SIF_RANGE;
			SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

//			::PostMessage(hWnd,WM_SYSCOMMAND,SC_VSCROLL, 10); 

//			int ret1 = SendMessage(hWnd,WM_VSCROLL,MAKELONG(SB_THUMBTRACK, 0),0); 
			int ret1 = PostMessage(hWnd,WM_VSCROLL, 0x0005, 0); 
			ret1 = PostMessage(hWnd,WM_CAPTURECHANGED, 0,0); 
			ret1 = PostMessage(hWnd,WM_VSCROLL, 0x004,0); 
			PostMessage(hWnd, WM_KEYDOWN, VK_END, NULL);
			PostMessage(hWnd, WM_KEYUP, VK_END, NULL);
//			ret1 = SendMessage(hWnd,WM_VSCROLL,MAKELONG(SB_THUMBPOSITION, si.nPos),0); 
//			ret1 = SendMessage(hWnd,WM_VSCROLL,MAKELONG(SB_ENDSCROLL, 100),0); 
//			TRACE2("SendMessage Ret = %d\n", ret1);
			if(GetScrollInfo(hWnd, SB_VERT, (SCROLLINFO*)&si))
			{
				TRACE2("GetScrollInfo nPage = %d\n", si.nPage);
				TRACE2("GetScrollInfo nTrackPos = %d\n", si.nTrackPos);
				TRACE2("GetScrollInfo nPos = %d\n", si.nPos);
			}
			else
			{
				TRACE2("Error = 0x%x\n", GetLastError());
			}

			si.nPos -=10;
// 			TRACE2("GetScrollInfo nMax = %d\n", si.nMax);
// 			TRACE2("GetScrollInfo nMin = %d\n", si.nMin);
// 			TRACE2("GetScrollInfo nPos = %d\n", si.nPos);
//			SetScrollPos(hWnd, SB_VERT, si.nPos, TRUE);
			HDC		hDc;
			RECT	Rect;
//				HBRUSH	hbr;
			hDc = GetDC(hWnd);
			GetWindowRect(hWnd, &Rect);
//			GetClientRect(hWnd, &Rect);

//				CDC dcMemory;
//				TRACE2("cdc = %d\n", sizeof(dcMemory));
//				dcMemory.CreateCompatibleDC(CDC::FromHandle(hDc));
//				dcMemory.Get

//				BitBlt(hDc, 0, 0, Rect.right, Rect.bottom, dcMemory.m_hDC, 0, 0, SRCCOPY);
//				BitBlt(dcMemory.m_hDC, -10, -10, Rect.right - Rect.left, Rect.bottom - Rect.top, hDc, 0, 0, SRCCOPY);
			
			//FillRect(hDc, &Rect, hbr);
//				LineTo(hDc, 100, 1000);
// 			BitBlt(
// 				hDc,
// 				-0,
// 				-1,
// 				Rect.right - Rect.left,
// 				Rect.bottom - Rect.top,
// 				hDc,
// 				0,
// 				0,
// 				SRCCOPY);
 			CBrush	hbr;
//			PostMessage(hWnd, WM_PAINT, NULL, NULL);
//			ScrollWindow(hWnd, 0, -10, NULL, NULL);
//			UpdateWindow (hWnd);
			
//			ScrollWindowEx(hWnd, 0, 1, )
//			((CScrollView*) CWnd::FromHandle(hWnd))->ScrollToPosition(pta);
//			InvalidateRect(hWnd, &Rect, TRUE);
			hbr.CreateSolidBrush(100);
//			LRESULT ret = SendMessage(hWnd, SBM_GETPOS, NULL, NULL);
//			TRACE2("ret = %x\n", ret);
			
//			PostMessage(hWnd, WM_VSCROLL, SB_ENDSCROLL, NULL);
//			PostMessage(hWnd, WM_KEYDOWN, VK_END, NULL);
//			PostMessage(hWnd, WM_KEYUP, VK_END, NULL);
			rbtnMove = TRUE;
			return CallNextHookEx(hkb, nCode, wParam, lParam );

//			FillRect(hDc, &Rect, (HBRUSH)hbr);
#endif

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
//				RefreshScreen(hWnd);
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

//				RefreshScreen(hWnd);

				CurPosPre.y = pMSLLHook->pt.y;
				rbtnMove = TRUE;
				
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
				rbtnMove = FALSE;
				return TRUE;
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
	return CallNextHookEx(hkb, nCode, wParam, lParam );
}

static HookList_t	PadsHook = {NULL,
	(WIN_PADS_PCB | WIN_PADS_LOGIC | WIN_PADS_ROUTER),
	PadsProc};

BOOL PadsInit(void)
{
	CWinApp *pApp = AfxGetApp();
	gEnableConfig = pApp->GetProfileInt(CONFIG_ENTRY, FuncAppName, 0xFFFFFFFF);
	return HookRegister(&PadsHook);
}

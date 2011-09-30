#include "stdafx.h"
#include "Hook.h"
#include "cam350.h"
#include "winuser.h"
#include <tlhelp32.h>
#include <Windows.h>

static const TCHAR FuncAppName[]=_T("Cam350");

static UINT gEnableConfig;

UINT	Cam350GetConf(void)
{
	return gEnableConfig;
}

void	Cam350SetConf(UINT value)
{
	CWinApp *pApp = AfxGetApp();
	gEnableConfig = value;
	pApp->WriteProfileInt(CONFIG_ENTRY, FuncAppName, gEnableConfig);
}

LRESULT Cam350Proc(int nWinType, int nCode, WPARAM wParam, LPARAM lParam)
{
	if(!(gEnableConfig & CAM350_GLOBAL_ENABLE))
	{
		return CallNextHookEx(hkb, nCode, wParam, lParam );
	}
	PMSLLHOOKSTRUCT pMSLLHook = (PMSLLHOOKSTRUCT)lParam;
	if((gEnableConfig & CAM350_RIGBTN_DRAG) && (wParam == WM_MOUSEMOVE))
	{
		if(rbtnDown)
		{
			POINT	pt;
			HWND	hWnd;
			int		step = PIXEL_PER_STEP;
			int		distance;

			GetCursorPos(&pt);
			hWnd = WindowFromPoint(pt);

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

				if(nWinType & (WIN_PADS_PCB | WIN_PADS_LOGIC | WIN_PADS_ROUTER))
				{
					PostMessage(hWnd, WM_KEYDOWN, VK_END, 0);
					PostMessage(hWnd, WM_KEYUP, VK_END, 0);
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
		else if(mbtnDown)
		{
			mbtnMove = TRUE;
		}
			
		return CallNextHookEx(hkb, nCode, wParam, lParam );
	}
	{
		static DWORD lParamSave;
		POINT	pt;
		GetCursorPos(&pt);
		HWND hWnd = WindowFromPoint(pt);
		if((gEnableConfig & CAM350_MIDBTN_ZOOM) && (wParam == WM_MOUSEWHEEL))
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
// 		else if((gEnableConfig & CAM350_MIDBTN_FITALL) && (wParam == WM_MBUTTONDOWN))
// 		{
// 			return TRUE;
// 		}
// 		else if((gEnableConfig & CAM350_MIDBTN_FITALL) && (wParam == WM_MBUTTONUP))
// 		{
// 			PostMessage(hWnd, WM_KEYDOWN, VK_HOME, 0);
// 			PostMessage(hWnd, WM_KEYUP, VK_HOME, 0);
// 			return TRUE;
// 		}
		else if((gEnableConfig & (CAM350_MIDBTN_FITALL | CAM350_MIDBTN_MOVE)) && (wParam == WM_MBUTTONDOWN))
		{
			mbtnMove = FALSE;
			mbtnDown = TRUE;
			GetCursorPos(&pt_pre);
			lParamSave = wParam;
			if (gEnableConfig & CAM350_MIDBTN_MOVE)		// 因为orcad有中键移动视图的功能，如果开启中键移动各操作，则要屏蔽原有功能
			{
				return	TRUE;
			}
			else
			{
				return CallNextHookEx(hkb, nCode, wParam, lParam );
			}
		}
		else if((gEnableConfig & (CAM350_MIDBTN_FITALL | CAM350_MIDBTN_MOVE)) && (wParam == WM_MBUTTONUP))
		{
			mbtnDown = FALSE;
			GetCursorPos(&pt_cur);
			if((gEnableConfig & CAM350_MIDBTN_MOVE) && (distance_xy() > MBTN_ACTION_GRID))	// 中键移动处理
			{
				if(distance_x() < distance_y())
				{
					if (pt_pre.y > pt_cur.y )		// 中键上移，相当于按键backspace，可删除画线时的上一操作
					{
						PostMessage(hWnd, WM_KEYDOWN, 'A', 0);
						PostMessage(hWnd, WM_KEYUP, 'A', 0);
					}
					else
					{
						keybd_event(VK_MENU, 0,  0, 0);	// ALT+E+D 启动删除操作
						keybd_event(VK_MENU, 0,  KEYEVENTF_KEYUP, 0);
						keybd_event('E', 0,  0, 0);
						keybd_event('E', 0,  KEYEVENTF_KEYUP, 0);
						keybd_event('D', 0,  0, 0);
						keybd_event('D', 0,  KEYEVENTF_KEYUP, 0);
					}
				}
				else
				{
					if (pt_pre.x > pt_cur.x )		// 中键左移，撤消操作
					{
						keybd_event(VK_CONTROL, 0,  0, 0);	// CTRL+Z
						keybd_event('Z', 0,  0, 0);
						keybd_event('Z', 0,  KEYEVENTF_KEYUP, 0);
						keybd_event(VK_CONTROL, 0,  KEYEVENTF_KEYUP, 0);
					}
					else							// 中键左移，重做操作
					{
						keybd_event(VK_CONTROL, 0,  0, 0);	// CTRL+DELETE
						keybd_event('U', 0,  0, 0);
						keybd_event('U', 0,  KEYEVENTF_KEYUP, 0);
						keybd_event(VK_CONTROL, 0,  KEYEVENTF_KEYUP, 0);
					}
				}
				return TRUE;
			}
			else if(mbtnMove != TRUE)
			{
				if(gEnableConfig & CAM350_MIDBTN_FITALL)
				{
					PostMessage(hWnd, WM_KEYDOWN, VK_HOME, 0);
					PostMessage(hWnd, WM_KEYUP, VK_HOME, 0);
				}
				else
				{
					POINT	p;
					GetCursorPos(&p);

					hWnd = WindowFromPoint(p);
					ScreenToClient(hWnd, &p);
					PostMessage(hWnd, WM_MBUTTONDOWN, MK_MBUTTON, MAKELPARAM(p.x, p.y));
					return CallNextHookEx(hkb, nCode, wParam, lParam );
				}
				return CallNextHookEx(hkb, nCode, wParam, lParam );
			}
			else if (gEnableConfig & CAM350_MIDBTN_MOVE)
			{
				return TRUE;
			}

			return CallNextHookEx(hkb, nCode, wParam, lParam );
		}
		else if((gEnableConfig & CAM350_RIGBTN_DRAG) && (wParam == WM_RBUTTONDOWN))
		{
			rbtnDown = TRUE;
			CurPosPre = pMSLLHook->pt;
			return TRUE;
		}
		else if((gEnableConfig & CAM350_RIGBTN_DRAG) && (wParam == WM_RBUTTONUP))
		{
			rbtnDown = FALSE;
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
		return CallNextHookEx(hkb, nCode, wParam, lParam );
	}
}

static HookList_t	Cam350Hook = {NULL,
	(WIN_CAM350),
	Cam350Proc};

BOOL Cam350Init(void)
{
	CWinApp *pApp = AfxGetApp();
	gEnableConfig = pApp->GetProfileInt(CONFIG_ENTRY, FuncAppName,0xFFFFFFFF);
	return HookRegister(&Cam350Hook);
}


// Hook.h : main header file for the HOOK DLL
//

#if !defined(AFX_HOOK_H__AF915EB3_9303_4281_90B1_26C298EAC81B__INCLUDED_)
#define AFX_HOOK_H__AF915EB3_9303_4281_90B1_26C298EAC81B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

typedef enum	WindowType_e
{
	WIN_EDA_NONE		= 0,
	WIN_PROTEL_SCH		= 1,
	WIN_PROTEL_PCB		= 1<<1,
	WIN_DXP_SCH			= 1<<2,
	WIN_DXP_PCB			= 1<<3,
	WIN_PADS_LOGIC		= 1<<4,
	WIN_PADS_PCB		= 1<<5,
	WIN_PADS_ROUTER		= 1<<6,
	WIN_ORCAD_CAPTURE	= 1<<7,
	WIN_CAM350			= 1<<8,
	WIN_CADENCE_ALLEGRO	= 1<<9,
	WIN_QUICKPCB		= 1<<10,
	WIN_SABER_SKETCH	= 1<<11,
	WIN_TYPE_MAX
}WindowType_t;

BOOL HookInit();
BOOL installhook();
BOOL UnHook();

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOOK_H__AF915EB3_9303_4281_90B1_26C298EAC81B__INCLUDED_)

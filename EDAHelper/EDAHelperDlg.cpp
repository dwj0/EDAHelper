// EDAHelperDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EDAHelper.h"
#include "EDAHelperDlg.h"
#include "Hook.h"
#include "Update.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  MYWM_NOTIFYICON  WM_USER+1


/////////////////////////////////////////////////////////////////////////////
// CEDAHelperDlg dialog

static char	*AppDesc = 
//"2.1.00说明：\r\n"
//"    1、增加自动检测更新\r\n"
//"    2、修改orcad的右键移动步进，使更灵敏\r\n"
//"    3、修改说明中一直以来的一个错别字，呵呵"
//"    4、增加新版本自动检测功能"
//"    4、增加saber sketch中键缩放功能"

"2.0.10临时版说明：\r\n"
"    1、修复在PADS中按住CONTROL，SHIFT，ALT键时，中键原功能失效的BUG\r\n"
"    2、修改在PADS中非画图窗口，鼠标功能失效的BUG\r\n"
"    3、增加繁体版（用WORD2003转换，不对之处请指正\r\n"
"    4、上传自googlecode，感兴趣的朋友可以通过svn checkout https://dbghelper.googlecode.com/svn/EDAHelper获得代码\r\n"
"2.0.09临时版说明：\r\n"
"    1、修复通过双击打开powerlogic文件时，外挂功能失效问题\r\n"
"    2、修改中键缩放时屏幕偏移的问题\r\n"
"2.0.08临时版说明：\r\n"
"    1、增加Protel99se、DXP打印预览中键缩放的支持\r\n"
"2.0.07临时版说明：\r\n"
"    1、增加OrCAD右键拖动的支持\r\n"
"2.0.06临时版说明：\r\n"
"    1、修正CAM350右键原功能失效的问题\r\n"
"2.0.05临时版说明：\r\n"
"    1、2.0.04版本导致速度变慢，由于大部分情况下不会出现刷新不正常，所以放弃该版本带来的更新\r\n"
"    2、解决PROTEL99SE原理图里增加右键移动后，按住左键再按右键时器件不能旋转的问题(直到今天"
"才有人反馈该问题，说明此功能很少人用到，其实这是很好用的功能，大家不妨试试，PCB里也可用)\r\n"
"2.0.04临时版说明：\r\n"
"    1、解决部分显卡不右键拖动时刷新不正常的问题\r\n"
"2.0.03临时版说明：\r\n"
"    1、增加CAM350的支持，包括滚轮放大缩小和右键拖动\r\n"
"2.0.02临时版说明：\r\n"
"    1、修复在BlazeRouter里右键拖动后还会出现菜单的BUG\r\n"
"2.0.01临时版说明：\r\n"
"    1、提高鼠标移动运行效率\r\n"
"    2、解决PROTEL侧边栏滚轮BUG\r\n"
"    3、增加PROTEL99SE原理图右键移动功能\r\n"
"    4、支持powerlogic\r\n"
"    5、支持BlazeRouter\r\n"
"第二版(2.0)说明：\r\n"
"    本软件是部分EDA软件的鼠标增强工具，将EDAHelper.exe和Hook.dll同时放到任意目录，运行EDAHelper.exe"
"就行，现在已不再自动运行EDA软件，支持protel99se，DXP，POWERPCB，OrCAD的capture。\r\n"
"\r\n针对protel99se,DXP(功能和从前一样)：\r\n"
"    1. 向上滚动滚轮 --> 放大，相当于PageUp(不能改掉软件原来的快捷键，否则就不灵了)\r\n"
"    2. 向下滚动滚轮 --> 放大，相当于PageDown(不能改掉软件原来的快捷键，否则就不灵了)\r\n"
"    3. 按下滚轮     --> 切换有效图层并刷新，相当于小键盘'*'的功能\r\n"
"    4. 按左键拖目标 --> 再按右键可旋转，相当于空格键的功能，在布局时非常好用，请留意\r\n"
"\r\n针对powerpcb(支持PADS2005SP2、PADS2007，别的版本没有测过，应该也支持)：\r\n"
"    1. 按住鼠标右键移动，可移动工作区，相当于SHIFT+滚轮或者ALT+滚轮，不平滑\r\n"
"    2. 向上滚动滚轮 --> 放大，相当于PageUp\r\n"
"    3. 向下滚动滚轮 --> 缩小，相当于PageDown\r\n" 
"    4. 鼠标中键点击 --> 切换图层，相当于F4，原中键点击功能废弃\r\n" 
"\r\n针对OrCAD：\r\n"
"    1. 向上滚动滚轮 --> 放大，相当于字母\"I\"键\r\n"
"    2. 向下滚动滚轮 --> 缩小相当于字母\"O\"键\r\n" 
"\r\n    目前存在问题：点击系统图标关闭或者最小化窗口时，大约会停止响应3 秒钟，可能是Hook没有反安"
"装，我发现同类软件也有这样的问题，但是不知道怎么解决，哪位知道怎么解决的朋友麻烦告知，临时解决办"
"法就是点击窗口下边的关闭按钮。还有朋友说可以不用动态链接库，我也不知道怎么弄，希望懂的告知。"
"\r\n    其它：本软件原名protel99se鼠标增强工具(protel_increase)，2007年03月25日首先在老古网站(www.laogu.com)"
"上发布，至今已有两年，当时只是想做了自己用，顺便放上来给大家分享。如今，我已经很久不画板了，但没想到两年"
"来，有1600多次下载，还有朋友转载到别的网站，小有一些兴奋，说明我的工作是有意义的。前两天一个名叫xtomfuk "
"的朋友来信要求增加PADS的支持，欣然接受，于是就有了这个版本。由于增加了多个EDA软件的支持，原名不能再"
"用了，姑且就叫EDAHelper吧。为了纪念上一个版本，这一版也选择在3月25日两周年之际发布。\r\n"
"    再罗嗦两句：本软件原理仅限于模拟按键，别的功能暂时做不了。依然不保留版权，任何人都可以修改，但请不要用"
"于营利目的，出问题也不要找我。当然如果你觉得应该增加更多更好功能，别介意联系我，QQ:126186，Email:spacexplorer@163.com。"
"\r\n    最后感谢xtomfuk的建议和测试，以及涛哥(paul)制作的图标。"
"\r\n\r\n第一版(1.0)说明：\r\n"
"    本软件是增强Protel99se鼠标中键功能的工具。\r\n"
"    1. 上下滚动滚轮，屏幕缩放相当于PageUp,PageDown的功能\r\n"
"    2. 按下滚轮，切换有效图层并刷新相当于小键盘'*'的功能\r\n"
"    3. 按住左键拖动器件再按右键可旋转，相当于\"TAB\"的功能\r\n"
"    4. 将本软件(含Hook.dll)放到Protel99se安装目录,运行时会自动启动Protel99se\r\n"
"    5. 仿照“Protel99se鼠标增强软件”(www.zsmcu.com)设计,因我运行她后用Source Insight"
"鼠标拖动时感觉明显变慢且部分操作不符合我的习惯而改写的\r\n"
"    6. 本软件包括两部分, 一是HOOK(.\\src\\test3)实现鼠标钩子和模拟按键，"
"二是主窗口(.\\src\\protel_increase), 安装钩子和显示帮助信息,部分代码源自"
"www.vckbase.com,无版权,任何人可以根据自己的需要把他改了在其它软件如:Orcad, "
"PowerPCB等里运行,也可以让其自适应多个软件的操作,自己发挥吧, 如果你愿意,请EMAIL一份我.";

static char	*AD = 
"捐助不局限于金钱，建议也可以，吆喝也可以\r\n"
"广告：\r\n"
"    目前本人正穷困潦倒，且有闲暇，欲找点兼职养家糊口，专长如下：\r\n"
"    1、精通单片机软、硬件开发，51、ARM等\r\n"
"    2、精通嵌入式TCPIP协议栈\r\n"
"    3、精通嵌入式USB-HOST协议栈(OHCI, EHCI)，基于ST的OS20和OS21开发，支持U盘、移动硬盘等\r\n"
"    4、精通C、C++编程，小型数据库编程\r\n"
"    5、精通LINUX内核编程\r\n"
"    有诚意者请联系：QQ:126186, Email:spacexplorer@163.com";

CEDAHelperDlg::CEDAHelperDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEDAHelperDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEDAHelperDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	bUpdateRunning = 0;

}

void CEDAHelperDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEDAHelperDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEDAHelperDlg, CDialog)
	//{{AFX_MSG_MAP(CEDAHelperDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CHECK_AUTORUN, OnCheckAutorun)
	ON_BN_CLICKED(IDC_BUTTON_MINIMIZE, OnButtonMinimize)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_HELP, OnButtonHelp)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_UPDATE, OnCheckUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEDAHelperDlg message handlers

BOOL CEDAHelperDlg::OnInitDialog()
{
	char	szVerTip[20] = "EDAHelper V";
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	GetDlgItem(IDC_EDIT_DESC)->SetWindowText(AppDesc);

	if(CheckAutorunStats())
	{
		((CButton*)GetDlgItem(IDC_CHECK_AUTORUN))->SetCheck(1);
	}
	
	if(CheckUpdateStats())
	{
		((CButton*)GetDlgItem(IDC_CHECK_UPDATE))->SetCheck(1);
		hUpdateThread = ::CreateThread( NULL, 0, UpdateTask, (LPVOID)&bUpdateRunning, 0, &dwUpdateThreadId );
		SetTimer(100, UPDATE_CHECK_PERIOD, NULL);
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	HookInit();
	installhook();

	// TODO: Add extra initialization here
    m_tnid.cbSize=sizeof(NOTIFYICONDATA);        //定义结构的成员变量
    m_tnid.hWnd=this->m_hWnd;
    m_tnid.uFlags=NIF_MESSAGE|NIF_ICON|NIF_TIP;
    m_tnid.uCallbackMessage=MYWM_NOTIFYICON;     //用户定义的回调消息
	strcat(szVerTip, CURRENT_VERSION);
    _tcscpy(m_tnid.szTip, _T(szVerTip));
	m_tnid.uID=IDR_MAINFRAME;
	m_tnid.hIcon=m_hIcon;
	if(m_hIcon)::DestroyIcon(m_hIcon);

	Shell_NotifyIcon(NIM_ADD,&m_tnid);

	PostMessage(WM_SYSCOMMAND,SC_MINIMIZE,0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEDAHelperDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CEDAHelperDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CEDAHelperDlg::OnCheckAutorun() 
{
	HKEY hKey;
	CString str = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	if (ERROR_SUCCESS != RegCreateKey(HKEY_CURRENT_USER, str, &hKey))
	{
	   MessageBox("打开注册表项失败");
	   RegCloseKey(hKey);
	   return;
	}

	if(((CButton*)GetDlgItem(IDC_CHECK_AUTORUN))->GetCheck())
	{
		if (ERROR_SUCCESS != RegSetValueEx(hKey, "EDAHelper", 0, REG_SZ, (unsigned char *)__argv[0], strlen(__argv[0])))
		{
		   MessageBox("写注册表失败");
		   RegCloseKey(hKey);
		   return;
		}
	}
	else
	{
		if (ERROR_SUCCESS != RegDeleteValue(hKey, "EDAHelper"))
		{
		   MessageBox("写注册表失败");
		   RegCloseKey(hKey);
		   return;
		}

	}

	RegCloseKey(hKey);
}

void CEDAHelperDlg::OnCheckUpdate() 
{
	if(((CButton*)GetDlgItem(IDC_CHECK_UPDATE))->GetCheck())
	{
		if (!bUpdateRunning)
		{
			hUpdateThread = ::CreateThread( NULL, 0, UpdateTask, (LPVOID)&bUpdateRunning, 0, &dwUpdateThreadId );
		}
		SetTimer(100, UPDATE_CHECK_PERIOD, NULL);
		AfxGetApp()->WriteProfileInt("Setting", "CheckUpdate", 1);
	}
	else
	{
		if (bUpdateRunning)
		{
			TerminateThread(hUpdateThread, 0);
			bUpdateRunning = 0;
		}
		AfxGetApp()->WriteProfileInt("Setting", "CheckUpdate", 0);
		KillTimer(100);
	}
}

void CEDAHelperDlg::OnButtonMinimize() 
{
	PostMessage(WM_SYSCOMMAND,SC_MINIMIZE,0);
}

BOOL CEDAHelperDlg::CheckAutorunStats()
{
	HKEY			hKey;
	unsigned char	szAppPath[MAX_PATH];
	unsigned long	dwSize = MAX_PATH;

	CString str = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";

	if (ERROR_SUCCESS != RegCreateKey(HKEY_CURRENT_USER, str, &hKey))
	{
	   MessageBox("打开注册表项失败");
	   RegCloseKey(hKey);
	   return FALSE;
	}
	int ErrCode = RegQueryValueEx(hKey, "EDAHelper", NULL, NULL, szAppPath, &dwSize);
	if(ERROR_SUCCESS == ErrCode)
	{
		if(!strncmp((char*)szAppPath, __argv[0],  dwSize))
		{
			return TRUE;
		}
	}
	else
	{
//		MessageBox("Read register value failed!");
	}


	return FALSE;
}

BOOL CEDAHelperDlg::CheckUpdateStats()
{
	CWinApp*	pApp = AfxGetApp();

	if(pApp->GetProfileInt("Setting", "CheckUpdate", 1))
	{
		return TRUE;
	}
	
	return FALSE;
	
}

void CEDAHelperDlg::OnClose() 
{
	CDialog::OnClose();
}

LRESULT CEDAHelperDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	static bool	IsShowWnd = TRUE;
    if((message==WM_SYSCOMMAND)&&(wParam==SC_MINIMIZE))         //如果是系统消息
	{
		this->ShowWindow(SW_MINIMIZE);
		this->ShowWindow(SW_HIDE);
		IsShowWnd = FALSE;
		return true;
	}
	else if((message==MYWM_NOTIFYICON)&&((lParam==WM_LBUTTONDOWN)||(lParam==WM_RBUTTONUP)))  //如果是用户定义的消息
	{
		if(!IsShowWnd)
		{
			this->ShowWindow(SW_SHOW);
			this->ShowWindow(SW_NORMAL);
			this->SetActiveWindow();   //双击并且出现在最上面
		}
		else
		{
			this->ShowWindow(SW_MINIMIZE);
			this->ShowWindow(SW_HIDE);
		}
		IsShowWnd = !IsShowWnd;
		return true;
	}
#if 0	// this place for resolve the close stop when 
	else if(message == 0xa1)
	{
		typedef void (_stdcall *uninshook)();//定义回调函数的地址 
		uninshook uninstmousehook;
		//	if(hinstDLL=LoadLibrary((LPCTSTR)"hook.dll"))
		{
			uninstmousehook=(uninshook)GetProcAddress(hinstDLL, "UnHook"); 
			uninstmousehook();
			FreeLibrary(hinstDLL);
		}
	}
#endif
		
	return CDialog::WindowProc(message, wParam, lParam);
}

void CEDAHelperDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	Shell_NotifyIcon(NIM_DELETE, &m_tnid);
}

void CEDAHelperDlg::OnButtonHelp() 
{
	static BOOL HelpNotAd = FALSE;
	if(HelpNotAd)
	{
		GetDlgItem(IDC_BUTTON_HELP)->SetWindowText("广告");
		GetDlgItem(IDC_EDIT_DESC)->SetWindowText(AppDesc);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_HELP)->SetWindowText("帮助");
		GetDlgItem(IDC_EDIT_DESC)->SetWindowText(AD);
	}

	HelpNotAd = !HelpNotAd;
	
}


void CEDAHelperDlg::OnTimer(UINT nIDEvent) 
{
	if ((nIDEvent == 100) && (!bUpdateRunning))
	{
		hUpdateThread = ::CreateThread( NULL, 0, UpdateTask, (LPVOID)&bUpdateRunning, 0, &dwUpdateThreadId );
	}
	CDialog::OnTimer(nIDEvent);
}

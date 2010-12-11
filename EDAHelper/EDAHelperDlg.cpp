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
//"2.1.00˵����\r\n"
//"    1�������Զ�������\r\n"
//"    2���޸�orcad���Ҽ��ƶ�������ʹ������\r\n"
//"    3���޸�˵����һֱ������һ�������֣��Ǻ�"
//"    4�������°汾�Զ���⹦��"
//"    4������saber sketch�м����Ź���"

"2.0.10��ʱ��˵����\r\n"
"    1���޸���PADS�а�סCONTROL��SHIFT��ALT��ʱ���м�ԭ����ʧЧ��BUG\r\n"
"    2���޸���PADS�зǻ�ͼ���ڣ���깦��ʧЧ��BUG\r\n"
"    3�����ӷ���棨��WORD2003ת��������֮����ָ��\r\n"
"    4���ϴ���googlecode������Ȥ�����ѿ���ͨ��svn checkout https://dbghelper.googlecode.com/svn/EDAHelper��ô���\r\n"
"2.0.09��ʱ��˵����\r\n"
"    1���޸�ͨ��˫����powerlogic�ļ�ʱ����ҹ���ʧЧ����\r\n"
"    2���޸��м�����ʱ��Ļƫ�Ƶ�����\r\n"
"2.0.08��ʱ��˵����\r\n"
"    1������Protel99se��DXP��ӡԤ���м����ŵ�֧��\r\n"
"2.0.07��ʱ��˵����\r\n"
"    1������OrCAD�Ҽ��϶���֧��\r\n"
"2.0.06��ʱ��˵����\r\n"
"    1������CAM350�Ҽ�ԭ����ʧЧ������\r\n"
"2.0.05��ʱ��˵����\r\n"
"    1��2.0.04�汾�����ٶȱ��������ڴ󲿷�����²������ˢ�²����������Է����ð汾�����ĸ���\r\n"
"    2�����PROTEL99SEԭ��ͼ�������Ҽ��ƶ��󣬰�ס����ٰ��Ҽ�ʱ����������ת������(ֱ������"
"�����˷��������⣬˵���˹��ܺ������õ�����ʵ���Ǻܺ��õĹ��ܣ���Ҳ������ԣ�PCB��Ҳ����)\r\n"
"2.0.04��ʱ��˵����\r\n"
"    1����������Կ����Ҽ��϶�ʱˢ�²�����������\r\n"
"2.0.03��ʱ��˵����\r\n"
"    1������CAM350��֧�֣��������ַŴ���С���Ҽ��϶�\r\n"
"2.0.02��ʱ��˵����\r\n"
"    1���޸���BlazeRouter���Ҽ��϶��󻹻���ֲ˵���BUG\r\n"
"2.0.01��ʱ��˵����\r\n"
"    1���������ƶ�����Ч��\r\n"
"    2�����PROTEL���������BUG\r\n"
"    3������PROTEL99SEԭ��ͼ�Ҽ��ƶ�����\r\n"
"    4��֧��powerlogic\r\n"
"    5��֧��BlazeRouter\r\n"
"�ڶ���(2.0)˵����\r\n"
"    �������ǲ���EDA�����������ǿ���ߣ���EDAHelper.exe��Hook.dllͬʱ�ŵ�����Ŀ¼������EDAHelper.exe"
"���У������Ѳ����Զ�����EDA������֧��protel99se��DXP��POWERPCB��OrCAD��capture��\r\n"
"\r\n���protel99se,DXP(���ܺʹ�ǰһ��)��\r\n"
"    1. ���Ϲ������� --> �Ŵ��൱��PageUp(���ܸĵ�����ԭ���Ŀ�ݼ�������Ͳ�����)\r\n"
"    2. ���¹������� --> �Ŵ��൱��PageDown(���ܸĵ�����ԭ���Ŀ�ݼ�������Ͳ�����)\r\n"
"    3. ���¹���     --> �л���Чͼ�㲢ˢ�£��൱��С����'*'�Ĺ���\r\n"
"    4. �������Ŀ�� --> �ٰ��Ҽ�����ת���൱�ڿո���Ĺ��ܣ��ڲ���ʱ�ǳ����ã�������\r\n"
"\r\n���powerpcb(֧��PADS2005SP2��PADS2007����İ汾û�в����Ӧ��Ҳ֧��)��\r\n"
"    1. ��ס����Ҽ��ƶ������ƶ����������൱��SHIFT+���ֻ���ALT+���֣���ƽ��\r\n"
"    2. ���Ϲ������� --> �Ŵ��൱��PageUp\r\n"
"    3. ���¹������� --> ��С���൱��PageDown\r\n" 
"    4. ����м���� --> �л�ͼ�㣬�൱��F4��ԭ�м�������ܷ���\r\n" 
"\r\n���OrCAD��\r\n"
"    1. ���Ϲ������� --> �Ŵ��൱����ĸ\"I\"��\r\n"
"    2. ���¹������� --> ��С�൱����ĸ\"O\"��\r\n" 
"\r\n    Ŀǰ�������⣺���ϵͳͼ��رջ�����С������ʱ����Լ��ֹͣ��Ӧ3 ���ӣ�������Hookû�з���"
"װ���ҷ���ͬ������Ҳ�����������⣬���ǲ�֪����ô�������λ֪����ô����������鷳��֪����ʱ�����"
"�����ǵ�������±ߵĹرհ�ť����������˵���Բ��ö�̬���ӿ⣬��Ҳ��֪����ôŪ��ϣ�����ĸ�֪��"
"\r\n    ������������ԭ��protel99se�����ǿ����(protel_increase)��2007��03��25���������Ϲ���վ(www.laogu.com)"
"�Ϸ����������������꣬��ʱֻ���������Լ��ã�˳�����������ҷ�����������Ѿ��ܾò������ˣ���û�뵽����"
"������1600������أ���������ת�ص������վ��С��һЩ�˷ܣ�˵���ҵĹ�����������ġ�ǰ����һ������xtomfuk "
"����������Ҫ������PADS��֧�֣���Ȼ���ܣ����Ǿ���������汾�����������˶��EDA������֧�֣�ԭ��������"
"���ˣ����Ҿͽ�EDAHelper�ɡ�Ϊ�˼�����һ���汾����һ��Ҳѡ����3��25��������֮�ʷ�����\r\n"
"    ���������䣺������ԭ��������ģ�ⰴ������Ĺ�����ʱ�����ˡ���Ȼ��������Ȩ���κ��˶������޸ģ����벻Ҫ��"
"��Ӫ��Ŀ�ģ�������Ҳ��Ҫ���ҡ���Ȼ��������Ӧ�����Ӹ�����ù��ܣ��������ϵ�ң�QQ:126186��Email:spacexplorer@163.com��"
"\r\n    ����лxtomfuk�Ľ���Ͳ��ԣ��Լ��θ�(paul)������ͼ�ꡣ"
"\r\n\r\n��һ��(1.0)˵����\r\n"
"    ����������ǿProtel99se����м����ܵĹ��ߡ�\r\n"
"    1. ���¹������֣���Ļ�����൱��PageUp,PageDown�Ĺ���\r\n"
"    2. ���¹��֣��л���Чͼ�㲢ˢ���൱��С����'*'�Ĺ���\r\n"
"    3. ��ס����϶������ٰ��Ҽ�����ת���൱��\"TAB\"�Ĺ���\r\n"
"    4. ��������(��Hook.dll)�ŵ�Protel99se��װĿ¼,����ʱ���Զ�����Protel99se\r\n"
"    5. ���ա�Protel99se�����ǿ������(www.zsmcu.com)���,��������������Source Insight"
"����϶�ʱ�о����Ա����Ҳ��ֲ����������ҵ�ϰ�߶���д��\r\n"
"    6. ����������������, һ��HOOK(.\\src\\test3)ʵ����깳�Ӻ�ģ�ⰴ����"
"����������(.\\src\\protel_increase), ��װ���Ӻ���ʾ������Ϣ,���ִ���Դ��"
"www.vckbase.com,�ް�Ȩ,�κ��˿��Ը����Լ�����Ҫ��������������������:Orcad, "
"PowerPCB��������,Ҳ������������Ӧ��������Ĳ���,�Լ����Ӱ�, �����Ը��,��EMAILһ����.";

static char	*AD = 
"�����������ڽ�Ǯ������Ҳ���ԣ�ߺ��Ҳ����\r\n"
"��棺\r\n"
"    Ŀǰ�����������ʵ���������Ͼ�����ҵ��ְ���Һ��ڣ�ר�����£�\r\n"
"    1����ͨ��Ƭ������Ӳ��������51��ARM��\r\n"
"    2����ͨǶ��ʽTCPIPЭ��ջ\r\n"
"    3����ͨǶ��ʽUSB-HOSTЭ��ջ(OHCI, EHCI)������ST��OS20��OS21������֧��U�̡��ƶ�Ӳ�̵�\r\n"
"    4����ͨC��C++��̣�С�����ݿ���\r\n"
"    5����ͨLINUX�ں˱��\r\n"
"    �г���������ϵ��QQ:126186, Email:spacexplorer@163.com";

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
    m_tnid.cbSize=sizeof(NOTIFYICONDATA);        //����ṹ�ĳ�Ա����
    m_tnid.hWnd=this->m_hWnd;
    m_tnid.uFlags=NIF_MESSAGE|NIF_ICON|NIF_TIP;
    m_tnid.uCallbackMessage=MYWM_NOTIFYICON;     //�û�����Ļص���Ϣ
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
	   MessageBox("��ע�����ʧ��");
	   RegCloseKey(hKey);
	   return;
	}

	if(((CButton*)GetDlgItem(IDC_CHECK_AUTORUN))->GetCheck())
	{
		if (ERROR_SUCCESS != RegSetValueEx(hKey, "EDAHelper", 0, REG_SZ, (unsigned char *)__argv[0], strlen(__argv[0])))
		{
		   MessageBox("дע���ʧ��");
		   RegCloseKey(hKey);
		   return;
		}
	}
	else
	{
		if (ERROR_SUCCESS != RegDeleteValue(hKey, "EDAHelper"))
		{
		   MessageBox("дע���ʧ��");
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
	   MessageBox("��ע�����ʧ��");
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
    if((message==WM_SYSCOMMAND)&&(wParam==SC_MINIMIZE))         //�����ϵͳ��Ϣ
	{
		this->ShowWindow(SW_MINIMIZE);
		this->ShowWindow(SW_HIDE);
		IsShowWnd = FALSE;
		return true;
	}
	else if((message==MYWM_NOTIFYICON)&&((lParam==WM_LBUTTONDOWN)||(lParam==WM_RBUTTONUP)))  //������û��������Ϣ
	{
		if(!IsShowWnd)
		{
			this->ShowWindow(SW_SHOW);
			this->ShowWindow(SW_NORMAL);
			this->SetActiveWindow();   //˫�����ҳ�����������
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
		typedef void (_stdcall *uninshook)();//����ص������ĵ�ַ 
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
		GetDlgItem(IDC_BUTTON_HELP)->SetWindowText("���");
		GetDlgItem(IDC_EDIT_DESC)->SetWindowText(AppDesc);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_HELP)->SetWindowText("����");
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
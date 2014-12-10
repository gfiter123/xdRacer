// UpdateHardwareDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UpdateHardware.h"
#include "UpdateHardwareDlg.h"
#include "XH_Game_Updata_Hardware_Code.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CUpdateHardwareDlg 对话框




CUpdateHardwareDlg::CUpdateHardwareDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUpdateHardwareDlg::IDD, pParent)
	, m_strFilePath(_T(""))
	, m_strLog(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pMotionSock = NULL;
	
}

void CUpdateHardwareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_IpCtrl);
	DDX_Text(pDX, IDC_EDIT1, m_strFilePath);
	DDX_Text(pDX, IDC_STATIC_LOG, m_strLog);
}

BEGIN_MESSAGE_MAP(CUpdateHardwareDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_START, &CUpdateHardwareDlg::OnBnClickedButtonStart)
	ON_MESSAGE(WM_UDPMSG,&CUpdateHardwareDlg::OnPauseCommand)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CUpdateHardwareDlg 消息处理程序

BOOL CUpdateHardwareDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	SetIpCtrl("192.168.1.101");
	CreateSocket();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CUpdateHardwareDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CUpdateHardwareDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CUpdateHardwareDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CUpdateHardwareDlg::SetIpCtrl(CString ip)
{
	int f1,f2,f3,f4,f5;
	
	sscanf_s(ip.GetBuffer(0),"%d.%d.%d.%d",&f1,&f2,&f3,&f4);
	byte b1,b2,b3,b4;
	b1 = 0xff&f1;
b2 = 0xff&f2;
b3 = 0xff&f3;
b4 = 0xff&f4;
	m_IpCtrl.SetAddress(b1,b2,b3,b4);
UpdateData(FALSE);
	
}
void CUpdateHardwareDlg::CreateSocket()
{
	if (!m_pMotionSock)
	{
		m_pMotionSock = new XHMotionUDPsock(this->GetSafeHwnd());
		m_pMotionSock->Create(8001);
	}
	strcpy_s(m_pMotionSock->m_szRemoteIp1,20,"192.168.1.101");
	m_pMotionSock->m_nRemotePort1 = 8002;
	
}
void CUpdateHardwareDlg::OnBnClickedButtonStart()
{
	UpdateData(TRUE);
	CString IP;
	BYTE f1,f2,f3,f4;
	TCHAR temp[10] = "\0";
	if (m_IpCtrl.GetAddress(f1,f2,f3,f4)<4)
	{
		MessageBox(_T("请重新写入IP地址,填写完整."), _T("IP address 错误"), MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	IP = _itoa(f1,temp,10);
	IP += _T('.');
	IP += _itoa(f2,temp,10);
	IP += _T('.');
	IP += _itoa(f3,temp,10);
	IP += _T('.');
	IP += _itoa(f4,temp,10);
	strcpy_s(m_pMotionSock->m_szRemoteIp1,20,IP.GetBuffer(0));
	if (!infile.is_open())
	{
		CFileDialog fDlg(TRUE);
		if (fDlg.DoModal() == IDOK)
		{
			m_strFilePath = fDlg.GetPathName();
			UpdateData(FALSE);
		}
	}
	commandSign = REQUEST;
	SetTimer(1,1500,NULL);

}

void CUpdateHardwareDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		if (commandSign == REQUEST)
		{
			Requst();
		}
	}

	CDialog::OnTimer(nIDEvent);
}


void CUpdateHardwareDlg::Requst()
{
	memset(command,0,MAX_COMMAND_LONG);
	XHUpdateHardwareRequest(command,commandLen);
	m_pMotionSock->Send1(command,commandLen);
}
void CUpdateHardwareDlg::CheckOne()
{
	memset(command,0,MAX_COMMAND_LONG);
	XHUpdateHardwareCheckFlashOne(command,commandLen);
	m_pMotionSock->Send1(command,commandLen);
}
void CUpdateHardwareDlg::CheckTwo()
{
	memset(command,0,MAX_COMMAND_LONG);
	XHUpdateHardwareCheckFlashTwo(command,commandLen);
	m_pMotionSock->Send1(command,commandLen);
}
void CUpdateHardwareDlg::DownLoadData()
{
	char buf[1024] = {0};
	static bool tmp_bFinish;
	if (m_nSendSize == 0)
	{
		if (infile.is_open())
		{
			infile.close();
			
		}
		infile.open(m_strFilePath.GetBuffer(),ios::binary);
		infile.clear();
		infile.seekg(0,ios::beg);
		m_nSendSize = 1;//正在下载
		
	}
	if (m_nSendSize == 1)
	{

		if (infile.is_open())
		{
			for (int i = 0; i< 1024;i++)
			{
				if (!infile.read(buf+i,1))
				{
					m_nSendSize = 2;
					break;
				}
			}
			XHUpdateHardwareDownload(command,commandLen,buf);
			m_pMotionSock->Send1(command,commandLen);
			
		}
		else
		{
			AfxMessageBox("打开文件失败！！");
			return;
		}
	}
	else 
	{
		
		Finish();
		commandSign = FINISH;
		m_nSendSize = 0;
		m_strLog = "成功";
		UpdateData(FALSE);
		return;
	}
}
void CUpdateHardwareDlg::Finish()
{
	memset(command,0,MAX_COMMAND_LONG);
	XHUpdateHardwareFinish(command,commandLen);
	m_pMotionSock->Send1(command,commandLen);
}

LRESULT CUpdateHardwareDlg::OnPauseCommand(WPARAM wp,LPARAM lp)
{
	short len = (short)wp;
	XHMotionUDPsock *xhsock = (XHMotionUDPsock *)lp;
	if (commandSign == REQUEST)
	{
		if(XHIsUpdateHardwareRequest(xhsock->m_szRecvBuf))
		{
			CheckOne();
			commandSign = CHECK_ONE;
			m_strLog = "成功";
			KillTimer(1);
			UpdateData(FALSE);
			return 1;
		}
		
	}
	else if (commandSign == CHECK_ONE)
	{
		if (XHIsUpdateHardwareCheckFlashOne(xhsock->m_szRecvBuf))
		{
			CheckTwo();
			commandSign = CHECK_TWO;
			m_strLog = "成功";
			UpdateData(FALSE);
			return 1;
		}
		
	}
	else if (commandSign == CHECK_TWO)
	{
		if (XHIsUpdateHardwareCheckFlashTwo(xhsock->m_szRecvBuf))
		{
			m_nSendSize = 0; //开始下载 
			DownLoadData();
			commandSign = DOWNLOAD;
			m_strLog = "正在下载";
			UpdateData(FALSE);
			return 1;
		}
		
	}
	else if (commandSign == DOWNLOAD)
	{
		if (XHIsUpdateHardwareDownload(xhsock->m_szRecvBuf))
		{
			DownLoadData();
			
			m_strLog = "正在下载";
			UpdateData(FALSE);
			return 1;
		}
	}
	else if (commandSign == FINISH)
	{

		if (XHIsUpdateHardwareFinish(xhsock->m_szRecvBuf))
		{
			commandSign = REQUEST;
			m_strLog = "下载完成";
			UpdateData(FALSE);
			return 1;
		}
	}
	m_strLog.Format("%02X %02X %02X %02X",xhsock->m_szRecvBuf[0],xhsock->m_szRecvBuf[1]&0xff,xhsock->m_szRecvBuf[2],xhsock->m_szRecvBuf[3]);
	UpdateData(FALSE);
	return 1;

}
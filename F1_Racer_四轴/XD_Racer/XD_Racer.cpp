
#include "XHUDPSocket.h"
#include <CommCtrl.h>
#include "XHMotionUDPsock.h"
#include "XHControlSocket.h"
#include "F1RacerMotionProtocal.h"


#include "XD_Racer.h"
#include "XHConfig.h"

#define  MAX_PLUSE    80000
#define  INTERVAL_TIME  0.02
#pragma comment(lib,"ws2_32.lib")

#define ZERO_TIMER 2
#define CONNECT_TIMER 3
#define  AUTO_TIMER   4


struct DecodeUnit DlgMessage[] ={
	WM_INITDIALOG ,OnInitDialog,
	WM_COMMAND,DoCommand,
	WM_CLOSE,OnClose,
	WM_VSCROLL,OnVscroll,
};

struct DecodeUnit CommandMessage[] = {
	IDOK,OnClose,
	IDCANCEL,OnClose,
	IDB_RUN_GAME,OnBtnRunGameClick,
	IDC_BTN_STOP,OnBtnStopClick,
	IDC_BTN_ZERO,OnBtnSetZeroClick,
	
};


HINSTANCE hInst;
XHUDPSocket *sock;
XHMotionUDPsock *motionSocket;
XHControlSocket *controlSocket;
bool Leave_Thread=true;
bool g_bConnectConnect = false;
UINT g_nMotionPort;
UINT g_nControlPort;
UINT g_nGamePort;
HANDLE threadTime = INVALID_HANDLE_VALUE;
HANDLE threadPosition = INVALID_HANDLE_VALUE;
HANDLE hEvent = INVALID_HANDLE_VALUE;
HWND g_hDlg;
int g_nCount;
bool g_bDownlod = false;
bool g_bConnect = false;
int g_nRunNum = 1;
std::vector<MMData> gMMDataList;
std::vector<KKData> gKKDataList;
int g_nAutorunCount = 0;

enum key{
	KEY_ENTER = 1,
};
int WINAPI WinMain(HINSTANCE hInstance,		
				   HINSTANCE hPrevInstance, 
				   LPSTR    lpCmdLine,		
				   int       nCmdShow)		

{
	hInst = hInstance;
	DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG1),NULL,DlgPorc);
	
	return (0);
}



INT_PTR CALLBACK DlgPorc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	int i;
	for (i = 0;i < dim(DlgMessage);i++)
	{
		if(DlgMessage[i].message == message)
			(*DlgMessage[i].fun)(hDlg,message,wParam,lParam);
	}

	return FALSE;
}

LONG DoCommand(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	int i ;
	for(i =0 ;i< dim(CommandMessage);++i)
	{
		if(CommandMessage[i].message == LOWORD(wParam))
			(*CommandMessage[i].fun)(hDlg,message,wParam,lParam);
	}
	return TRUE;
}


LONG OnClose(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{

	MotionProtocal com = MakeExitGameMode();
	motionSocket->Send1(com);
	Sleep(20);
	motionSocket->Send2(com);
	WSACleanup();
	
	EndDialog(hDlg,0);
	return TRUE;
}

long OnInitDialog(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	WORD wVer = MAKEWORD(2,0);
	WSADATA ws;
	WSAStartup(wVer,&ws);
	g_hDlg = hDlg;
	if (ws.wVersion != wVer)
	{
		OutputDebugString(TEXT("startup error"));
	}
	
	sock = new XHUDPSocket(hDlg);
	motionSocket = new XHMotionUDPsock(hDlg);
	controlSocket = new XHControlSocket(hDlg);
	ReadConfig();
	sock->Create(g_nGamePort);
	motionSocket->Create(g_nMotionPort);
	controlSocket->Create(g_nControlPort);
	SendMessageW(GetDlgItem(hDlg,IDS_AXIS1),TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(-32767,32766));
	SendMessageW(GetDlgItem(hDlg,IDS_AXIS2), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(-32767,32766));
	SendMessageW(GetDlgItem(hDlg,IDS_AXIS3),TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(-32767,32766));
	SendMessageW(GetDlgItem(hDlg,IDS_AXIS4),TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(-32767,32766));
	hEvent = CreateEvent(NULL,TRUE,FALSE," Event");

	ConnectControlHost(hDlg);

	SendMessage(GetDlgItem(hDlg,IDC_COMBO_SAFE),CB_ADDSTRING,0,(LPARAM)("跟随安全带"));
	SendMessage(GetDlgItem(hDlg,IDC_COMBO_SAFE),CB_ADDSTRING,0,(LPARAM)("强制运动"));
	SendMessage(GetDlgItem(hDlg,IDC_COMBO_SAFE),CB_ADDSTRING,0,(LPARAM)("强制不动"));
	SendMessage(GetDlgItem(hDlg,IDC_COMBO_SAFE),CB_SETCURSEL,0,(LPARAM)0);
	char buf[10]={0};
	itoa(g_nRunNum,buf,10);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_RUN_NUM),buf);
	//openKeyData();
	return 1;
}

long OnVscroll(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	
	HWND hwndScrollBar = (HWND) lParam;       // handle to scroll bar   lParam一般表示窗口句柄 <b>      </b>   
	int nPos=200;
	nPos=SendMessage(hwndScrollBar,TBM_GETPOS,0,0);
	{
		if (hwndScrollBar == GetDlgItem(hDlg,IDS_AXIS1))
		{
			axis1 = (float)nPos*MAX_AXIS_LONG/MAX_PLUSE;
		}
		if (hwndScrollBar == GetDlgItem(hDlg,IDS_AXIS2))
		{
			axis2 = (float)nPos*MAX_AXIS_LONG/MAX_PLUSE;
		}
		if (hwndScrollBar == GetDlgItem(hDlg,IDS_AXIS3))
		{
			axis3 = (float)nPos*MAX_AXIS_LONG/MAX_PLUSE;
		}
		if (hwndScrollBar == GetDlgItem(hDlg,IDS_AXIS4))
		{
			axis4 = (float)nPos*MAX_AXIS_LONG/MAX_PLUSE;
		}
		 
	}
	return 1;

}
void ConnectControlHost(HWND hDlg)
{
	g_bConnectConnect = false;
	SetTimer(hDlg,CONNECT_TIMER,1000,OnTimerProc);
}
bool ReadConfig()
{
	char buf[MAX_PATH];
	GetModuleFileName(NULL,buf,MAX_PATH);
	for(int i = strlen(buf);i >= 0; i--)
	{
		if (buf[i] == '.')
		{
			buf[i] = '\0';
			break;
		}
		buf[i] = 0;
	}
	strcat(buf,".config");
	ReadConfigFromFile(buf);
	
	char XHvalue[64]={0};
	
	//游戏机IP
	if (GetConfigFromMem("MotionIp1",XHvalue))
	{
		strcpy(motionSocket->m_szRemoteIp1,XHvalue);
	}
	if (GetConfigFromMem("MotionPort1",XHvalue))
	{
		motionSocket->m_nRemotePort1 = atoi(XHvalue);
	}
	if (GetConfigFromMem("MotionIp2",XHvalue))
	{
		strcpy(motionSocket->m_szRemoteIp2,XHvalue);
	}
	if (GetConfigFromMem("MotionPort2",XHvalue))
	{
		motionSocket->m_nRemotePort2 = atoi(XHvalue);
	}
	if (GetConfigFromMem("ControlIp",XHvalue))
	{
		strcpy(controlSocket->m_szRemoteIp,XHvalue);
	}
	if (GetConfigFromMem("ControlPort",XHvalue))
	{
		controlSocket->m_nRemotePort = atoi(XHvalue);
	}

	if (GetConfigFromMem("LocalMotionPort",XHvalue))
	{
		g_nMotionPort = atoi(XHvalue);
	}

	if (GetConfigFromMem("LocalGamePort",XHvalue))
	{
		g_nGamePort = atoi(XHvalue);
		
	}
	if (GetConfigFromMem("LocalControlPort",XHvalue))
	{
		g_nControlPort = atoi(XHvalue);

	}
	if (GetConfigFromMem("M_Q",XHvalue))
	{
		M_Q = atof(XHvalue);

	}
	if (GetConfigFromMem("AVE_N",XHvalue))
	{
		AVE_N = atof(XHvalue);

	}
	else{
		OutputDebugString(TEXT("cannot read config\n"));
		return false;
	}
	return true;
	
}


LONG OnBtnRunGameClick(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	

	 LRESULT r = SendMessage(GetDlgItem(hDlg,IDC_CHECK_AUTO),BM_GETCHECK,0,0); 
	if ( r == BST_CHECKED)
	{
		g_nAutorunCount = 0;
		SetTimer(hDlg,AUTO_TIMER,1000,OnTimerProc);
	}

	Leave_Thread=false;
	DWORD dwCode;
	if(threadTime != INVALID_HANDLE_VALUE)
	{
		GetExitCodeThread(threadTime, &dwCode);
		if(dwCode == STILL_ACTIVE)
		{
			TerminateThread(threadTime, dwCode);
			CloseHandle(threadTime);
		}
		threadTime = INVALID_HANDLE_VALUE;
	}
	if(threadPosition != INVALID_HANDLE_VALUE)
	{
		GetExitCodeThread(threadPosition, &dwCode);
		if(dwCode == STILL_ACTIVE)
		{
			TerminateThread(threadPosition, dwCode);
			CloseHandle(threadPosition);
		}
		threadPosition = INVALID_HANDLE_VALUE;
	}
	DWORD dw;
	Leave_Thread=true;
	threadTime = CreateThread(NULL,NULL,TimerThreadProc,LPVOID(NULL),NULL,&dw);
	threadPosition = CreateThread(NULL,NULL,PositionThreadProc,LPVOID(NULL),NULL,&dw);
	return 1;
}

float axis1 = 0.0;
float axis2 = 0.0;
float axis3 = 0.0;
float axis4 = 0.0;

static void MakePlane(int & axi1,int & axi2,int & axi3,int & axi4)
{
	float x[4] = {-270,270,-270,270};
	float y[4] = {550,550,-550,-550};
	float ax1= (float)axi1;
	float ax2= (float)axi2;
	float ax3= (float)axi3;
	float ax4= (float)axi4;
	float newZ = 0.0;
	float z[4] = {ax1,ax2,ax3,newZ};
	float A = (y[2] - y[0])*(z[2]-z[0]) - (z[1] - z[0])*(y[2]-y[0]); 
	float B = (x[2] - x[0])*(z[1]-z[0]) - (x[1] - x[0])*(z[2]-z[0]);
	float C = (x[1] - x[0])*(y[2]-y[0]) - (x[2] - x[0])*(y[1]-y[0]);
	float D = -(A * x[1] + B * y[1] + C * z[1]);
	newZ = -(x[3]*A + y[3]*B + D)/C;
	//if (abs(newZ - ax4) > 5000)
	{
		axi4 = newZ;
	}

}
DWORD WINAPI PositionThreadProc(LPVOID lp)
{
	int nError_Code_1;
	int x1,y1;
	int x2,y2;
	OutputDebugString("BEGIN Position\n");
	MotionProtocal com = MakeSetZero();
	motionSocket->Send1(com);
	Sleep(1000);
	motionSocket->Send2(com);
	Sleep(4000);
	com = MakeEnterGameMode();
	motionSocket->Send1(com);
	Sleep(20);
	motionSocket->Send2(com);
	FILE *fp;
	if ((fp = fopen("d:\\1.txt","w+")) == NULL)
	{
		OutputDebugString("open file fail\n");
		return 1;
	}
	int tempx = 0;
	int tempy = 0;
	com = MakeRunGameData_1(tempx,tempy);
	motionSocket->Send1(com);
	Sleep(20);
	tempx = 0;
	tempy = 0;
	com = MakeRunGameData_2(tempx,tempy);
	motionSocket->Send2(com);

	while (Leave_Thread)
	{
		WaitForSingleObject(hEvent,INFINITE);
		ResetEvent(hEvent);
		x1 = axis1*MAX_PLUSE/MAX_AXIS_LONG;
		y1 = axis2*MAX_PLUSE/MAX_AXIS_LONG;
		x2 = axis3*MAX_PLUSE/MAX_AXIS_LONG;
		y2 = axis4*MAX_PLUSE/MAX_AXIS_LONG;
		
		if (x1 >= MAX_PLUSE/2)
		{
			x1 = MAX_PLUSE/2;
			OutputDebugString("x\n");
		}
		if (x1 <= -MAX_PLUSE/2)
		{
			x1 = -MAX_PLUSE/2;
			OutputDebugString("x");
		}
		
		if (x2 >= MAX_PLUSE/2)
		{
			x2 = MAX_PLUSE/2;
			OutputDebugString("x");
		}
		if (x2 <= -MAX_PLUSE/2)
		{
			x2 = -MAX_PLUSE/2;
			OutputDebugString("x");
		}
		if (y1 >= MAX_PLUSE/2)
		{
			y1 = MAX_PLUSE/2;
			OutputDebugString("y");
		}
		if (y1 <= -MAX_PLUSE/2)
		{
			y1 = -MAX_PLUSE/2;
			OutputDebugString("y");
		}
		if (y2 >= MAX_PLUSE/2)
		{
			y2 = MAX_PLUSE/2;
			OutputDebugString("y");
		}
		if (y2 <= -MAX_PLUSE/2)
		{
			y2 = -MAX_PLUSE/2;
			OutputDebugString("y");
		}
		//MakePlane(x1,y1,x2,y2);
		
		SendMessageW(GetDlgItem(g_hDlg,IDS_AXIS1),TBM_SETPOS, (WPARAM)1,(LPARAM)(int)x1*65535/MAX_PLUSE);
		SendMessageW(GetDlgItem(g_hDlg,IDS_AXIS2),TBM_SETPOS, (WPARAM)1,(LPARAM)(int)y1*65535/MAX_PLUSE);
		SendMessageW(GetDlgItem(g_hDlg,IDS_AXIS3),TBM_SETPOS, (WPARAM)1,(LPARAM)(int)x2*65535/MAX_PLUSE);
		SendMessageW(GetDlgItem(g_hDlg,IDS_AXIS4),TBM_SETPOS, (WPARAM)1,(LPARAM)(int)y2*65535/MAX_PLUSE);

		

		com = MakeRunGameData_1(x1,y1);
		
		motionSocket->Send1(com);
		Sleep(20);
		com = MakeRunGameData_2(x2,y2);
		static int count = 0;
		fprintf(fp,"%d\t%d\t%d\t%d\t%d\t\n",count++,x1,y1,x2,y2);
		fflush(fp);
		motionSocket->Send2(com);

	}
	fclose(fp);
	return 0;
}

DWORD WINAPI TimerThreadProc(LPVOID lp)
{
	OutputDebugString("BEGIN TIMER\n");
	LARGE_INTEGER   lOldCounter; 
	LARGE_INTEGER   lCurrentCounter; 
	LARGE_INTEGER   lFrequency;
	QueryPerformanceFrequency(&lFrequency); 
	double gIncreament = 0.0;
	double increament=0.0;
	QueryPerformanceCounter(&lOldCounter);//获取当前的计数 
	//计算时间增量，（单位：秒） 
	while(Leave_Thread)
	{
		QueryPerformanceCounter(&lCurrentCounter);//获取当前的计数 
		increament=(double)(lCurrentCounter.QuadPart-lOldCounter.QuadPart)/(double)lFrequency.QuadPart;
		lOldCounter = lCurrentCounter;
		gIncreament += increament;
		if(gIncreament >= INTERVAL_TIME)
		{
			gIncreament -= INTERVAL_TIME;
			//SetEvent(hEvent);
		}
	}
	OutputDebugString("END TIMER\n");
	return 0;
}

LONG OnBtnSetZeroClick(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	MotionProtocal com = MakeSetZero();
	motionSocket->SendAll(com);
	EnableWindow(GetDlgItem(hDlg,IDC_BTN_ZERO),false);
	EnableWindow(GetDlgItem(hDlg,IDC_BTN_STOP),false);
	EnableWindow(GetDlgItem(hDlg,IDB_RUN_GAME),false);
	EnableWindow(GetDlgItem(hDlg,IDC_BTN_DOWNLOAD),false);
	EnableWindow(GetDlgItem(hDlg,IDC_BTN_PLAY_MOVIE),false);
	SetTimer(hDlg,2,3000,OnTimerProc);
	return 1;
}
LONG OnBtnStopClick(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	MotionProtocal com;
	//motionSocket->SendAll(com);
	Leave_Thread=false;
	DWORD dwCode;
	if(threadTime != INVALID_HANDLE_VALUE)
	{
		GetExitCodeThread(threadTime, &dwCode);
		if(dwCode == STILL_ACTIVE)
		{
			TerminateThread(threadTime, dwCode);
			CloseHandle(threadTime);
		}
		threadTime = INVALID_HANDLE_VALUE;
	}
	if(threadPosition != INVALID_HANDLE_VALUE)
	{
		GetExitCodeThread(threadPosition, &dwCode);
		if(dwCode == STILL_ACTIVE)
		{
			TerminateThread(threadPosition, dwCode);
			CloseHandle(threadPosition);
		}
		threadPosition = INVALID_HANDLE_VALUE;
	}
	Sleep(300);
	com = MakeExitGameMode();
	motionSocket->Send1(com);
	Sleep(20);
	motionSocket->Send2(com);

	DWORD dw;
	Leave_Thread=true;
	return 1;
}




VOID CALLBACK OnTimerProc(HWND hDlg,UINT message,UINT nIDEvent,DWORD lParam)
{

	if (nIDEvent == ZERO_TIMER)
	{
		EnableWindow(GetDlgItem(hDlg,IDC_BTN_ZERO),true);
		EnableWindow(GetDlgItem(hDlg,IDC_BTN_STOP),true);
		EnableWindow(GetDlgItem(hDlg,IDB_RUN_GAME),true);
		EnableWindow(GetDlgItem(hDlg,IDC_BTN_DOWNLOAD),true);
		EnableWindow(GetDlgItem(hDlg,IDC_BTN_PLAY_MOVIE),true);
		KillTimer(hDlg,2);
	}
	if (nIDEvent == CONNECT_TIMER)
	{
		if (!g_bConnect)
		{
			OnBtnConnectClick(hDlg,0,0,0);
			SetWindowText(GetDlgItem(hDlg,IDC_BTN_CONNECT),"正在连接");
			
		//	ShowLog("正在连接");
		}
		
	}
	if (nIDEvent == AUTO_TIMER)
	{
		SetKeyDown();
	}
}


LONG OnBtnConnectClick(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	//controlSocket->Send("CONNECT#DMP");
	return 1;
}

void SendControlReady(HWND hDlg)
{
	//controlSocket->Send("READY#00");
	return;
}

void ShowLog(char *buf)
{
	char temp[MAX_PATH] = {0};
	SYSTEMTIME st;
	GetSystemTime(&st);
	sprintf_s(temp,MAX_PATH,"%02d:%02d:%02d>",st.wHour,st.wMinute,st.wSecond);
	strcat_s(temp,buf);
	HWND  hListLog = GetDlgItem(g_hDlg,IDC_LIST_LOG);
	SendMessage(hListLog,LB_ADDSTRING,NULL,(LPARAM)temp);

}

void SendKeyMessage(char *title,int key_value)
{

	HWND wnd = FindWindow(NULL,title);
	if (wnd == NULL)
	{
		char buf[MAX_PATH]={0};
		strcpy(buf,title);
		strcat_s(buf,"没有打开");
		ShowLog(buf);
		return;
	}
	int key;
	switch (key_value)
	{
		case KEY_ENTER:
			key = VK_RETURN;
			break;
		
	}
	
	PostMessage(wnd,WM_KEYDOWN,key, NULL);

}
bool Pause(char *buf,KKData &outData)
{
	outData.nIndex = 0;
	outData.nKey_value = 0;
	outData.nTime = 0;
	
	// Token seperators.

	int Position = 0;
	int count = 0;
	char splits[10][10] = {0};
	int m = 0, n= 0;
	while (*buf++ != 0)
	{
		if (*buf == '\t')
		{
			m++;
			n=0;
			continue;
		}
		splits[m][n++] = *buf;
	}
	//i = _wtoi(s); // if you use wide charater formats 
	//i = _atoi(s); // otherwise 

	if(m < 2 || splits[2][0]==0)
		return false;
	{
		outData.nIndex = atoi((LPCSTR)splits[0]);
		outData.nTime = atoi((LPCSTR)splits[1]);
		outData.nKey_value = atoi((LPCSTR)splits[2]);
		
	}
	return true;

}

void openKeyData()
{
	FILE *fp;
	if ((fp =fopen("dirt3_auto.data","r+")) == NULL)
	{
		MessageBox(g_hDlg,"不能打开文件","警告",MB_OK);
	}
	char buf[MAX_PATH] = {0};
	fgets(buf,MAX_PATH,fp);
	gKKDataList.clear();
	while(fgets(buf,MAX_PATH,fp) != NULL)
	{
		KKData data;
		if(Pause(buf,data))
			gKKDataList.push_back(data);
	}
	fclose(fp);
	return;

}



void SetKeyDown()
{
	static int index = 0;;
	g_nAutorunCount++;
	if (g_nAutorunCount == gKKDataList[index].nTime)
	{
		SendKeyMessage("DiRT 3",gKKDataList[index].nKey_value);
	}
	
}
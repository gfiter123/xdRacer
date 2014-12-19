#include "XHUDPSocket.h"
#include "XD_DOF.h"
#include "XD_Racer.h"
#include "XD_OutSim.h"
#include <stdio.h>
#include <math.h>
#include "XH_INPUTTOAXIS.h"
#include <commctrl.h>


XHUDPSocket::XHUDPSocket(void)
{
	m_nPort = 0;
	m_nTime = 20;
	m_parent = (HWND)0;
}

XHUDPSocket::~XHUDPSocket(void)
{
}

XHUDPSocket::XHUDPSocket(HWND parent)
{
	m_parent = parent;
}

bool XHUDPSocket::Create(UINT nPort,UINT nTime)
{
	m_nPort = nPort;
	m_nTime = nTime;
	m_sock = socket(AF_INET,SOCK_DGRAM,0);
	if (m_sock == SOCKET_ERROR)
	{
		OutputDebugString("socket error");
		return false;
	}

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(m_nPort);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	int len = sizeof(SOCKADDR_IN);
	if (bind(m_sock,(sockaddr *)&addr,len) == SOCKET_ERROR)
	{
		OutputDebugString("bind error");
		return false;
	}
	/*unsigned long ul = 1;
	if (ioctlsocket(m_sock,FIONBIO,&ul)== SOCKET_ERROR)
	{
		OutputDebugString("ioctlsocket error");
		return false;
	}*/
	HANDLE hThread;
	DWORD ThreadId;
	hThread = CreateThread(NULL,0,XHUDPSocket::Receive,(LPVOID)this,0,&ThreadId);

}

bool XHUDPSocket::Send(char *szRemoteIp,int m_nRemotePort,char *szbuf)
{
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(m_nRemotePort);
	addr.sin_addr.S_un.S_addr = inet_addr(szRemoteIp);
	int len = sizeof(SOCKADDR_IN);
	//struct fd_set fds;
	//struct timeval timeout={m_nTime,0}; //selectµÈ´ý3Ãë£¬3ÃëÂÖÑ¯£¬Òª·Ç×èÈû¾ÍÖÃ0
	//FD_ZERO(&fds);
	//FD_SET(sock,&fds);
	//int rt = select(m_sock+1,NULL,&fds,NULL,&timeout);
	//if (rt == -1 || rt == 0)
	//{
	//	int nError = GetLastError();
	//	TCHAR szError[10];
	//	_itow(nError,szError,10);
	//	TCHAR strError[256]={0};
	//	wcscpy(strError,TEXT("socket select error "));
	//	wcscat(strError,szError);
	//	wcscat(strError,TEXT("\n"));
	//	OutputDebugString(strError);
	//	
	//	return false;
	//}
	if (SOCKET_ERROR == sendto(m_sock,szbuf,strlen(szbuf),0,(sockaddr *)&addr,len))
	{
		int nError = GetLastError();
		char szError[10];
		itoa(nError,szError,10);
		TCHAR strError[256]={0};
		strcpy(strError,"socket send error ");
		strcat(strError,szError);
		strcat(strError,"\n");
		OutputDebugString(strError);
		
		return false;
	}
	

}
SOCKET XHUDPSocket::GetSocket()
{
	return m_sock;
}

bool XHUDPSocket::AnalysisData_exa()
{
	OutExtra *extra = (OutExtra *)m_szRecvBuf;
	axis1 = extra->fl_sus*0.3;//suspension travel front left;
	axis2 =	extra->fr_sus*0.3;//suspension travel front right;
	axis3 = extra->rl_sus*0.3;//suspension travel rear left;
	axis4 = extra->rr_sus*0.3;//suspension travel rear right;
	AvaCrest(axis1,axis2,axis3,axis4);
	
	if (axis1 >= MAX_AXIS_LONG/2-2)
	{
		axis1 = MAX_AXIS_LONG/2-2;
	}
	if (axis1 <= -MAX_AXIS_LONG/2+2)
	{
		axis1 = -MAX_AXIS_LONG/2+2;
	}
	if (axis2 >= MAX_AXIS_LONG/2-2)
	{
		axis2 = MAX_AXIS_LONG/2-2;
	}
	if (axis2 <= -MAX_AXIS_LONG/2+2)
	{
		axis2 = -MAX_AXIS_LONG/2+2;
	}
	if (axis3 >= MAX_AXIS_LONG/2-2)
	{
		axis3 = MAX_AXIS_LONG/2-2;
	}
	if (axis3 <= -MAX_AXIS_LONG/2+2)
	{
		axis3 = -MAX_AXIS_LONG/2+2;
	}
	if (axis4 >= MAX_AXIS_LONG/2-2)
	{
		axis4 = MAX_AXIS_LONG/2-2;
	}
	if (axis4 <= -MAX_AXIS_LONG/2+2)
	{
		axis4 = -MAX_AXIS_LONG/2+2;
	}
	
	axis1 += extra->fl_sh * 0.003;
	axis2 += extra->fr_sh * 0.003;
	axis3 += extra->rl_sh * 0.003;
	axis4 += extra->rr_sh * 0.003;

	//SendMessageW(GetDlgItem(m_parent,IDS_AXIS1),TBM_SETPOS, (WPARAM)1,(LPARAM)(int)(axis1*65535/MAX_AXIS_LONG));
	//SendMessageW(GetDlgItem(m_parent,IDS_AXIS2),TBM_SETPOS, (WPARAM)1,(LPARAM)(int)(axis2*65535/MAX_AXIS_LONG));
	//SendMessageW(GetDlgItem(m_parent,IDS_AXIS3),TBM_SETPOS, (WPARAM)1,(LPARAM)(int)(axis3*65535/MAX_AXIS_LONG));
	//SendMessageW(GetDlgItem(m_parent,IDS_AXIS4),TBM_SETPOS, (WPARAM)1,(LPARAM)(int)(axis4*65535/MAX_AXIS_LONG));
	return 1;

}




bool XHUDPSocket::AnalysisData_sim()
{
	OutSimPack *sim = (OutSimPack *)m_szRecvBuf;
	//sim->Heading += 0.232;
	//sim->Pitch -= 0.002;
	//sim->Roll -=1.57;
	float a,b,heading;	
	heading = sim->Heading;
	a = sin(-heading);
	b = cos(-heading);
	float x = sim->Accel.x;
	float y = sim->Accel.y;
	sim->Accel.x = b*x - a*y; 
	sim->Accel.y = a*x + b*y;

	
	XH3DPoint xhQuantitative[4] = {-270,550,0,270,550,0,-270,-550,0,270,-550,0};
	XH3DPoint xhVariable[4]     = {-270,550,400,270,550,400,-270,-550,400,270,-550,400};

	for (int i = 0;i < 4;i++)
	{
		initCoor.XHQuantitative[i]=xhQuantitative[i];
		initCoor.XHVariable[i] = xhVariable[i];
	}

	
	input.a = 0;//sim->Roll;
	input.b = 0;//sim->Pitch;
	input.c = 0;
	input.x = sim->Accel.x;
	input.y = sim->Accel.y;
	input.z = sim->Accel.z;


	SetEvent(hEvent);
	ShowLog("setevevt");
	
	return 1;
}


DWORD WINAPI  XHUDPSocket::Receive(LPVOID lp)
{
	XHUDPSocket *XhSock = (XHUDPSocket *)lp;
	SOCKADDR_IN addr;
	int len = sizeof(SOCKADDR_IN);
	axis_Struct = new Axis_Struct[AVE_N];
	inputAva = new InputVector[AVE_N];
	memset(axis_Struct,0,AVE_N * sizeof(Axis_Struct));
	memset(inputAva,0,AVE_N * sizeof(InputVector));
	while (1)
	{
		memset(XhSock->m_szRecvBuf,0,MAX_BUF);
		recvfrom(XhSock->m_sock,XhSock->m_szRecvBuf,MAX_BUF,0,(struct sockaddr *)&addr,&len);
		XhSock->AnalysisData_sim();
	}
	delete [] axis_Struct;
}

bool XHUDPSocket::Send(char *szbuf)
{
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(m_nRemotePort);
	addr.sin_addr.S_un.S_addr = inet_addr(m_szRemoteIp);
	int len = sizeof(SOCKADDR_IN);
	//struct fd_set fds;
	//struct timeval timeout={m_nTime,0}; //selectµÈ´ý3Ãë£¬3ÃëÂÖÑ¯£¬Òª·Ç×èÈû¾ÍÖÃ0
	//FD_ZERO(&fds);
	//FD_SET(sock,&fds);
	//int rt = select(m_sock+1,NULL,&fds,NULL,&timeout);
	//if (rt == -1 || rt == 0)
	//{
	//	int nError = GetLastError();
	//	TCHAR szError[10];
	//	_itow(nError,szError,10);
	//	TCHAR strError[256]={0};
	//	wcscpy(strError,TEXT("socket select error "));
	//	wcscat(strError,szError);
	//	wcscat(strError,TEXT("\n"));
	//	OutputDebugString(strError);
	//	
	//	return false;
	//}
	if (SOCKET_ERROR == sendto(m_sock,szbuf,strlen(szbuf),0,(sockaddr *)&addr,len))
	{
		int nError = GetLastError();
		char szError[10];
		itoa(nError,szError,10);
		TCHAR strError[256]={0};
		strcpy(strError,"socket send error ");
		strcat(strError,szError);
		strcat(strError,"\n");
		OutputDebugString(strError);

		return false;
	}
}

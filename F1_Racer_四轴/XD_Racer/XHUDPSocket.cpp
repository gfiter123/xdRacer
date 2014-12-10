#include "XHUDPSocket.h"
#include "XD_DOF.h"
#include "XD_Racer.h"
#include "XD_OutSim.h"
#include <stdio.h>
#include <math.h>

#include <commctrl.h>
int   AVE_N;
int   M_Q;
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

struct Axis_Struct
{
	float axis1;
	float axis2;
	float axis3;
	float axis4;
};
Axis_Struct *axis_Struct;
InitCoordinates initCoor;
void AvaCrest(float &axis1,float &axis2,float &axis3,float & axis4)
{
	
	
	float ax1 = axis1;
	float ax2 = axis2;
	float ax3 = axis3;
	float ax4 = axis4;

	for(int i = 0;i < AVE_N -1 ;i++)
	{
		axis_Struct[i] = axis_Struct[i+1];
		
	}
	axis_Struct[AVE_N -1].axis1 = axis1;
	axis_Struct[AVE_N -1].axis2 = axis2;
	axis_Struct[AVE_N -1].axis3 = axis3;
	axis_Struct[AVE_N -1].axis4 = axis4;

	
	for (int i = 0; i< AVE_N - 1;i++)
	{
		ax1 += axis_Struct[i].axis1;
		ax2 += axis_Struct[i].axis2;
		ax3 += axis_Struct[i].axis3;
		ax4 += axis_Struct[i].axis4;
	}

	axis1 = ax1 / AVE_N;
	axis2 = ax2 / AVE_N;
	axis3 = ax3 / AVE_N;
	axis4 = ax4 / AVE_N;
}

InputVector * inputAva;
void AvaOula(InputVector &input)
{
	

	InputVector temp = {0};
	for(int i = 0;i < AVE_N -1 ;i++)
	{
		inputAva[i] = inputAva[i+1];

	}
	inputAva[AVE_N -1] =  input;



	for (int i = 0; i< AVE_N - 1;i++)
	{
		temp = temp + inputAva[i];
		
	}

	input = temp / AVE_N;
	
}
void KaermanCrest(float &axis1,float &axis2,float &axis3,float & axis4)
{
	float m_xMid[4];
	float m_pMid[4];
	static float m_xLast[4] = {0};
	static float m_pLast[4] = {0};
	float m_kg[4];
	float m_pNow[4];
	float m_xNow[4];
	float ax[4];
	ax[0] = axis1;
	ax[1] = axis2;
	ax[2] = axis3;
	ax[3] = axis4;
	for (int i=0;i < 4;i++)
	{
		m_xMid[i] = m_xLast[i];
		m_pMid[i] = sqrt(m_pLast[i]* m_pLast[i] + M_Q * M_Q);
		m_kg[i] = pow(m_pMid[i],2)/(pow(m_pMid[i],2) + M_Q * M_Q);
		m_xNow[i] = m_xMid[i] + m_kg[i]*(ax[i] - m_xMid[i]);
		m_pNow[i] = sqrt((1 - m_kg[i]) * m_pMid[i] * m_pMid[i]);
		m_xLast[i] = m_xNow[i];
		m_pLast[i] = m_pNow[i];
	}
	axis1 = m_xNow[0];
	axis2 = m_xNow[1];
	axis3 = m_xNow[2];
	axis4 = m_xNow[3];
}

InputVector sqrt(InputVector x)
{
	InputVector temp;
	temp.a = sqrt(x.a);
	temp.b = sqrt(x.b);
	temp.c = sqrt(x.c);
	temp.x = sqrt(x.x);
	temp.y = sqrt(x.y);
	temp.z = sqrt(x.z);
	return temp;
}
InputVector inPow(InputVector x,float n)
{
	InputVector temp;
	temp.a = (float)pow(x.a,n);
	temp.b = (float)pow(x.b,n);
	temp.c = (float)pow(x.c,n);
	temp.x = (float)pow(x.x,n);
	temp.y = (float)pow(x.y,n);
	temp.z = (float)pow(x.z,n);
	return temp;
}
void KaermanOula(InputVector &input)
{
	InputVector m_xMid;
	InputVector m_pMid;
	static InputVector m_xLast = {0};
	static InputVector m_pLast = {0};
	InputVector m_kg;
	InputVector m_pNow;
	InputVector m_xNow;
	InputVector ax = input;
	
	m_xMid = m_xLast;
	m_pMid = sqrt(m_pLast* m_pLast + M_Q * M_Q);
	m_kg = inPow(m_pMid,2.0)/(inPow(m_pMid,2.0) + M_Q * M_Q);
	m_xNow = m_xMid + m_kg*(ax - m_xMid);
	m_pNow.a = sqrt((1 - m_kg.a) * m_pMid.a * m_pMid.a);
	m_pNow.b = sqrt((1 - m_kg.b) * m_pMid.b * m_pMid.b);
	m_pNow.c = sqrt((1 - m_kg.c) * m_pMid.c * m_pMid.c);
	m_pNow.x = sqrt((1 - m_kg.x) * m_pMid.x * m_pMid.x);
	m_pNow.y = sqrt((1 - m_kg.y) * m_pMid.y * m_pMid.y);
	m_pNow.z = sqrt((1 - m_kg.z) * m_pMid.z * m_pMid.z);

	m_xLast = m_xNow;
	m_pLast = m_pNow;

	input = m_xNow;


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

static void TakeMaxAxis(float& axis1,float& axis2,float& axis3,float& axis4)
{
	static float oldAx1 = 0.0;
	static float oldAx2 = 0.0;
	static float oldAx3 = 0.0;
	static float oldAx4 = 0.0;
	float fMaxAxisLong = MAX_AXIS_LONG / 2;
	float ax1 = axis1,ax2 = axis2,ax3 = axis3,ax4 = axis4;
	if (axis1 >= fMaxAxisLong)
	{
		if (oldAx1 > axis1)
		{
			ax1 = fMaxAxisLong - oldAx1 + axis1;
		}
		else
			ax1 = fMaxAxisLong;
	}
	if (axis1 <= -fMaxAxisLong)
	{
		if (oldAx1 < axis1)
		{
			ax1 = -fMaxAxisLong +  axis1 - oldAx1;
		}
		else
			ax1 = -fMaxAxisLong;
	}

	if (axis2 >= fMaxAxisLong)
	{
		if (oldAx2 > axis2)
		{
			ax2 = fMaxAxisLong - oldAx2 + axis2;
		}
		else
			ax2 = fMaxAxisLong;
	}
	if (axis2 <= -fMaxAxisLong)
	{
		if (oldAx2 < axis2)
		{
			ax2 = -fMaxAxisLong +  axis2 - oldAx2;
		}
		else
			ax2 = -fMaxAxisLong;
	}

	if (axis3 >= fMaxAxisLong)
	{
		if (oldAx3 > axis3)
		{
			ax3 = fMaxAxisLong - oldAx3 + axis3;
		}
		else
			ax3 = fMaxAxisLong;
	}
	if (axis3 <= -fMaxAxisLong)
	{
		if (oldAx3 < axis3)
		{
			ax3 = -fMaxAxisLong +  axis3 - oldAx3;
		}
		else
			ax3 = -fMaxAxisLong;
	}

	if (axis4 >= fMaxAxisLong)
	{
		if (oldAx4 > axis4)
		{
			ax4 = fMaxAxisLong - oldAx4 + axis4;
		}
		else
			ax4 = fMaxAxisLong;
	}
	if (axis4 <= -fMaxAxisLong)
	{
		if (oldAx4 < axis4)
		{
			ax4 = -fMaxAxisLong +  axis4 - oldAx4;
		}
		else
			ax4 = -fMaxAxisLong;
	}


	oldAx1 = axis1;
	oldAx2 = axis2;
	oldAx3 = axis3;
	oldAx4 = axis4;
	axis1 = ax1;
	axis2 = ax2;
	axis3 = ax3;
	axis4 = ax4;

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

	InputVector input;
	input.a = 0;//sim->Roll;
	input.b = 0;//sim->Pitch;
	input.c = 0;
	input.x = sim->Accel.x;
	input.y = sim->Accel.y;
	input.z = sim->Accel.z;


	AvaOula(input);
	KaermanOula(input);
	OutputVector output;
	GetXD_DOFData(&input,&output);

	axis1 = output.l1;
	axis2 = output.l2;
	axis3 = output.l3;
	axis4 = output.l4;
	
	
	//AvaCrest(axis1,axis2,axis3,axis4);
	//KaermanCrest(axis1,axis2,axis3,axis4);
	TakeMaxAxis(axis1,axis2,axis3,axis4);
	
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

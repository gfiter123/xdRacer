#include "stdafx.h"
#include "XHMotionUDPsock.h"


#include <stdio.h>
#include <math.h>

#include <commctrl.h>
int commandSign;
XHMotionUDPsock::XHMotionUDPsock(void)
{
	m_nPort = 0;
	m_nTime = 20;
	m_parent = (HWND)0;
}

XHMotionUDPsock::~XHMotionUDPsock(void)
{
}

XHMotionUDPsock::XHMotionUDPsock(HWND parent)
{
	m_parent = parent;
}

bool XHMotionUDPsock::Create(UINT nPort,UINT nTime)
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
	hThread = CreateThread(NULL,0,&XHMotionUDPsock::Receive,(LPVOID)this,0,&ThreadId);

}

bool XHMotionUDPsock::Send(char *szRemoteIp,int nRemotePort,char *szbuf,int nLen)
{
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nRemotePort);
	addr.sin_addr.S_un.S_addr = inet_addr(szRemoteIp);
	int len = sizeof(SOCKADDR_IN);
	if (SOCKET_ERROR == sendto(m_sock,szbuf,nLen,0,(sockaddr *)&addr,len))
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
	return true;
	

}
SOCKET XHMotionUDPsock::GetSocket()
{
	return m_sock;
}



void pauseCommand(char *Buf,int len,XHMotionUDPsock *xhSock)
{
	
}


DWORD WINAPI  XHMotionUDPsock::Receive(LPVOID lp)
{
	XHMotionUDPsock *XhSock = (XHMotionUDPsock *)lp;
	SOCKADDR_IN addr;
	int l = sizeof(SOCKADDR_IN);
	while (1)
	{
		memset(XhSock->m_szRecvBuf,0,MAX_BUF);
		int len = recvfrom(XhSock->m_sock,XhSock->m_szRecvBuf,MAX_BUF,0,(struct sockaddr *)&addr,&l);
		SendMessage(XhSock->m_parent,WM_UDPMSG,(WPARAM)len,(LPARAM)XhSock);
	}
}

bool XHMotionUDPsock::SetRemoteInfo(int index,char *szRemoteIp,UINT port)
{
	
	if (index == 1)
	{
		strcpy(m_szRemoteIp1,szRemoteIp);
		m_nRemotePort1 = port;
	}
	else
	{
		strcpy(m_szRemoteIp2,szRemoteIp);
		m_nRemotePort2 = port;
	}
	return true;
}

bool XHMotionUDPsock::Send1(char *szbuf,int nlen)
{
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(m_nRemotePort1);
	addr.sin_addr.S_un.S_addr = inet_addr(m_szRemoteIp1);
	int len = sizeof(SOCKADDR_IN);
	Sleep(20);
	if (SOCKET_ERROR == sendto(m_sock,szbuf,nlen,0,(sockaddr *)&addr,len))
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
	return true;
}

bool XHMotionUDPsock::Send2(char *szbuf,int nlen)
{
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(m_nRemotePort2);
	addr.sin_addr.S_un.S_addr = inet_addr(m_szRemoteIp2);
	int len = sizeof(SOCKADDR_IN);

	if (SOCKET_ERROR == sendto(m_sock,szbuf,nlen,0,(sockaddr *)&addr,len))
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
	return true;
}

bool XHMotionUDPsock::SendAll(char *szbuf,int nlen)
{
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(m_nRemotePort1);
	addr.sin_addr.S_un.S_addr = inet_addr(m_szRemoteIp1);
	int len = sizeof(SOCKADDR_IN);

	if (SOCKET_ERROR == sendto(m_sock,szbuf,nlen,0,(sockaddr *)&addr,len))
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
	Sleep(1000);
	memset(&addr,0,sizeof(SOCKADDR_IN));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(m_nRemotePort2);
	addr.sin_addr.S_un.S_addr = inet_addr(m_szRemoteIp2);
	len = sizeof(SOCKADDR_IN);

	if (SOCKET_ERROR == sendto(m_sock,szbuf,nlen,0,(sockaddr *)&addr,len))
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
	return true;
}
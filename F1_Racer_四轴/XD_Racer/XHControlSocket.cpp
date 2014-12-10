#include <Windows.h>
#include "XHControlSocket.h"

#include "XD_Racer.h"

#include <stdio.h>
#include <math.h>

#include <commctrl.h>
XHControlSocket::XHControlSocket(void)
{
	m_nPort = 0;
	m_nTime = 20;
	m_parent = (HWND)0;
}

XHControlSocket::~XHControlSocket(void)
{
}

XHControlSocket::XHControlSocket(HWND parent)
{
	m_parent = parent;
}

bool XHControlSocket::Create(UINT nPort,UINT nTime)
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
	hThread = CreateThread(NULL,0,&XHControlSocket::Receive,(LPVOID)this,0,&ThreadId);

}

bool XHControlSocket::Send(char *szRemoteIp,int nRemotePort,char *szbuf,int nLen)
{
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nRemotePort);
	addr.sin_addr.S_un.S_addr = inet_addr(szRemoteIp);
	int len = sizeof(SOCKADDR_IN);
	//struct fd_set fds;
	//struct timeval timeout={m_nTime,0}; //select等待3秒，3秒轮询，要非阻塞就置0
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
SOCKET XHControlSocket::GetSocket()
{
	return m_sock;
}





void pauseCommand(char *Buf,int len,XHControlSocket *xhSock)
{
	const char *d = "#";
	char *front = NULL;
	char *back = NULL;
	char *pNext = NULL;
	if (!strcmp(Buf,""))
	{
		return;
	}
	front = strtok_s(Buf,d,&pNext);
	back = strtok_s(NULL,d,&pNext);
	if (!strcmp(front,"CONNECTED"))
	{
		g_bConnect = true;
		SetWindowText(GetDlgItem(xhSock->m_parent,IDC_BTN_CONNECT),"连接成功");
	}
	
	


}


DWORD WINAPI  XHControlSocket::Receive(LPVOID lp)
{
	XHControlSocket *XhSock = (XHControlSocket *)lp;
	SOCKADDR_IN addr;
	int l = sizeof(SOCKADDR_IN);
	while (1)
	{
		memset(XhSock->m_szRecvBuf,0,MAX_BUF);
		int len = recvfrom(XhSock->m_sock,XhSock->m_szRecvBuf,MAX_BUF,0,(struct sockaddr *)&addr,&l);
		pauseCommand(XhSock->m_szRecvBuf,len,XhSock);
	}
}

bool XHControlSocket::SetRemoteInfo(char *szRemoteIp,UINT port)
{
	strcpy(m_szRemoteIp,szRemoteIp);
	m_nRemotePort = port;
	return true;
}

bool XHControlSocket::Send(char *sendBuf)
{
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(m_nRemotePort);
	addr.sin_addr.S_un.S_addr = inet_addr(m_szRemoteIp);
	int len = sizeof(SOCKADDR_IN);
	
	if (SOCKET_ERROR == sendto(m_sock,sendBuf,strlen(sendBuf),0,(sockaddr *)&addr,len))
	{
		g_bConnect = false;
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
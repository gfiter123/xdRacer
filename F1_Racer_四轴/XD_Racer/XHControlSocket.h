#pragma once

#include "F1RacerMotionProtocal.h"
#define MAX_BUF    256 
class XHControlSocket
{
private:
	UINT m_nPort;
	UINT m_nTime;

public:
	SOCKET m_sock;
	HWND m_parent;
	char m_szRecvBuf[MAX_BUF];
	char m_szRemoteIp[20];
	UINT m_nRemotePort;
	XHControlSocket(HWND m_parent);
	XHControlSocket(void);
	~XHControlSocket(void);
	SOCKET GetSocket();
	bool SetRemoteInfo(char *szRemoteIp,UINT port);
	bool Create(UINT nPort,UINT nTime = 20);
	bool Send(char *szRemoteIp,int m_nRemotePort,char *szbuf,int len);
	bool Send(char *sendBuf);
	static DWORD WINAPI Receive(LPVOID lp);
};

#pragma once

#define MAX_BUF    256 

extern int   AVE_N;
extern int   M_Q;
typedef unsigned int UINT;
#include <windows.h>
class XHUDPSocket
{
private:
	
	UINT m_nPort;
	UINT m_nTime;
	HWND m_parent;
	
public:
	SOCKET m_sock;
	char m_szRecvBuf[MAX_BUF];
	char m_szRemoteIp[20];
	UINT m_nRemotePort;
	
	XHUDPSocket(void);
	XHUDPSocket(HWND m_parent);
	SOCKET GetSocket();
	bool Create(UINT nPort,UINT nTime = 20);
	bool Send(char *szRemoteIp,int m_nRemotePort,char *szbuf);
	bool Send(char *szbuf);
	static DWORD WINAPI Receive(LPVOID lp);
	bool AnalysisData_sim();
	bool AnalysisData_exa();
	~XHUDPSocket(void);
};

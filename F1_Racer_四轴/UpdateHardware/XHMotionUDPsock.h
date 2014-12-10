#pragma once
#define WM_UDPMSG WM_USER+8
#define MAX_BUF    256 
class XHMotionUDPsock
{
private:
	UINT m_nPort;
	UINT m_nTime;
	
public:
	SOCKET m_sock;
	HWND m_parent;
	char m_szRecvBuf[MAX_BUF];
	char m_szRemoteIp1[20];
	UINT m_nRemotePort1;
	char m_szRemoteIp2[20];
	UINT m_nRemotePort2;
	XHMotionUDPsock(HWND m_parent);
	XHMotionUDPsock(void);
	~XHMotionUDPsock(void);
	SOCKET GetSocket();
	bool SetRemoteInfo(int index,char *szRemoteIp,UINT port);
	bool Create(UINT nPort,UINT nTime = 20);
	bool Send(char *szRemoteIp,int m_nRemotePort,char *szbuf,int len);
	bool Send1(char *szbuf,int len);
	bool Send2(char *szbuf,int len);
	bool SendAll(char *szbuf,int len);
	static DWORD WINAPI Receive(LPVOID lp);
};

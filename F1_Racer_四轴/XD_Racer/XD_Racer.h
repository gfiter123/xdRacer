#pragma once
#include "resource.h"

#include<vector>
#define dim(x) (sizeof(x)/sizeof(x[0]))



struct DecodeUnit{
	UINT message;
	LONG (*fun)(HWND,UINT,WPARAM,LPARAM);
};


extern HANDLE hEvent;
struct MMData
{
public:
	int index;
	int flag;
	int x1, y1;
	int x2, y2;
	int acce;

};

struct KKData
{
public:
	int nIndex;
	int nKey_value;
	int nTime;
};


#define MAX_AXIS_LONG  45

extern float axis1;
extern float axis2;
extern float axis3;
extern float axis4;

extern std::vector<MMData> gMMDataList;
extern std::vector<KKData> gKKDataList;
extern bool g_bDownlod;
extern bool g_bConnect;
INT_PTR CALLBACK DlgPorc(HWND,UINT,WPARAM,LPARAM);

LONG OnInitDialog(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG DoCommand(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnClose(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnVscroll(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);

bool ReadConfig();
VOID CALLBACK OnTimerProc(HWND hDlg,UINT message,UINT nIDEvent,DWORD lParam);
LONG OnBtnRunGameClick(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnBtnSetZeroClick(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnBtnStopClick(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);


void ShowLog(char *buf);
LONG OnBtnConnectClick(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);

DWORD WINAPI PositionThreadProc(LPVOID lp);
DWORD WINAPI TimerThreadProc(LPVOID lp);

void ConnectControlHost(HWND hDlg);
void SendControlReady(HWND hDlg);
void openKeyData();
void SetKeyDown();
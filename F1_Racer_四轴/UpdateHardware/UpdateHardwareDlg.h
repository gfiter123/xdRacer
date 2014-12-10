// UpdateHardwareDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include <fstream>
using namespace std;
#include "XHMotionUDPsock.h"
#define  MAX_COMMAND_LONG  2048

// CUpdateHardwareDlg 对话框
class CUpdateHardwareDlg : public CDialog
{
// 构造
public:
	CUpdateHardwareDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_UPDATEHARDWARE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStart();
	void SetIpCtrl(CString ip);
	CIPAddressCtrl m_IpCtrl;
	XHMotionUDPsock *m_pMotionSock;
	char command[2048];
	int commandLen;
	int m_nSendSize;
	CString m_strFilePath;
	void Requst();
	void CheckOne();
	void CheckTwo();
	void DownLoadData();
	void Finish();
	void CreateSocket();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnPauseCommand(WPARAM,LPARAM);
	CString m_strLog;
	ifstream infile;
};

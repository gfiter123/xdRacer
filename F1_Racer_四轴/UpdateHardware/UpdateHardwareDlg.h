// UpdateHardwareDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include <fstream>
using namespace std;
#include "XHMotionUDPsock.h"
#define  MAX_COMMAND_LONG  2048

// CUpdateHardwareDlg �Ի���
class CUpdateHardwareDlg : public CDialog
{
// ����
public:
	CUpdateHardwareDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_UPDATEHARDWARE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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

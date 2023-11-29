
// sipClientDlg.h : 头文件
//

#pragma once

#include "eXosip2/eX_setup.h"
#include "eXosip2/eXosip.h"
#include <atomic>
#include <thread>

// CsipClientDlg 对话框
class CsipClientDlg : public CDialogEx
{
// 构造
public:
	CsipClientDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CsipClientDlg();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SIPCLIENT_DIALOG };
#endif

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
	afx_msg void OnBnClickedBtnRegister();
	afx_msg void OnBnClickedBtnInit();
	void showMsg(char* _s);
private:
	eXosip_t * sip_context;
	void closeListen();
	std::atomic<bool> _keepRunning;
	std::thread _receiverThread;
	CEdit m_edit_show_msg;
	
	
public:
	afx_msg void OnBnClickedBtnDeInit();
	CIPAddressCtrl m_svr_ip;
	afx_msg void OnBnClickedBtnInvite();
};

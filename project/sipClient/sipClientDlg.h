
// sipClientDlg.h : ͷ�ļ�
//

#pragma once

#include "eXosip2/eX_setup.h"
#include "eXosip2/eXosip.h"
#include <atomic>
#include <thread>

// CsipClientDlg �Ի���
class CsipClientDlg : public CDialogEx
{
// ����
public:
	CsipClientDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CsipClientDlg();
// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SIPCLIENT_DIALOG };
#endif

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


// sipClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "sipClient.h"
#include "sipClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CsipClientDlg �Ի���



CsipClientDlg::CsipClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SIPCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CsipClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SHOW_MSG, m_edit_show_msg);
}

BEGIN_MESSAGE_MAP(CsipClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_REGISTER, &CsipClientDlg::OnBnClickedBtnRegister)
	ON_BN_CLICKED(IDC_BTN_INIT, &CsipClientDlg::OnBnClickedBtnInit)
	ON_BN_CLICKED(IDC_BTN_DE_INIT, &CsipClientDlg::OnBnClickedBtnDeInit)
END_MESSAGE_MAP()


// CsipClientDlg ��Ϣ�������

BOOL CsipClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	_keepRunning = false;

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CsipClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CsipClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CsipClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CsipClientDlg::showMsg(char* _s)
{
	CString cstr_temp;
	m_edit_show_msg.GetWindowTextA(cstr_temp); //��ȡ�༭������
	CString cstr_temp02 = cstr_temp + "\r\n" + _s;
	m_edit_show_msg.SetWindowText(cstr_temp02); 	//��������������
}


void CsipClientDlg::OnBnClickedBtnRegister()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int ret = 0;
	int local_port = 5062;
	char local_ip[32] = { 0 };
	char from_uri[128] = {0};
	char proxy_uri[128] = { 0 };
	char contact[128] = { 0 };
	char* deviceId = "piratemike";
	char* server_sip_id = "piratemikeproxy";
	ret = eXosip_guess_localip(sip_context, AF_INET, local_ip, 128);
	if (ret != OSIP_SUCCESS) {
		m_edit_show_msg.SetWindowTextA("eXosip_guess_localip failed");
		return;
	}

	//������<>��������ȻeXosip_register_build_initial_register ��һֱ���� -5���﷨����
	sprintf_s(from_uri, "sip:%s@%s:%d", deviceId, local_ip, local_port);
	sprintf_s(contact, "sip:%s@%s:%d", deviceId, local_ip, local_port);
	//sprintf(contact,"<sip:%s@%s:%d>",deviceId,server_ip,server_port);
	sprintf_s(proxy_uri, "sip:%s@%s:5060", server_sip_id, local_ip);

	eXosip_clear_authentication_info(sip_context);


	osip_message_t * register_message = NULL;
	//struct eXosip_t *excontext, const char *from, const char *proxy, const char *contact, int expires, osip_message_t ** reg
	int register_id = eXosip_register_build_initial_register(sip_context, from_uri, proxy_uri, contact, 3600, &register_message);
	if (register_message == NULL) {
		AfxMessageBox("eXosip_register_build_initial_register failed");
		return;
	}
	//��ǰ��������֤��Ϣ������ϢΪ401������eXosip_automatic_action()�Զ�����
	//eXosip_add_authentication_info(sip_context,"022000000110000", "022000000110000", "12345678", "MD5", NULL);
	eXosip_lock(sip_context);
	eXosip_register_send_register(sip_context, register_id, register_message);
	eXosip_unlock(sip_context);
}

void CsipClientDlg::OnBnClickedBtnInit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int ret = 0;
	char local_ip[32] = { 0 };
	int local_port = 0;
	CString strTmp;
	sip_context = eXosip_malloc();
	if (sip_context == NULL) {
		m_edit_show_msg.SetWindowTextA("eXosip_malloc failed\r\n");
		return;
	}
	ret = eXosip_init(sip_context);
	if (ret != OSIP_SUCCESS) {
		m_edit_show_msg.SetWindowTextA("sip init failed");
		return;
	}
	ret = eXosip_guess_localip(sip_context, AF_INET, local_ip, 32);
	if (ret != OSIP_SUCCESS) {
		m_edit_show_msg.SetWindowTextA("eXosip_guess_localip failed");
		return;
	}
	local_port = eXosip_find_free_port(sip_context, 5060, IPPROTO_UDP);
	if (local_port <= 0) {
		m_edit_show_msg.SetWindowTextA("eXosip_find_free_port failed");
		return;
	}
	strTmp.Format("ip: %s  port: %d", local_ip, local_port);
	showMsg(strTmp.GetBuffer());
	ret = eXosip_listen_addr(sip_context, IPPROTO_UDP, local_ip, local_port+2, AF_INET, 0);
	if (ret != OSIP_SUCCESS) {
		m_edit_show_msg.SetWindowTextA("eXosip_listen_addr failed");
		return;
	}

	_keepRunning = true;
	_receiverThread = std::thread([=]()//��ʽ���� ��ֵ����ʽ
	{
		eXosip_event_t* evt = NULL;
		while (_keepRunning)
		{
			evt = eXosip_event_wait(this->sip_context, 0, 50);
			eXosip_lock(this->sip_context);
			eXosip_automatic_action(this->sip_context);
			eXosip_unlock(this->sip_context);
			if (evt == NULL) {
				continue;
			}
			switch (evt->type)
			{
				case EXOSIP_REGISTRATION_SUCCESS: // ע��ɹ�
					this->showMsg("EXOSIP_REGISTRATION_SUCCESS");
					break;
				default:
					break;
			}
		}
	});
}

void CsipClientDlg::closeListen()
{
	_keepRunning = false;
	_receiverThread.join();
	if (sip_context != NULL) {
		eXosip_quit(sip_context);
		sip_context = NULL;
	}
}


void CsipClientDlg::OnBnClickedBtnDeInit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	closeListen();
}

CsipClientDlg::~CsipClientDlg() {
	closeListen();
}
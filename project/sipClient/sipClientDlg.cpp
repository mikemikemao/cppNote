
// sipClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "sipClient.h"
#include "sipClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static constexpr int SIP_CLIENT_PORT = 5065;
static constexpr int SIP_SERVER_PORT = 5060;

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
	DDX_Control(pDX, IDC_IPADDRESS1, m_svr_ip);
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
	m_svr_ip.SetWindowTextA("10.6.120.2");
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
	in_addr addr;
	memset(&addr, 0x00, sizeof(in_addr));
	DWORD ipAddr = 0;
	char* svrIpStr = NULL;
	char local_ip[128] = { 0 };
	char from_uri[128] = {0};
	char proxy_uri[128] = { 0 };
	char* deviceId = "piratemike";
	CString strTmp;
	m_svr_ip.GetAddress(ipAddr);
	addr.s_addr = htonl(ipAddr);
	svrIpStr = inet_ntoa(addr);

	ret = eXosip_guess_localip(sip_context, AF_INET, local_ip, 128);
	if (ret != OSIP_SUCCESS) {
		m_edit_show_msg.SetWindowTextA("eXosip_guess_localip failed");
		return;
	}
	strTmp.Format("local_ip : %s", local_ip);
	showMsg(strTmp.GetBuffer());
	strTmp.Format("svr_ip   : %s", svrIpStr);
	showMsg(strTmp.GetBuffer());
	//������<>��������ȻeXosip_register_build_initial_register ��һֱ���� -5���﷨����
	sprintf_s(from_uri, "sip:%s@%s:%d", deviceId, local_ip, SIP_CLIENT_PORT);
	sprintf_s(proxy_uri, "sip:%s:%d", svrIpStr, SIP_SERVER_PORT);
	eXosip_clear_authentication_info(sip_context);
	osip_message_t * register_message = NULL;
	//struct eXosip_t *excontext, const char *from, const char *proxy, const char *contact, int expires, osip_message_t ** reg
	int register_id = eXosip_register_build_initial_register(sip_context, from_uri, proxy_uri, NULL, 3600, &register_message);
	if (register_message == NULL) {
		AfxMessageBox("eXosip_register_build_initial_register failed");
		return;
	}
	strTmp.Format("register_id   : %d", register_id);
	showMsg(strTmp.GetBuffer());
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

	ret = eXosip_guess_localip(sip_context, AF_INET, local_ip, 128);
	if (ret != OSIP_SUCCESS) {
		m_edit_show_msg.SetWindowTextA("eXosip_guess_localip failed");
		return;
	}

	ret = eXosip_listen_addr(sip_context, IPPROTO_UDP, local_ip, SIP_CLIENT_PORT, AF_INET, 0);
	if (ret != OSIP_SUCCESS) {
		m_edit_show_msg.SetWindowTextA("eXosip_listen_addr failed");
		return;
	}
	showMsg("init success");
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
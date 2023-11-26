
// sipClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "sipClient.h"
#include "sipClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CsipClientDlg 对话框



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


// CsipClientDlg 消息处理程序

BOOL CsipClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	_keepRunning = false;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CsipClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CsipClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CsipClientDlg::showMsg(char* _s)
{
	CString cstr_temp;
	m_edit_show_msg.GetWindowTextA(cstr_temp); //获取编辑框内容
	CString cstr_temp02 = cstr_temp + "\r\n" + _s;
	m_edit_show_msg.SetWindowText(cstr_temp02); 	//设置输入框的内容
}


void CsipClientDlg::OnBnClickedBtnRegister()
{
	// TODO: 在此添加控件通知处理程序代码
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

	//不能用<>包裹，不然eXosip_register_build_initial_register 会一直返回 -5，语法错误
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
	//提前输入了验证信息，在消息为401处，用eXosip_automatic_action()自动处理
	//eXosip_add_authentication_info(sip_context,"022000000110000", "022000000110000", "12345678", "MD5", NULL);
	eXosip_lock(sip_context);
	eXosip_register_send_register(sip_context, register_id, register_message);
	eXosip_unlock(sip_context);
}

void CsipClientDlg::OnBnClickedBtnInit()
{
	// TODO: 在此添加控件通知处理程序代码
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
	_receiverThread = std::thread([=]()//隐式捕获 以值捕获方式
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
				case EXOSIP_REGISTRATION_SUCCESS: // 注册成功
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
	// TODO: 在此添加控件通知处理程序代码
	closeListen();
}

CsipClientDlg::~CsipClientDlg() {
	closeListen();
}

// sipClientDlg.cpp : 实现文件
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
static constexpr char* FROM_NUMBER = "piratemike";
static constexpr char* TO_NUMBER = "piratemike";

#define SIP_VERSION					"V2.0.0"
#define VIS_SDP_2_INFO	("v=0\r\n"\
						"o=%s 0 0 IN IP4 %s\r\n"\
						"s=Talk session\r\n"\
						"c=IN IP4 %s\r\n"\
						"t=0 0\r\n"\
						"a=doorFloor:%d\r\n"\
						"a=responseType:%d\r\n"\
						"a=doorType:%d\r\n"\
						"a=isSpecialType:%d\r\n"\
						"a=isConfirm:%d\r\n"\
						"a=version:%s\r\n"\
						"a=callforwardseq:%d\r\n"\
						"a=meetingMaxForwards:%d\r\n"\
						"m=audio %d RTP/AVP %d %d 101\r\n"\
						"a=rtpmap:0 PCMU/8000\r\n"\
						"a=rtpmap:8 PCMA/8000\r\n"\
						"a=rtpmap:101 telephone-event/8000\r\n"\
						"a=fmtp:101 0-16\r\n"\
						"a=sendrecv\r\n"\
						"m=video %d RTP/AVP 96\r\n"\
						"a=rtpmap:96 H264/90000\r\n"\
						"a=fmtp:96 profile-level-id=42801F;packetization-mode=1\r\n"\
						"a=sendrecv\r\n")

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
	DDX_Control(pDX, IDC_IPADDRESS1, m_svr_ip);
}

BEGIN_MESSAGE_MAP(CsipClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_REGISTER, &CsipClientDlg::OnBnClickedBtnRegister)
	ON_BN_CLICKED(IDC_BTN_INIT, &CsipClientDlg::OnBnClickedBtnInit)
	ON_BN_CLICKED(IDC_BTN_DE_INIT, &CsipClientDlg::OnBnClickedBtnDeInit)
	ON_BN_CLICKED(IDC_BTN_INVITE, &CsipClientDlg::OnBnClickedBtnInvite)
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
	m_svr_ip.SetWindowTextA("10.6.120.2");
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
	in_addr addr;
	memset(&addr, 0x00, sizeof(in_addr));
	DWORD ipAddr = 0;
	char* svrIpStr = NULL;
	char local_ip[128] = { 0 };
	char from_uri[128] = {0};
	char proxy_uri[128] = { 0 };
	char* deviceId = "piratemike";
	char ifName[8] = "eth0";
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
	//不能用<>包裹，不然eXosip_register_build_initial_register 会一直返回 -5，语法错误
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

	//增加content内容
	ret = osip_message_set_body(register_message, ifName, strlen(ifName));
	if (ret != OSIP_SUCCESS) {
		m_edit_show_msg.SetWindowTextA("osip_message_set_body failed");
		return;
	}
	ret = osip_message_set_content_type(register_message, "text/plain");
	if (ret != OSIP_SUCCESS) {
		m_edit_show_msg.SetWindowTextA("osip_message_set_content_type failed");
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
				case EXOSIP_REGISTRATION_FAILURE:
					this->showMsg("EXOSIP_REGISTRATION_FAILURE");
					break;
				case EXOSIP_CALL_INVITE:
					this->showMsg("EXOSIP_CALL_INVITE");
					break;
				case EXOSIP_CALL_REDIRECTED:
					this->showMsg("EXOSIP_CALL_INVITE");
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

void CsipClientDlg::OnBnClickedBtnInvite()
{
	// TODO: 在此添加控件通知处理程序代码
	int ret = 0;
	DWORD ipAddr = 0;
	in_addr addr;
	memset(&addr, 0x00, sizeof(in_addr));
	char* svrIpStr = NULL;
	char local_ip[32] = { 0 };
	char from[128] = { 0 };
	char to[128] = { 0 };
	char route[128] = { 0 };
	char acTmpBuf[1024] = { 0 };
	std::string strMsg = "hello sip";

	m_svr_ip.GetAddress(ipAddr);
	addr.s_addr = htonl(ipAddr);
	svrIpStr = inet_ntoa(addr);

	//rtp
	int audio_type1 = 8;
	int audio_type2 = 0;
	int rtp_audio_port = 9654;
	int rtp_video_port = 9654;

	int isSpecialType = 2;

	ret = eXosip_guess_localip(sip_context, AF_INET, local_ip, 128);
	if (ret != OSIP_SUCCESS) {
		m_edit_show_msg.SetWindowTextA("eXosip_guess_localip failed");
		return;
	}
	sprintf_s(from, "sip:%s@%s:%d", FROM_NUMBER, local_ip, SIP_CLIENT_PORT);
	sprintf_s(to,"sip:%s@%s:%d", TO_NUMBER, svrIpStr, SIP_SERVER_PORT);
	sprintf_s(route, "sip:%s@%s:%d", TO_NUMBER, svrIpStr, SIP_SERVER_PORT);

	eXosip_lock(sip_context);
	do
	{
		osip_message_t* invite_msg = NULL;
		ret = eXosip_call_build_initial_invite(sip_context, &invite_msg, to, from, route, "This is a call to extension.");
		if(ret != OSIP_SUCCESS)
		{
			showMsg("eXosip_call_build_initial_invite fail");
			break;
		}
		/* SDP信息设置 */
		sprintf_s(acTmpBuf, VIS_SDP_2_INFO, "piratemike", local_ip, local_ip, 0,
			0, 1, isSpecialType, 0,
			SIP_VERSION, 0, 0, rtp_audio_port, audio_type1, audio_type2, rtp_video_port);

		osip_message_set_body(invite_msg, acTmpBuf, strlen(acTmpBuf));
		osip_message_set_content_type(invite_msg, "application/sdp");

		ret = eXosip_call_send_initial_invite(sip_context, invite_msg);
		if(ret < 0 )
		{
			CString strTmp;
			strTmp.Format("eXosip_call_send_initial_invite failed ret =%d", ret);
			showMsg(strTmp.GetBuffer());
			break;
		}
	} while (0);

	eXosip_unlock(sip_context);
}

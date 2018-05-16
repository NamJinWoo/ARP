// ARPAppDlg.cpp : implementation file
//
#include "stdafx.h"
#include "arp.h"
#include "ARPAppDlg.h"

#define UM_UPDATEDATA WM_USER+1

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CARPAppDlg dialog

// ���� �ʿ�
CARPAppDlg::CARPAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CARPAppDlg::IDD, pParent), 
	  CBaseLayer( "ARPDlg" ),
	  m_bSendReady( FALSE )
{
	//{{AFX_DATA_INIT(CARPAppDlg)
	m_unDstEthAddr = "0";

	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	// ������ ����
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	// -------------------------- ���� ���� -------------------------- //
	m_LayerMgr.AddLayer( new CARPLayer( "ARP" ) ) ; //  
	m_LayerMgr.AddLayer( new CIPLayer( "IP" ) ) ; 
	m_LayerMgr.AddLayer( new CEthernetLayer( "Ethernet" ) ) ; // 
	m_LayerMgr.AddLayer( new CNILayer( "NI" ) ) ; // 
	m_LayerMgr.AddLayer( this ) ; //  ChatDlg

	// -------------------------- ���� ���� -------------------------- //
	//m_LayerMgr.ConnectLayers(""); // ���� ������� ����
/*
	m_ARP = (CARPLayer*) m_LayerMgr.GetLayer( "ARP" ) ;
	m_IP = (CIPLayer*) m_LayerMgr.GetLayer( "IP" ) ;
	m_ETH = (CEthernetLayer*) m_LayerMgr.GetLayer( "Ethernet" ) ;
	m_NI = (CNILayer*) m_LayerMgr.GetLayer( "NI" ) ;
	*/
}

void CARPAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CARPAppDlg)
	DDX_Control(pDX, IDC_LIST_CHAT, m_ListChat);
	
	DDX_Text(pDX, IDC_EDIT_DSTMAC, m_unDstEthAddr);
	DDX_Control(pDX, IDC_EDIT_DSTIP, m_unDstIPAddr);

	//}}AFX_DATA_MAP
}


// MEssage �� �ڵ鷯 ���� 
BEGIN_MESSAGE_MAP(CARPAppDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER() 

	ON_MESSAGE(UM_UPDATEDATA,OnUpdateDataFalse)
	
	ON_BN_CLICKED(IDC_BUTTON_ITEMDELETE, &CARPAppDlg::OnBnClickedButtonItemdelete)
	ON_BN_CLICKED(IDC_BUTTON_ALLDELETE, &CARPAppDlg::OnBnClickedButtonAlldelete)
	ON_BN_CLICKED(IDC_BUTTON_SENDARP, &CARPAppDlg::OnBnClickedButtonSendarp)
	ON_BN_CLICKED(IDC_BUTTON_SENDGRATUITOUSARP, &CARPAppDlg::OnBnClickedButtonSendgratuitousarp)
	ON_BN_CLICKED(IDC_BUTTON_CANCLE, &CARPAppDlg::OnBnClickedButtonCancle)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CARPAppDlg::OnBnClickedButtonExit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CARPAppDlg message handlers

// ���� �ʿ�
BOOL CARPAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.
	
	// ������ ����

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, TRUE);		// Set small icon
	
	// TODO: Add extra initialization here

	//SetDlgState( arp_INITIALIZING ) ; // �⺻ ���� ����
	//SetDlgState( CFT_COMBO_SET ) ; // ��ī�� ���� ��������

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CARPAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ( nID == SC_CLOSE )
	{
		if ( MessageBox( "Are you sure ?", 
						 "Question", 
						 MB_YESNO | MB_ICONQUESTION )  == IDNO )
			return ;
		else EndofProcess( ) ;
	}

	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CARPAppDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CARPAppDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// ���� �ʿ�
void CARPAppDlg::SendData()
{
	CString MsgHeader ; 

	//------------- ä�ó��� ǥ�ø� ���ؼ� �������ּ� ���� -------------//
	unsigned char src_ip[4]; // src IP ����� ���� ����
	unsigned char dst_ip[4]; // src IP ����� ���� ����
	char srcIP[30];
	char dstIP[30];

	m_unDstIPAddr.GetAddress(dst_ip[0],dst_ip[1],dst_ip[2],dst_ip[3]); // ���� ��巹�� (ip)
	m_unSrcIPAddr.GetAddress(src_ip[0],src_ip[1],src_ip[2],src_ip[3]);

	::wsprintfA(srcIP, "%d.%d.%d.%d",src_ip[0],src_ip[1],src_ip[2],src_ip[3]);
	::wsprintfA(dstIP, "%d.%d.%d.%d",dst_ip[0],dst_ip[1],dst_ip[2],dst_ip[3]);
	//------------- IP �ּ� ���ڿ��� ���� �Ϸ� -------------//

	MsgHeader.Format( "[%s:%s] ", srcIP, dstIP ) ;

	m_ListChat.AddString( MsgHeader + m_stMessage ) ; // ä��â�� �޼������
	
	int nlength =  m_stMessage.GetLength();
	unsigned char *ppayload = new BYTE[nlength+1]; // '\0'�� ���ؼ� 1�� ��ū �迭 ����
	memset( ppayload, 0, nlength+1);
	memcpy(ppayload,(unsigned char*)(LPCTSTR)m_stMessage,nlength); // �޸� ����� ���� ���� 
	ppayload[nlength] = '\0'; //������ ��ġ �ΰ� ����
	// ����!

	if(!m_ARP->Send((unsigned char*)(LPCTSTR)m_stMessage, nlength))
		AfxMessageBox("���� ����!");
	
	delete ppayload; // �����Ҵ� ����
}

// ���� �ʿ�
BOOL CARPAppDlg::Receive(unsigned char *ppayload)
{
	CString Message; // ��µ� ����
	Message.Format( "[%s:%s] %s", m_IP->GetSrcAddressOfRecv(), m_IP->GetDstAddressOfRecv(), (char *)ppayload ) ;
	m_ListChat.AddString( Message ) ; // ä��â�� �޼������
	
	// ���ξƴ� �����忡�� GUI ȯ�� ���� �� �߻��� ������ �����ϱ� ���� message�� �̿��Ͽ� ���� �����忡�� ó�� //
	AfxGetApp()->m_pMainWnd->PostMessageA(UM_UPDATEDATA);

	return TRUE ;
}


LRESULT CARPAppDlg::OnUpdateDataFalse(WPARAM wParam, LPARAM lParam)
{
      UpdateData(FALSE);
      return 0;
}


BOOL CARPAppDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch( pMsg->message )
	{
	case WM_KEYDOWN :
		switch( pMsg->wParam )
		{
		case VK_RETURN : 
			if ( ::GetDlgCtrlID( ::GetFocus( ) ) == IDC_EDIT_DSTIP ) 
				OnBnClickedButtonSendarp( ) ;
			else if ( ::GetDlgCtrlID( ::GetFocus( ) ) == IDC_EDIT_DSTMAC ) 
				OnBnClickedButtonSendgratuitousarp( ) ;
			return FALSE ;
		case VK_ESCAPE : 
			return FALSE ;
		}
		break ;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}
// ���� �ʿ�
void CARPAppDlg::SetDlgState(int state)
{
	UpdateData( TRUE ) ;

	CButton*	pSendARPButton = (CButton*) GetDlgItem( IDC_BUTTON_SENDARP ) ;
	CButton*	pSendGARPButton = (CButton*) GetDlgItem( IDC_BUTTON_SENDGRATUITOUSARP ) ;
	
	CEdit*		pDstEdit = (CEdit*) GetDlgItem( IDC_EDIT_DSTMAC ) ;
	CEdit*		pDstIPEdit = (CEdit*) GetDlgItem( IDC_EDIT_DSTIP ) ;

	switch( state ){}

}

void CARPAppDlg::EndofProcess()
{
	m_LayerMgr.DeAllocLayer( ) ;
}

// ���� �ʿ�
void CARPAppDlg::OnTimer(UINT nIDEvent) 
{
	CDialog::OnTimer(nIDEvent);
}

// ���� �ʿ� // ItemDelete ��ư Ŭ����
void CARPAppDlg::OnBnClickedButtonItemdelete()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

// ���� �ʿ� // AllDelete ��ư Ŭ����
void CARPAppDlg::OnBnClickedButtonAlldelete()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

// ���� �ʿ� // arp send ��ư Ŭ����
void CARPAppDlg::OnBnClickedButtonSendarp()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData( TRUE ) ;

	if ( !m_unDstIPAddr.IsBlank() )
	{
		SetTimer(2018, 5000, NULL ) ;

		SendData( ) ; // ARP ��Ŷ ����
		m_unDstIPAddr.ClearAddress();
		(CEdit*) GetDlgItem( IDC_EDIT_DSTIP )->SetFocus( ) ;
	}

	UpdateData( FALSE ) ;
}

// ���� �ʿ� // gratuitousarp send ��ư Ŭ����
void CARPAppDlg::OnBnClickedButtonSendgratuitousarp()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData( TRUE ) ;

	if ( !m_unDstEthAddr.IsEmpty() )
	{
		SetTimer(2018, 5000, NULL ) ;

		SendData( ) ; // G-ARP ��Ŷ ����
		m_unDstEthAddr = "";
		(CEdit*) GetDlgItem( IDC_EDIT_DSTMAC )->SetFocus( ) ;
	}

	UpdateData( FALSE ) ;
}

// ���� �ʿ� // ��� ��ư Ŭ����
void CARPAppDlg::OnBnClickedButtonCancle()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

// ���� �ʿ� // ���� ��ư Ŭ����
void CARPAppDlg::OnBnClickedButtonExit()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}
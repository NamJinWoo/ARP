// ARPAppDlg.cpp : implementation file
//
#include "stdafx.h"
#include "arp.h"
#include "ARPAppDlg.h"

#define UM_UPDATEDATA WM_USER+1
#define UM_ARPTABLEUPDATE WM_USER+2

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

// 수정 필요
CARPAppDlg::CARPAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CARPAppDlg::IDD, pParent), 
	  CBaseLayer( "ARPDlg" ),
	  m_bSendReady( FALSE )
{
	//{{AFX_DATA_INIT(CARPAppDlg)
	m_unDstEthAddr = "0";

	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	// 아이콘 설정
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	// -------------------------- 계층 생성 -------------------------- //
	m_LayerMgr.AddLayer( new CARPLayer( "ARP" ) ) ; //  
	m_LayerMgr.AddLayer( new CIPLayer( "IP" ) ) ; 
	m_LayerMgr.AddLayer( new CEthernetLayer( "Ethernet" ) ) ; // 
	m_LayerMgr.AddLayer( new CNILayer( "NI" ) ) ; // 
	m_LayerMgr.AddLayer( this ) ; //  ChatDlg

	// -------------------------- 계층 연결 -------------------------- //
	m_LayerMgr.ConnectLayers("NI ( *Ethernet ( +IP ( -ARP *ARPDlg ) *ARP ) ) )"); // 계층 순서대로 연결

	m_ARP = (CARPLayer*) m_LayerMgr.GetLayer( "ARP" ) ;
	m_IP = (CIPLayer*) m_LayerMgr.GetLayer( "IP" ) ;
	m_ETH = (CEthernetLayer*) m_LayerMgr.GetLayer( "Ethernet" ) ;
	m_NI = (CNILayer*) m_LayerMgr.GetLayer( "NI" ) ;
}	

void CARPAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CARPAppDlg)
	DDX_Control(pDX, IDC_LIST_ARP, m_ListARP);
	
	DDX_Control(pDX, IDC_SRCIP, m_unSrcIPAddr);
	DDX_Text(pDX, IDC_EDIT_DSTMAC, m_unDstEthAddr);
	DDX_Control(pDX, IDC_EDIT_DSTIP, m_unDstIPAddr);
	DDX_Control(pDX, IDC_COMBO_NIC, m_ComboNIC);
	//}}AFX_DATA_MAP
}


// MEssage 별 핸들러 연결 
BEGIN_MESSAGE_MAP(CARPAppDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER() 

	ON_MESSAGE(UM_UPDATEDATA,OnUpdateDataFalse)
	
	ON_MESSAGE(UM_ARPTABLEUPDATE, OnARPTableUpdate)

	ON_BN_CLICKED(IDC_BUTTON_ITEMDELETE, &CARPAppDlg::OnBnClickedButtonItemdelete)
	ON_BN_CLICKED(IDC_BUTTON_ALLDELETE, &CARPAppDlg::OnBnClickedButtonAlldelete)
	ON_BN_CLICKED(IDC_BUTTON_SENDARP, &CARPAppDlg::OnBnClickedButtonSendarp)
	ON_BN_CLICKED(IDC_BUTTON_SENDGRATUITOUSARP, &CARPAppDlg::OnBnClickedButtonSendgratuitousarp)
	ON_BN_CLICKED(IDC_BUTTON_CANCLE, &CARPAppDlg::OnBnClickedButtonCancle)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CARPAppDlg::OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_SET, &CARPAppDlg::OnBnClickedButtonSet)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CARPAppDlg message handlers

// 수정 필요
BOOL CARPAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.
	
	// 제작자 설명

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

	SetDlgState( arp_INITIALIZING ) ; // 기본 통신 준비
	SetDlgState( CFT_COMBO_SET );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// 수정 필요
void CARPAppDlg::SetDlgState(int state)
{
	
	CButton*	pSendARPButton = (CButton*) GetDlgItem( IDC_BUTTON_SENDARP ) ;
	CButton*	pSendGARPButton = (CButton*) GetDlgItem( IDC_BUTTON_SENDGRATUITOUSARP ) ;
	CButton*	pAllDeleteButton = (CButton*) GetDlgItem( IDC_BUTTON_ALLDELETE ) ;
	CButton*	pItemDeleteButton = (CButton*) GetDlgItem( IDC_BUTTON_ITEMDELETE ) ;
	CButton*	pSetButton = (CButton*) GetDlgItem( IDC_BUTTON_SET ) ;
	
	CEdit*		pDstEdit = (CEdit*) GetDlgItem( IDC_EDIT_DSTMAC ) ;

	CComboBox*	pNICCombo = (CComboBox*) GetDlgItem( IDC_COMBO_NIC ) ;

	UpdateData( TRUE ) ;

	switch( state ){
	case arp_INITIALIZING: 
		// 기본 화면 세팅, arp 테이블 지우는 거 필요
		m_ListARP.ResetContent();
		m_ListARP.EnableWindow( FALSE );
		pSendARPButton->EnableWindow( FALSE );
		pSendGARPButton->EnableWindow( FALSE );
		m_unDstIPAddr.EnableWindow( FALSE );
		pDstEdit->EnableWindow( FALSE );
		pAllDeleteButton->EnableWindow( FALSE );
		pItemDeleteButton->EnableWindow( FALSE );
		break;
	case arp_ADDR_SET:
		// 설정 버튼 누름
		m_NI->SetThreadState( TRUE ) ;

		SetAddresses();
		SetTimer(2018, 1000, NULL ) ;

		m_ListARP.EnableWindow( TRUE );
		pSendARPButton->EnableWindow( TRUE );
		pSendGARPButton->EnableWindow( TRUE );
		m_unDstIPAddr.EnableWindow( TRUE );
		pDstEdit->EnableWindow( TRUE );
		pAllDeleteButton->EnableWindow( TRUE );
		pItemDeleteButton->EnableWindow( TRUE );
		m_ComboNIC.EnableWindow( FALSE );
		m_unSrcIPAddr.EnableWindow( FALSE );
		pSetButton->SetWindowText( "재설정" ) ; 
		break;
	case arp_ADDR_RESET:
		// 재설정 버튼 누름
		m_NI->SetThreadState( FALSE ) ;

		KillTimer(2018) ;

		m_ListARP.EnableWindow( FALSE );
		pSendARPButton->EnableWindow( FALSE );
		pSendGARPButton->EnableWindow( FALSE );
		m_unDstIPAddr.EnableWindow( FALSE );
		pDstEdit->EnableWindow( FALSE );
		pAllDeleteButton->EnableWindow( FALSE );
		pItemDeleteButton->EnableWindow( FALSE );
		m_ComboNIC.EnableWindow( TRUE );
		m_unSrcIPAddr.EnableWindow( TRUE );
		pSetButton->SetWindowText( "설정" ) ; 
		break;
	case CFT_COMBO_SET: // 왜 안되지 ?
		CString NIC_description;

		for(int i = 0 ; i< NI_COUNT_NIC ; i ++ )
		{
			if( !m_NI->GetAdapter( i ) )
				break;
			NIC_description = m_NI->GetAdapter( i )->description ;
			NIC_description.Trim();
			
			m_ComboNIC.AddString( NIC_description ) ;
			
		}
		m_ComboNIC.SetCurSel( 0 ) ;
		break;
	}
	UpdateData( FALSE ) ;

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

void CARPAppDlg::SetAddresses() // 입력된 주소로 IP 헤더 설정
{
	// 맥주소, IP 주소 설정
	BYTE src_ip[4];

	// 입력된 값으로 주소 설정
	int index = m_ComboNIC.GetCurSel() ;
	
	PPACKET_OID_DATA OidData = (PPACKET_OID_DATA)malloc(sizeof(PACKET_OID_DATA));
	OidData->Oid = 0x01010101;
	OidData->Length = 6;

	LPADAPTER adapter = PacketOpenAdapter( m_NI->GetAdapter( index )->name ) ;
	PacketRequest( adapter, FALSE, OidData ) ;

	m_ETH->SetSourceAddress(OidData->Data);

	m_unSrcIPAddr.GetAddress(src_ip[0],src_ip[1],src_ip[2],src_ip[3]); // 출발 어드레스 (ip)
	
	// arp 헤더 주소 설정
	m_ARP->setSrcIPAddress( src_ip );
	m_ARP->setSrcMacAddress( OidData->Data );

	m_ARP->setMyIPAddress( src_ip );
	m_ARP->setMyMacAddress( OidData->Data );

	
	m_IP->SetSourceAddress(src_ip);
}

// 수정 필요 // 아이피 아래로 보내기
void CARPAppDlg::SendARP()
{
	unsigned char dst_ip[4]; // arp 대상
	unsigned char temp = 'a';

	m_unDstIPAddr.GetAddress(dst_ip[0],dst_ip[1],dst_ip[2],dst_ip[3]); // 도착 어드레스 (ip)
	
	m_IP->SetDestinAddress( dst_ip );
	// arp, 주소 설정
	m_ARP->setDstIPAddress( dst_ip );

	BOOL bSuc = m_IP->Send(&temp,1);

	// 전송!
	if( !bSuc )
		AfxMessageBox(" 전송 오류 ");
}

void CARPAppDlg::SendGARP()
{
	unsigned char dst_ip[4]; // arp 대상
	unsigned char dst_mac[6]; // arp 대상
	unsigned char temp = 'a';

	// GARP - targetIP = 본인, targetMAC = ~~, sendIP = 본인, sendMAC = 입력 값
	m_unDstIPAddr.GetAddress(dst_ip[0],dst_ip[1],dst_ip[2],dst_ip[3]); // 도착 어드레스 (ip)
	sscanf(m_unDstEthAddr, "%02x%02x%02x%02x%02x%02x", &dst_mac[0], &dst_mac[1], &dst_mac[2], &dst_mac[3], &dst_mac[4], &dst_mac[5]);
	
	// arp, 주소 설정

	m_ARP->setSrcIPAddress( dst_ip );
	m_ARP->setDstIPAddress( dst_ip );
	m_ARP->setSrcMacAddress( dst_mac );

	BOOL bSuc = m_IP->Send(&temp,1);

	// 전송!
	if( !bSuc )
		AfxMessageBox(" 전송 오류 ");
}

// ARP 정보 수신하여, listbox 업데이트, 아마 안 쓸듯
BOOL CARPAppDlg::Receive(unsigned char *ppayload)
{
	CString Message; // 출력될 내용
	Message.Format( "%s", (char *)ppayload ) ;
	m_ListARP.AddString( Message ) ; // 채팅창에 메세지출력
	
	// 메인아닌 스레드에서 GUI 환경 접근 시 발생할 오류를 방지하기 위해 message를 이용하여 메인 스레드에서 처리 //
	AfxGetApp()->m_pMainWnd->PostMessageA(UM_UPDATEDATA);

	return TRUE ;
}


LRESULT CARPAppDlg::OnUpdateDataFalse(WPARAM wParam, LPARAM lParam)
{
      UpdateData(FALSE);
      return 0;
}

LRESULT CARPAppDlg::OnARPTableUpdate(WPARAM wParam, LPARAM lParam)
{
	int table_count = m_ARP->getTableCount();
		int i = 0;

		m_ListARP.ResetContent();

		while ( i < table_count ){
			CString i_info = m_ARP->checkTableState(i);

			m_ListARP.AddString( i_info );
		}

	AfxGetApp()->m_pMainWnd->PostMessageA(UM_UPDATEDATA);
    
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

void CARPAppDlg::EndofProcess()
{
	m_LayerMgr.DeAllocLayer( ) ;
}

// 수정 필요 // 1초마다 arp table 확인 
void CARPAppDlg::OnTimer(UINT nIDEvent) 
{
	CDialog::OnTimer(nIDEvent);

	// ARP TABLE CHECK TIMER 인 경우
	if( nIDEvent == 2018 ){
		// 테이블 내의 접근
		int table_count = m_ARP->getTableCount();
		int i = 0;

		m_ListARP.ResetContent();

		while ( i < table_count ){
			CString i_info = m_ARP->checkTableState(i);

			m_ListARP.AddString( i_info );
		}
	}
}

void CARPAppDlg::OnBnClickedButtonSet(){
	UpdateData( TRUE ) ;

	if ( m_bSendReady ){
		SetDlgState( arp_ADDR_RESET ) ;
		SetDlgState( arp_INITIALIZING ) ;
	}
	else{
		// NI_수신 시작
		int index = m_ComboNIC.GetCurSel() ; // 현재 선택한 랜카드 인덱스가져오기

		m_NI->SetAdapterNum( index ); // 가져온 인덱스로 통신 준비
		m_NI->OpenSocket() ; // 통신 가능 설정

		SetDlgState( arp_ADDR_SET ) ;
	}

	m_bSendReady = !m_bSendReady ; // 이전 상태와 반대로 바꿈
}

// 수정 필요 // ItemDelete 버튼 클릭시
void CARPAppDlg::OnBnClickedButtonItemdelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSel = m_ListARP.GetCurSel();

	if( nSel >= 0 ){
		m_ListARP.DeleteString( nSel );
		// arp table 삭제
		m_ARP->itemDelete( nSel );
	}
}

// 수정 필요 // AllDelete 버튼 클릭시
void CARPAppDlg::OnBnClickedButtonAlldelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_ListARP.ResetContent();
	// arptable 초기화
	m_ARP->allDelete();
}

// 수정 필요 // arp send 버튼 클릭시
void CARPAppDlg::OnBnClickedButtonSendarp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE ) ;

	if ( !m_unDstIPAddr.IsBlank() )
	{
		SendARP() ; // ARP 패킷 전송
		m_unDstIPAddr.ClearAddress();
		(CEdit*) GetDlgItem( IDC_EDIT_DSTIP )->SetFocus( ) ;
	}

	UpdateData( FALSE ) ;
}

// 수정 필요 // gratuitousarp send 버튼 클릭시
void CARPAppDlg::OnBnClickedButtonSendgratuitousarp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE ) ;

	if ( !m_unDstEthAddr.IsEmpty() )
	{

		SendGARP( ) ; // G-ARP 패킷 전송
		
		m_unDstEthAddr = "";
		(CEdit*) GetDlgItem( IDC_EDIT_DSTMAC )->SetFocus( ) ;
	}

	UpdateData( FALSE ) ;
}

// 수정 필요 // 취소 버튼 클릭시 // 없어도 될듯
void CARPAppDlg::OnBnClickedButtonCancle()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

// 수정 필요 // 종료 버튼 클릭시
void CARPAppDlg::OnBnClickedButtonExit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

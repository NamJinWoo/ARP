// IPLayer.cpp: implementation of the CIPLayer class.
//
//////////////////////////////////////////////////////////////////////

/*
 *  Test 버전
 */

#include "stdafx.h"
#include "arp.h"
#include "IPLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPLayer::CIPLayer( char* pName )
: CBaseLayer( pName )
{
	ResetHeader( ) ;
}

CIPLayer::~CIPLayer()
{
}

// Ether 계층 리셋
void CIPLayer::ResetHeader()
{
	m_sHeader.ip_verlen = 0x00; // ip version		(1 byte)
	m_sHeader.ip_tos = 0x00;	// type of service	(1 byte)
	m_sHeader.ip_len = 0x0000;  // total packet length   (2 byte)
	m_sHeader.ip_id = 0x0000;   // datagram id		     (2 byte)
	m_sHeader.ip_fragoff = 0x0000; // fragment offset		   (2 bytes)
	m_sHeader.ip_ttl = 0x00;   // time to live in gateway hops		(1 byte)
	m_sHeader.ip_proto = 0x00; // IP protocol			 (1 byte)
	m_sHeader.ip_cksum = 0x00; // header checksum		 (2 bytes)
	memset( m_sHeader.ip_src, 0, 4); // IP address of source (4 bytes)
	memset( m_sHeader.ip_dst, 0, 4); // IP address of destination (4 bytes)
	memset( m_sHeader.ip_data, 0, IP_DATA_SIZE);  // variable length data
}

char* CIPLayer::GetSrcAddressOfRecv()
{
	return this->m_SrcOfRecv;
}

char* CIPLayer::GetDstAddressOfRecv()
{
	return this->m_DstOfRecv;
	
}

void CIPLayer::SetSourceAddress(unsigned char *pAddress)
{
	// 바이트 오더 처리된 체로 넘어옴
	memcpy( m_sHeader.ip_src, pAddress, 4 );
}

void CIPLayer::SetDestinAddress(unsigned char *pAddress)
{
	// 바이트 오더 처리된 체로 넘어옴
	memcpy( m_sHeader.ip_dst, pAddress, 4 ) ;
}

// 해줄게 없음. 헤더의 주소 설정은 ARPAppDlg에서 이미함
BOOL CIPLayer::Send(unsigned char *ppayload, int nlength)
{
	memcpy( m_sHeader.ip_data, ppayload, nlength ) ;
	BOOL bSuccess = FALSE ;
	m_sHeader.ip_len = nlength;
	bSuccess = this->mp_UnderLayer->Send((unsigned char*)(&m_sHeader), nlength + IP_HEADER_SIZE);

	return bSuccess ;
}

// private 변수인 IP 주소를 담는 친구들을 설정해줘야함. 나중에 ARPAppDlg에서 채팅 내용보여줄때 사용할꺼, 1대1 통신이지만 혹시 몰라서.
BOOL CIPLayer::Receive( unsigned char* ppayload )
{
	LPIPLayer_HEADER lpFrame = (LPIPLayer_HEADER) ppayload ;

	BOOL bSuccess = FALSE ;

	// 아이피 확인 필요, 나에게 왔으며, 연결 했던 상대로 부터 왔는 지 //
	if( memcmp( m_sHeader.ip_src, lpFrame->ip_dst, 4 ) == 0 &&		  // 상대방의 도착지가 내가 맞는지
		memcmp( m_sHeader.ip_src, lpFrame->ip_src, 4 ) != 0	)		  // 내가 보낸 건 아닌지 //
	{
		// 패킷의 주소 저장, 단 넘어온 패킷이 네트워크 바이트 순서라는 거 잊지말기, 즉 빅인디안이니 저장할때 리틀인디안으로 //
		// arpAppLayer 에서 패킷 내용 출력시 사용하기 위함 //
		::wsprintfA( m_SrcOfRecv, "%d.%d.%d.%d",lpFrame->ip_src[3],lpFrame->ip_src[2],lpFrame->ip_src[1],lpFrame->ip_src[0]);
		::wsprintfA( m_DstOfRecv, "%d.%d.%d.%d",lpFrame->ip_dst[3],lpFrame->ip_dst[2],lpFrame->ip_dst[1],lpFrame->ip_dst[0]);

		bSuccess = this->mp_aUpperLayer[0]->Receive(lpFrame->ip_data);
	}
	else bSuccess = FALSE;

	return bSuccess ;
}

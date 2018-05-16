// EthernetLayer.cpp: implementation of the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "arp.h"
#include "EthernetLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEthernetLayer::CEthernetLayer( char* pName )
: CBaseLayer( pName )
{
	ResetHeader( ) ;
}

CEthernetLayer::~CEthernetLayer()
{
}

// Ether 계층 리셋
void CEthernetLayer::ResetHeader()
{
	memset( m_sHeader.enet_dstaddr.addrs, 0, 6 ) ;
	memset( m_sHeader.enet_srcaddr.addrs, 0, 6 ) ;
	memset( m_sHeader.enet_data, 0, ETHER_MAX_DATA_SIZE) ;
	m_sHeader.enet_type = 0x0000;
}

unsigned char* CEthernetLayer::GetSourceAddress()
{
	return m_sHeader.enet_srcaddr.addrs ;
}

unsigned char* CEthernetLayer::GetDestinAddress()
{
	return m_sHeader.enet_dstaddr.addrs ;
}

void CEthernetLayer::SetSourceAddress(unsigned char *pAddress)
{
	memcpy(m_sHeader.enet_srcaddr.addrs, pAddress, 6);
}

void CEthernetLayer::SetDestinAddress(unsigned char *pAddress)
{
	memcpy( m_sHeader.enet_dstaddr.addrs, pAddress, 6 ) ;
}

// 할게 없음. type 0x800으로 설정, 바이트 오더
BOOL CEthernetLayer::Send(unsigned char *ppayload, int nlength)
{
	memcpy( m_sHeader.enet_data, ppayload, nlength ) ;

	BOOL bSuccess = FALSE ;
	m_sHeader.enet_type = htons(0x800);
	bSuccess = this->mp_UnderLayer->Send((unsigned char*)(&m_sHeader), nlength + ETHER_HEADER_SIZE);

	return bSuccess ;
}
// 맥주소 확인, 같은 바이트오더를 쓰는 장치간 통신이니 바이트 오더는 신경 쓰지 않아도 됨
BOOL CEthernetLayer::Receive( unsigned char* ppayload )
{
	LPETHERNET lpFrame = (LPETHERNET) ppayload ;

	BOOL bSuccess = FALSE ;

	// 이더넷 주소 확인 // 
	if( memcmp( m_sHeader.enet_srcaddr.addrs, lpFrame->enet_dstaddr.addrs, 6 ) == 0 &&		// 나한테 보냈으며 //
		memcmp( m_sHeader.enet_srcaddr.addrs, lpFrame->enet_srcaddr.addrs, 6 ) != 0 )		// 내가 보낸게 아닌 것만 수신 //
		if( ntohs(lpFrame->enet_type) == 0x800) // IP 프로토콜인 경우만 처리 //
			bSuccess = this->mp_aUpperLayer[0]->Receive(lpFrame->enet_data);
		else // 그외 거부 //
			bSuccess = FALSE;

	return bSuccess ;
}

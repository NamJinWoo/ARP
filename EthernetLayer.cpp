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

void CEthernetLayer::SetEnetType( unsigned short enet_type ){

	this->m_sHeader.enet_type = htons( enet_type );

}

// 할게 없음. type 0x800으로 설정, 바이트 오더
BOOL CEthernetLayer::Send(unsigned char *ppayload, int nlength)
{
	memcpy( m_sHeader.enet_data, ppayload, nlength ) ;

	BOOL bSuccess = FALSE ;
	bSuccess = this->mp_UnderLayer->Send(( unsigned char* )( &m_sHeader ), nlength + ETHER_HEADER_SIZE ); //UnderLayer는 NILayer

	return bSuccess ;
}
// 맥주소 확인, 같은 바이트오더를 쓰는 장치간 통신이니 바이트 오더는 신경 쓰지 않아도 됨
BOOL CEthernetLayer::Receive( unsigned char* ppayload )
{
	LPETHERNET lpFrame = (LPETHERNET) ppayload ;
	
	unsigned char broadcast[6] = {0xff,0xff,0xff,0xff,0xff,0xff};

	BOOL bSuccess = FALSE ;

	// 이더넷 주소 확인 // 
	if( ( memcmp( m_sHeader.enet_srcaddr.addrs, lpFrame->enet_dstaddr.addrs, 6 ) == 0 || 	// 나한테 보냈거나 broadcast 인데, 내가 보낸게 아닌 것 //
		  memcmp( lpFrame->enet_dstaddr.addrs, broadcast, 6 ) == 0 ) &&		
		  memcmp( m_sHeader.enet_srcaddr.addrs, lpFrame->enet_srcaddr.addrs, 6 ) != 0 ){	
		if( ntohs(lpFrame->enet_type) == 0x800) { // IP 프로토콜인 경우 처리 //
			bSuccess = this->mp_aUpperLayer[0]->Receive(lpFrame->enet_data); //IPLayer는 0번 !IPLayer 먼저 set해야함
		}
		else if (ntohs(lpFrame->enet_type) == 0x806) { // ARP 프로토콜인 경우 처리 //
			bSuccess = this->mp_aUpperLayer[1]->Receive(lpFrame->enet_data); //ARPLayer는 1번
		}
		else {
			bSuccess = FALSE;
		}
	}
	return bSuccess ;
}

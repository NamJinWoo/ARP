// IPLayer.cpp: implementation of the CIPLayer class.
//
//////////////////////////////////////////////////////////////////////

/*
 *  Test ����
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

// Ether ���� ����
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
	// ����Ʈ ���� ó���� ü�� �Ѿ��
	memcpy( m_sHeader.ip_src, pAddress, 4 );
}

void CIPLayer::SetDestinAddress(unsigned char *pAddress)
{
	// ����Ʈ ���� ó���� ü�� �Ѿ��
	memcpy( m_sHeader.ip_dst, pAddress, 4 ) ;
}

// ���ٰ� ����. ����� �ּ� ������ ARPAppDlg���� �̹���
BOOL CIPLayer::Send(unsigned char *ppayload, int nlength)
{
	memcpy( m_sHeader.ip_data, ppayload, nlength ) ;
	BOOL bSuccess = FALSE ;
	m_sHeader.ip_len = nlength;
	bSuccess = this->mp_UnderLayer->Send((unsigned char*)(&m_sHeader), nlength + IP_HEADER_SIZE);

	return bSuccess ;
}

// private ������ IP �ּҸ� ��� ģ������ �����������. ���߿� ARPAppDlg���� ä�� ���뺸���ٶ� ����Ҳ�, 1��1 ��������� Ȥ�� ����.
BOOL CIPLayer::Receive( unsigned char* ppayload )
{
	LPIPLayer_HEADER lpFrame = (LPIPLayer_HEADER) ppayload ;

	BOOL bSuccess = FALSE ;

	// ������ Ȯ�� �ʿ�, ������ ������, ���� �ߴ� ���� ���� �Դ� �� //
	if( memcmp( m_sHeader.ip_src, lpFrame->ip_dst, 4 ) == 0 &&		  // ������ �������� ���� �´���
		memcmp( m_sHeader.ip_src, lpFrame->ip_src, 4 ) != 0	)		  // ���� ���� �� �ƴ��� //
	{
		// ��Ŷ�� �ּ� ����, �� �Ѿ�� ��Ŷ�� ��Ʈ��ũ ����Ʈ ������� �� ��������, �� ���ε���̴� �����Ҷ� ��Ʋ�ε������ //
		// arpAppLayer ���� ��Ŷ ���� ��½� ����ϱ� ���� //
		::wsprintfA( m_SrcOfRecv, "%d.%d.%d.%d",lpFrame->ip_src[3],lpFrame->ip_src[2],lpFrame->ip_src[1],lpFrame->ip_src[0]);
		::wsprintfA( m_DstOfRecv, "%d.%d.%d.%d",lpFrame->ip_dst[3],lpFrame->ip_dst[2],lpFrame->ip_dst[1],lpFrame->ip_dst[0]);

		bSuccess = this->mp_aUpperLayer[0]->Receive(lpFrame->ip_data);
	}
	else bSuccess = FALSE;

	return bSuccess ;
}
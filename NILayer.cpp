// NILayer.cpp: implementation of the CNILayer class.
//
//////////////////////////////////////////////////////////////////////

/*
 * Test 버전
 */

#include "stdafx.h"
#include "arp.h"
#include "NILayer.h"
#include <stdio.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using namespace std;

CNILayer::CNILayer( char* pName, LPADAPTER *lpAdapterm, int iAdapterNumToUse )
: CBaseLayer( pName )
{
	m_pAdapter = NULL;
	m_AdapterNumToUse = iAdapterNumToUse;
	m_thrdState = TRUE;
	SetAdapterList();
}

CNILayer::~CNILayer()
{
}
// 선택된 랜카드를 통해 패킷 전송
BOOL CNILayer::Send(unsigned char *ppayload, int nlength)
{
	if(pcap_sendpacket(this->m_pAdapter, ppayload, nlength))
	{
		AfxMessageBox("패킷 전송 실패");	
		return FALSE;
	}

	return TRUE ;
}
// 패킷 송신과 수신을 동시하기 위한 수신 스레드에 사용되는 함수 //
UINT CNILayer::ReadingThread(LPVOID lpParam){
	struct pcap_pkthdr *header;
	const u_char *pkt_data;
	int result;
	
	CNILayer *pNI = ( CNILayer *) lpParam;

	while( pNI->m_thrdState ){
		// 수신 가능 상태에 놓일 시, 즉 설정 버튼을 눌렀을 시 //
		result = pcap_next_ex( pNI->m_pAdapter, &header, &pkt_data );
		// 오프라인 캡처 또는 인터페이스로 부터 패킷을 읽음 //
		// 1 : 성공, 0 : timeout, -1 : 에러, -2 : EOF //
		if( result == 0 ) {}
		// 성공 일 경우 상위 레이어로 전달 //
		else if ( result == 1 ){ pNI->Receive((u_char *)pkt_data);} 
		else if ( result < 0 ) {}
	}

	return 0;
}
BOOL CNILayer::Receive( unsigned char* ppayload )
{	
	return  this->mp_aUpperLayer[0]->Receive(ppayload);
}

// 가능한 랜카드, 어댑터 중 현재 쓸거 선택해서 통신 준비 완료!
void CNILayer::OpenSocket()
{
	char errbuf[PCAP_ERRBUF_SIZE];

	if( ( m_pAdapter = pcap_open_live(m_pAdapterList[m_AdapterNumToUse]->name, 1514, 1, -1, errbuf ) ) == NULL )
	{// 디바이스 open , m_pAdapterList에서 m_AdapterNumToUse번째의 어댑터 사용
	// pcap_open_live( 장치의 이름, 캡쳐할 크기( 이더넷의 최대 단위), Promiscuous mode를 사용할 지, 패킷이 도착했을 때 패킷이 
	// 잠시 기다리는 시간, errbuf 에러가 발생했을 때 에러 원인을 저장/ 성공하면 Null값 저장)

		AfxMessageBox(errbuf);
		return ;
	}
	
	AfxBeginThread(ReadingThread, this);
}

void CNILayer::SetThreadState( BOOL state )
{
	m_thrdState = state ;
}

// 랜카드 목록 만들기
void CNILayer::SetAdapterList()
{
	pcap_if_t *alldevs, *d;
	int i = 0; // for iter
	char errbuf[PCAP_ERRBUF_SIZE];

	for(int j=0 ; j < NI_COUNT_NIC ; j++)
		m_pAdapterList[j] = NULL;

	if( pcap_findalldevs( &alldevs, errbuf ) == -1 ){
		AfxMessageBox("NIC 없음");
		return;
	}

	if( !alldevs ){
		AfxMessageBox("NIC 없음");
		return;
	}

	for( d = alldevs ; d ; d = d->next ){
		m_pAdapterList[i++] = d ;
	}
}

pcap_if_t* CNILayer::GetAdapter(int index){
	return m_pAdapterList[index];
}

// 사용할 랜카드 설정
void CNILayer::SetAdapterNum(int num)
{
	m_AdapterNumToUse = num ;
}
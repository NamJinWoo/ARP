// ARPLayer.cpp: implementation of the CARPLayer class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "arp.h"
#include "ARPLayer.h"
#include "EthernetLayer.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define UM_ARPTABLEUPDATE WM_USER+2
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CARPLayer::CARPLayer( char* pName ) 
: CBaseLayer( pName ), 
  mp_Dlg( NULL )
{
	ResetHeader( ) ;
}

CARPLayer::~CARPLayer()
{
}


// 헤더 리셋
void CARPLayer::ResetHeader( )
{
	memset(m_sHeader.senderMac, 0, 6);
	memset(m_sHeader.targetMac, 0, 6);
}
// host ip, mac 주소 설정
void CARPLayer::setMyIPAddress(unsigned char* pAddress)
{
	memcpy(m_srcIP, pAddress, 4);
}
void CARPLayer::setMyMacAddress(unsigned char* pAddress)
{
	memcpy(m_srcMAC, pAddress, 6);
}

unsigned char * CARPLayer::getMyMacAddress()
{
	return m_srcMAC;
}
///////이제 ARP Packet을 설정해봅시다 ㅎㅎ /////////////

//송신자의 IP,MAC 주소를 설정한다.
//Source IPAddress를 설정 합니다^^
void CARPLayer::setSrcIPAddress(unsigned char* pAddress)
{
	memcpy(m_sHeader.senderIP, pAddress, 4);
}
void CARPLayer::setSrcMacAddress(unsigned char* pAddress)
{
	memcpy(m_sHeader.senderMac, pAddress, 6);
}

//수신자의 IP,MAC주소를 설정한다.
//Destination IPAddress를 설정 합니다^^
void CARPLayer::setDstIPAddress(unsigned char* pAddress)
{
	memcpy(m_sHeader.targetIP, pAddress, 4);
}
void CARPLayer::setDstMacAddress(unsigned char* pAddress)
{
	memcpy(m_sHeader.targetMac, pAddress, 6);
}

BOOL CARPLayer::Send(unsigned char *ppayload, int nlength)
{
	int ARPindex = SearchCacheTable( m_sHeader.targetIP );
	//ARP Table안에 있는지 검사한다.함수를 돌리는데
	if( ARPindex == -1 ){
		//없으면 arp패킷 전송 헤더에 있는 값들 설정해줌...
		m_sHeader.protType = htons(0x0800); //request니깐 0800로 설정한다.
		m_sHeader.hardType = htons(0x0001);
		m_sHeader.protSize = 4;
		m_sHeader.hardSize = 6;
		m_sHeader.op = htons(0x0001); //request 니깐 1로 설정.
		
		
		unsigned char broadcast[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
		
		// GARP 사용시 돌아올 맥주소를 위해 실제 NIC 맥주소를 같이 보낸다.
		setDstMacAddress( m_srcMAC );
	
		if( memcmp( m_sHeader.targetIP, m_srcIP, 4 ) != 0 ){
			// arp 인경우만
			CacheTable data;
			memcpy( data.ipAddress, m_sHeader.targetIP, 4 );
			memcpy( data.macAddress, broadcast, 6 );
			data.state.Format("%s","imcomplete");
			data.time = TIMEOFIMCOMPLETE;
		
			// arp table에 추가
			cache_table.AddTail( data );

			 //없으면 FF-FF-FF...으로 다보냄...
			//이더넷 주소를 브로드캐스트로 보냄.
			AfxGetApp()->m_pMainWnd->PostMessageA(UM_ARPTABLEUPDATE);
		}
		
		((CEthernetLayer * )mp_UnderLayer)->SetEnetType( 0x806 );
		((CEthernetLayer * )mp_UnderLayer)->SetDestinAddress(broadcast);

        return mp_UnderLayer->Send((unsigned char*)&m_sHeader , sizeof(m_sHeader));//여기서 저장된 정보를 하위 레이어로 보냄
		 //ARP Request 전송
		//그 index를 가지고 전송.
	}
	return TRUE ;
}

// 받기
BOOL CARPLayer::Receive( unsigned char* ppayload )
{
	LPARPLayer_HEADER pFrame = (LPARPLayer_HEADER)ppayload;

	if( ntohs(pFrame->op) == 0x0001 ){ // arp request
		m_sHeader.protType = htons(0x0800); 
		m_sHeader.hardType = htons(0x0001);
		m_sHeader.protSize = 4;
		m_sHeader.hardSize = 6;
		m_sHeader.op = htons(0x0002); //reply 니깐 1로 설정.

		setSrcIPAddress( m_srcIP );
		setSrcMacAddress( m_srcMAC );
		setDstIPAddress( pFrame->senderIP );
		setDstMacAddress( pFrame->senderMac );
		
		// 송신한 사람한테만 전송하기위해
		((CEthernetLayer * )mp_UnderLayer)->SetEnetType( 0x806 );
		((CEthernetLayer * )mp_UnderLayer)->SetDestinAddress( pFrame->targetMac );

		if( memcmp( pFrame->targetIP, pFrame->senderIP, 4 ) == 0 ){
			// garp 요청인 경우
			if( memcmp( pFrame->targetIP, m_srcIP, 4 ) == 0 ){
				// 본인 IP 이면 충돌, 보내고
				return mp_UnderLayer->Send((unsigned char*)&m_sHeader , sizeof(m_sHeader));
			}else{
				// 본인 IP 아니면 ARP Table update, 안보내고
				int ARPindex = SearchCacheTable( pFrame->senderIP );

				if( ARPindex == -1 ){
					CacheTable data;
					memcpy( data.ipAddress, pFrame->senderIP, 4 );
					memcpy( data.macAddress, pFrame->senderMac, 6 );
					data.state.Format("%s","complete");
					data.time = TIMEOFCOMPLETE;
					cache_table.AddTail( data );
				}else{
					memcpy( cache_table.GetAt( cache_table.FindIndex( ARPindex ) ).ipAddress, pFrame->senderIP, 4);
					memcpy( cache_table.GetAt( cache_table.FindIndex( ARPindex ) ).macAddress, pFrame->senderMac, 6);
					cache_table.GetAt( cache_table.FindIndex( ARPindex ) ).state.Format("%s","complete");
					cache_table.GetAt( cache_table.FindIndex( ARPindex ) ).time = TIMEOFCOMPLETE;
				}

				AfxGetApp()->m_pMainWnd->PostMessageA(UM_ARPTABLEUPDATE);
			}
		}
		else{
			// arp 요청인 경우, 업데이트 후 보내기
			int ARPindex = SearchCacheTable( pFrame->senderIP );

			if( ARPindex == -1 ){
				CacheTable data;
				memcpy( data.ipAddress, pFrame->senderIP, 4 );
				memcpy( data.macAddress, pFrame->senderMac, 6 );
				data.state.Format("%s","complete");
				data.time = TIMEOFCOMPLETE;
				cache_table.AddTail( data );
			}else{
				memcpy( cache_table.GetAt( cache_table.FindIndex( ARPindex ) ).ipAddress, pFrame->senderIP, 4);
				memcpy( cache_table.GetAt( cache_table.FindIndex( ARPindex ) ).macAddress, pFrame->senderMac, 6);
				cache_table.GetAt( cache_table.FindIndex( ARPindex ) ).state.Format("%s","complete");
				cache_table.GetAt( cache_table.FindIndex( ARPindex ) ).time = TIMEOFCOMPLETE;
			}
			
			AfxGetApp()->m_pMainWnd->PostMessageA(UM_ARPTABLEUPDATE);

			if( memcmp( pFrame->targetIP, m_srcIP, 4 ) == 0 ){
				// 본인 IP 인가
				return mp_UnderLayer->Send((unsigned char*)&m_sHeader , sizeof(m_sHeader));
			}
		}
	}
	else if( ntohs(pFrame->op) == 0x0002 ){
		//reply
		if( memcmp( pFrame->targetIP, pFrame->senderIP, 4 ) == 0 ){
			// garp 응답인 경우, arp 테이블 업데이트하고, 충돌표시 해줌
			int ARPindex = SearchCacheTable( pFrame->senderIP );

			if( ARPindex == -1 ){
				CacheTable data;
				memcpy( data.ipAddress, pFrame->senderIP, 4 );
				memcpy( data.macAddress, pFrame->senderMac, 6 );
				data.state.Format("%s","complete");
				data.time = TIMEOFCOMPLETE;
				cache_table.AddTail( data );
			}else{
				memcpy( cache_table.GetAt( cache_table.FindIndex( ARPindex ) ).ipAddress, pFrame->senderIP, 4);
				memcpy( cache_table.GetAt( cache_table.FindIndex( ARPindex ) ).macAddress, pFrame->senderMac, 6);
				cache_table.GetAt( cache_table.FindIndex( ARPindex ) ).state.Format("%s","complete");
				cache_table.GetAt( cache_table.FindIndex( ARPindex ) ).time = TIMEOFCOMPLETE;
			}

			AfxGetApp()->m_pMainWnd->PostMessageA(UM_ARPTABLEUPDATE);
			AfxMessageBox("IP 충돌");
		}
		else{
			// arp 응답인 경우, 업데이트
			int ARPindex = SearchCacheTable( pFrame->senderIP );

			if( ARPindex == -1 ){
				CacheTable data;
				memcpy( data.ipAddress, pFrame->senderIP, 4 );
				memcpy( data.macAddress, pFrame->senderMac, 6 );
				data.state.Format("%s","complete");
				data.time = TIMEOFCOMPLETE;
				cache_table.AddTail( data );
			}else{
				memcpy( cache_table.GetAt( cache_table.FindIndex( ARPindex ) ).ipAddress, pFrame->senderIP, 4);
				memcpy( cache_table.GetAt( cache_table.FindIndex( ARPindex ) ).macAddress, pFrame->senderMac, 6);
				cache_table.GetAt( cache_table.FindIndex( ARPindex ) ).state.Format("%s","complete");
				cache_table.GetAt( cache_table.FindIndex( ARPindex ) ).time = TIMEOFCOMPLETE;
			}
			
			AfxGetApp()->m_pMainWnd->PostMessageA(UM_ARPTABLEUPDATE);
		}
	}
	return TRUE; //op가 1,2가 아닐때.
}

int CARPLayer::SearchCacheTable( unsigned char ip[4] ){
	if(cache_table.GetCount() == 0){ //CacheTable 안에 아무것도 없을때
		return -1;
	}
	else{ //CacheTable 안에 무언가가 있으때.
		for(int i = 0; i< cache_table.GetCount(); i++){ //getCount를 사용하여 그 숫자만큼 for문을 돌려준다.
			if(memcmp(cache_table.GetAt(cache_table.FindIndex(i)).ipAddress, ip, 4) == 0 ){
				//memcmp를 사용하여 targetIP와 테이블안에있는것들 ipAddress를 확인하여 같은경우 ( 0일때 )
				return i; //그 인덱스를 리턴한다.
			}
		}
	}
	return -1; //cacheTable안에 없으면 -1을 리턴한다.
}

int		CARPLayer::getTableCount(){
	return cache_table.GetCount();
}

CString CARPLayer::checkTableState( int index, int state ){

	CString index_info;
	CacheTable& check = cache_table.GetAt(cache_table.FindIndex(index));
	int time = check.time;

	if( state == 0 ) // 타이머인경우
		check.time -= 1;

	if( time == TIMEOFDELETE ){
		// 해당 정보 삭제
		CString deleted("deleted");
		cache_table.RemoveAt(cache_table.FindIndex(index));
		return deleted;
	}else if ( time == TIMEOFIMCOMPLETE ){
		// 아직 못 받음 
		check.state.Format("%s","imcomplete");
	}

	index_info.Format("%d.%d.%d.%d  %02x:%02x:%02x:%02x:%02x:%02x  %s",
			check.ipAddress[0],
			check.ipAddress[1],
			check.ipAddress[2],
			check.ipAddress[3],
			check.macAddress[0],
			check.macAddress[1],
			check.macAddress[2],
			check.macAddress[3],
			check.macAddress[4],
			check.macAddress[5],
			check.state);

	return index_info;
}

void	CARPLayer::itemDelete( int index ){
	cache_table.RemoveAt( cache_table.FindIndex(index) );
}

void	CARPLayer::allDelete(){
	if( !cache_table.IsEmpty() ){
		cache_table.RemoveAll();
	}
}
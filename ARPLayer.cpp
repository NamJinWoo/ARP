// ARPLayer.cpp: implementation of the CARPLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "arp.h"
#include "ARPLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const DWORD CARPLayer::nRegArpSendMsg = ::RegisterWindowMessage( "ARP Send Message" ); //메세지 등록
const DWORD CARPLayer::nRegKillRestartTimerMsg = ::RegisterWindowMessage( "ARP Send Kill Timer "); //메세지등록


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

	int ARPindex = SearchCacheTable();
	//ARP Table안에 있는지 검사한다.함수를 돌리는데
	if(ARPindex != -1 && cache_table.GetAt(cache_table.FindIndex(ARPindex)).state  == "complete"){ /*테이블에 없으면( 이것은 음수로 설정한다.-1)*/
		/*cache table에 ip주소가 저장되어있고(ARPindex != -1),mac주소도 저장되어있고.....아직 timer가 지나지 않아서 아직 테이블 안에 있는..complete인..*/
		CString alreadyExist; //이미 있다는 메세지와 함께 맥주소를 보여주는 출력문 변수

		/*
		alreadyExist.Format("이미 해당 MAC 주소가 CacheTable에 존재합니다. Request를 종료합니다. \n%02x:%02x:%02x:%02x:%02x:%02x" ,
		cache_table.GetAt(cache_table.FindIndex(ARPindex)).macAddress[0],
		cache_table.GetAt(cache_table.FindIndex(ARPindex)).macAddress[1],
		cache_table.GetAt(cache_table.FindIndex(ARPindex)).macAddress[2],
		cache_table.GetAt(cache_table.FindIndex(ARPindex)).macAddress[3],
		cache_table.GetAt(cache_table.FindIndex(ARPindex)).macAddress[4],
		cache_table.GetAt(cache_table.FindIndex(ARPindex)).macAddress[5]);
		*/
		/*alreadyExist변수에 이미 존재한다는 메세지를 남기고, cahceTable의 mac 주소 를 하나하나씩 뽑아 출력해준다.*/
		//%02X : 16진수 대문자로. 2자리 헥사에서, 앞의 빈 칸 있으면 0으로 채움
		alreadyExist.Format("%02x.%02x.%02x.%02x  %02x:%02x:%02x:%02x:%02x:%02x  complete",
			cache_table.GetAt(cache_table.FindIndex(ARPindex)).ipAddress[0],
			cache_table.GetAt(cache_table.FindIndex(ARPindex)).ipAddress[1],
			cache_table.GetAt(cache_table.FindIndex(ARPindex)).ipAddress[2],
			cache_table.GetAt(cache_table.FindIndex(ARPindex)).ipAddress[3],
			cache_table.GetAt(cache_table.FindIndex(ARPindex)).macAddress[0],
			cache_table.GetAt(cache_table.FindIndex(ARPindex)).macAddress[1],
			cache_table.GetAt(cache_table.FindIndex(ARPindex)).macAddress[2],
			cache_table.GetAt(cache_table.FindIndex(ARPindex)).macAddress[3],
			cache_table.GetAt(cache_table.FindIndex(ARPindex)).macAddress[4],
			cache_table.GetAt(cache_table.FindIndex(ARPindex)).macAddress[5]);
		cache_table.
		return mp_UnderLayer -> Receive();

		
		//없으면 FF-FF-FF...으로 다보냄...
		//이더넷 주소를 브로드캐스트로 보냄.
	}
	else{
		//없으면 헤더에 있는 값들 설정해줌...
		m_sHeader.protType = htons(0x0800); //request니깐 0800로 설정한다.
		m_sHeader.hardType = htons(0x0001);
		m_sHeader.protSize = 4;
		m_sHeader.hardSize = 6;
		m_sHeader.op = htons(0x0001); //request 니깐 1로 설정.
		
		//이부분 이해 잘안됨....

		//ARP 헤더에 sender호스트의 맥주소 설정
         SendMessage(HWND_BROADCAST , nRegArpSendMsg ,0 , 0);//하위 레이어에 arpmessage를 broadcast로 보냄 
		 //PostMessage를 써야할지 고민....

         return GetUnderLayer()->Send((unsigned char*)&m_sHeader , sizeof(m_sHeader) , 0x01);//여기서 저장된 정보를 하위 레이어로 보냄
		 //ARP Request 전송
		//그 index를 가지고 전송.
	}
	

	

	//타이머(웨잇해라)
	return TRUE ;
}

// 받기
BOOL CARPLayer::Receive( unsigned char* ppayload )
{
	return TRUE ;
}

int CARPLayer::SearchCacheTable(){
	if(cache_table.GetCount() == 0){ //CacheTable 안에 아무것도 없을때
		return -1;
	}
	else{ //CacheTable 안에 무언가가 있으때.
		for(int i = 0; i< cache_table.GetCount(); i++){ //getCount를 사용하여 그 숫자만큼 for문을 돌려준다.
			if(memcmp(cache_table.GetAt(cache_table.FindIndex(i)).ipAddress, m_sHeader.targetIP, 4) == 0 ){
				//memcmp를 사용하여 targetIP와 테이블안에있는것들 ipAddress를 확인하여 같은경우 ( 0일때 )
				return i; //그 인덱스를 리턴한다.
			}
		}
	}
	return -1; //cacheTable안에 없으면 -1을 리턴한다.
}
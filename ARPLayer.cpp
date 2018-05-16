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

const DWORD CARPLayer::nRegArpSendMsg = ::RegisterWindowMessage( "ARP Send Message" ); //�޼��� ���
const DWORD CARPLayer::nRegKillRestartTimerMsg = ::RegisterWindowMessage( "ARP Send Kill Timer "); //�޼������


CARPLayer::CARPLayer( char* pName ) 
: CBaseLayer( pName ), 
  mp_Dlg( NULL )
{
	ResetHeader( ) ;
}

CARPLayer::~CARPLayer()
{
}


// ��� ����
void CARPLayer::ResetHeader( )
{
	memset(m_sHeader.senderMac, 0, 6);
	memset(m_sHeader.targetMac, 0, 6);
}

///////���� ARP Packet�� �����غ��ô� ���� /////////////

//�۽����� IP,MAC �ּҸ� �����Ѵ�.
//Source IPAddress�� ���� �մϴ�^^
void CARPLayer::setSrcIPAddress(unsigned char* pAddress)
{
	memcpy(m_sHeader.senderIP, pAddress, 4);
}
void CARPLayer::setSrcMacAddress(unsigned char* pAddress)
{
	memcpy(m_sHeader.senderMac, pAddress, 6);
}

//�������� IP,MAC�ּҸ� �����Ѵ�.
//Destination IPAddress�� ���� �մϴ�^^
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
	//ARP Table�ȿ� �ִ��� �˻��Ѵ�.�Լ��� �����µ�
	if(ARPindex != -1 && cache_table.GetAt(cache_table.FindIndex(ARPindex)).state  == "complete"){ /*���̺��� ������( �̰��� ������ �����Ѵ�.-1)*/
		/*cache table�� ip�ּҰ� ����Ǿ��ְ�(ARPindex != -1),mac�ּҵ� ����Ǿ��ְ�.....���� timer�� ������ �ʾƼ� ���� ���̺� �ȿ� �ִ�..complete��..*/
		CString alreadyExist; //�̹� �ִٴ� �޼����� �Բ� ���ּҸ� �����ִ� ��¹� ����

		/*
		alreadyExist.Format("�̹� �ش� MAC �ּҰ� CacheTable�� �����մϴ�. Request�� �����մϴ�. \n%02x:%02x:%02x:%02x:%02x:%02x" ,
		cache_table.GetAt(cache_table.FindIndex(ARPindex)).macAddress[0],
		cache_table.GetAt(cache_table.FindIndex(ARPindex)).macAddress[1],
		cache_table.GetAt(cache_table.FindIndex(ARPindex)).macAddress[2],
		cache_table.GetAt(cache_table.FindIndex(ARPindex)).macAddress[3],
		cache_table.GetAt(cache_table.FindIndex(ARPindex)).macAddress[4],
		cache_table.GetAt(cache_table.FindIndex(ARPindex)).macAddress[5]);
		*/
		/*alreadyExist������ �̹� �����Ѵٴ� �޼����� �����, cahceTable�� mac �ּ� �� �ϳ��ϳ��� �̾� ������ش�.*/
		//%02X : 16���� �빮�ڷ�. 2�ڸ� ���翡��, ���� �� ĭ ������ 0���� ä��
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

		
		//������ FF-FF-FF...���� �ٺ���...
		//�̴��� �ּҸ� ��ε�ĳ��Ʈ�� ����.
	}
	else{
		//������ ����� �ִ� ���� ��������...
		m_sHeader.protType = htons(0x0800); //request�ϱ� 0800�� �����Ѵ�.
		m_sHeader.hardType = htons(0x0001);
		m_sHeader.protSize = 4;
		m_sHeader.hardSize = 6;
		m_sHeader.op = htons(0x0001); //request �ϱ� 1�� ����.
		
		//�̺κ� ���� �߾ȵ�....

		//ARP ����� senderȣ��Ʈ�� ���ּ� ����
         SendMessage(HWND_BROADCAST , nRegArpSendMsg ,0 , 0);//���� ���̾ arpmessage�� broadcast�� ���� 
		 //PostMessage�� ������� ����....

         return GetUnderLayer()->Send((unsigned char*)&m_sHeader , sizeof(m_sHeader) , 0x01);//���⼭ ����� ������ ���� ���̾�� ����
		 //ARP Request ����
		//�� index�� ������ ����.
	}
	

	

	//Ÿ�̸�(�����ض�)
	return TRUE ;
}

// �ޱ�
BOOL CARPLayer::Receive( unsigned char* ppayload )
{
	return TRUE ;
}

int CARPLayer::SearchCacheTable(){
	if(cache_table.GetCount() == 0){ //CacheTable �ȿ� �ƹ��͵� ������
		return -1;
	}
	else{ //CacheTable �ȿ� ���𰡰� ������.
		for(int i = 0; i< cache_table.GetCount(); i++){ //getCount�� ����Ͽ� �� ���ڸ�ŭ for���� �����ش�.
			if(memcmp(cache_table.GetAt(cache_table.FindIndex(i)).ipAddress, m_sHeader.targetIP, 4) == 0 ){
				//memcmp�� ����Ͽ� targetIP�� ���̺��ȿ��ִ°͵� ipAddress�� Ȯ���Ͽ� ������� ( 0�϶� )
				return i; //�� �ε����� �����Ѵ�.
			}
		}
	}
	return -1; //cacheTable�ȿ� ������ -1�� �����Ѵ�.
}
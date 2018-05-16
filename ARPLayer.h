// ARPLayer.h: interface for the CARPLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARPLayer_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_)
#define AFX_ARPLayer_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"

class CARPLayer 
: public CBaseLayer  
{
private:
	inline void		ResetHeader( ) ;
	CObject* mp_Dlg ;

public:
	BOOL			Receive( unsigned char* ppayload );
	BOOL			Send( unsigned char* ppayload, int nlength );
	void			setSrcIPAddress(unsigned char* pAddress);
	void			setSrcMacAddress(unsigned char* pAddress);
	void			setDstIPAddress(unsigned char* pAddress);
	void			setDstMacAddress(unsigned char* pAddress);
	int				SearchCacheTable();
	CARPLayer( char* pName );
	virtual ~CARPLayer();

	typedef struct _ARPLayer_HEADER {
		unsigned char targetIP[4]; //������ IP
		unsigned char targetMac[6]; //������ Mac
		unsigned char senderIP[4]; //�۽��� IP
		unsigned char senderMac[6]; //�۽��� Mac
		unsigned char op; 
		unsigned short protSize; //Protocol ũ�� 2����Ʈ
		unsigned short hardSize; //�ϵ���� ũ�� 2����Ʈ
		unsigned char protType; //Protocol Ÿ��
		unsigned char hardType; //Protocol Ÿ��
		unsigned char frameType; // 0x0800���� 0x0806����
		unsigned char MacSrcAddress; 
		unsigned char MacDstAddress;

	} ARPLayer_HEADER, *LPARPLayer_HEADER ;

	typedef struct CacheTableStruct{
		unsigned char macAddress[6]; //mac�ּ� 6����Ʈ
		unsigned char ipAddress[4]; //ip�ּ� 4����Ʈ
		CString state; //complete���� �ƴ���
	} CacheTable , *LPCacheTable;

	CList<CacheTable , CacheTable&> cache_table;

	
   static const DWORD nRegArpSendMsg;
   static const DWORD nRegKillRestartTimerMsg;



protected:
	ARPLayer_HEADER	m_sHeader;
};

#endif // !defined(AFX_ARPLayer_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_)










// LayerManager.cpp: implementation of the CLayerManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "arp.h"
#include "LayerManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLayerManager::CLayerManager()
: m_nLayerCount( 0 ),
  mp_sListHead( NULL ),
  mp_sListTail( NULL ),
  m_nTop( -1 )
{

}

CLayerManager::~CLayerManager()
{

}
// 레이어 추가
void CLayerManager::AddLayer(CBaseLayer *pLayer)
{
	// 현재 레이어들
	mp_aLayers[ m_nLayerCount++ ] = pLayer ;
}
// 인덱스로 반환
CBaseLayer* CLayerManager::GetLayer(int nindex)
{
	return mp_aLayers[ nindex ] ;
}
// 저장한 id 값으로 반환
CBaseLayer* CLayerManager::GetLayer(char *pName)
{
	for ( int i = 0 ; i < m_nLayerCount ; i++ )
	{
		CString stName( pName ) ;
		if ( !stName.Compare( mp_aLayers[i]->GetLayerName( ) ) )
			return mp_aLayers[i] ;
	}

	return NULL ;
}

// 각 레이어 연결
void CLayerManager::ConnectLayers(char *pcList)
{
	MakeList( pcList ) ; // 들어온 문자열을 토큰 식으로 구성함
	LinkLayer( mp_sListHead ) ;
}

void CLayerManager::MakeList(char *pcList)
{
	while( pcList != (char*) 0x01 )
	{
		char sBuff[ 100 ] ;
		sscanf( pcList, "%s", sBuff ) ;  // 파라미터의 문자열 내에서 문자열 추출( 공백을 입력 끝으로 인식)
		pcList = strchr( pcList, ' ' ) + 1 ; // 공백을 토큰으로 사용

		PNODE pNode = AllocNode( sBuff ) ;
		AddNode( pNode ) ;
	}
}

CLayerManager::PNODE CLayerManager::AllocNode(char *pcName)
{
	PNODE node = new NODE ;
	ASSERT( node ) ;

	strcpy( node->token, pcName ) ;
	node->next = NULL ;

	return node ;
}

void CLayerManager::AddNode(PNODE pNode)
{
	if ( !mp_sListHead ) // 비어있는 경우
	{
		mp_sListHead = mp_sListTail = pNode ;
	}
	else
	{//뭔가 있는 경우
		mp_sListTail->next = pNode ;
		mp_sListTail = pNode ;
	}
}


void CLayerManager::Push(CBaseLayer *pLayer)
{
	if ( m_nTop >= MAX_LAYER_NUMBER )
	{
#ifdef _DEBUG
		TRACE( "The Stack is full.. so cannot run the push operation.. \n" ) ;
#endif
		return ;
	}

	mp_Stack[ ++m_nTop ] = pLayer ;
}

CBaseLayer* CLayerManager::Pop()
{
	if ( m_nTop < 0 )
	{
#ifdef _DEBUG
		TRACE( "The Stack is empty.. so cannot run the pop operation.. \n" ) ;
#endif
		return NULL ;
	}

	CBaseLayer* pLayer = mp_Stack[ m_nTop ] ;
	mp_Stack[ m_nTop ] = NULL ;
	m_nTop-- ;

	return pLayer ;
}

CBaseLayer* CLayerManager::Top()
{
	if ( m_nTop < 0 )
	{
#ifdef _DEBUG
		TRACE( "The Stack is empty.. so cannot run the top operation.. \n" ) ;
#endif
		return NULL ;
	}

	return mp_Stack[ m_nTop ] ;
}
// 계층 연결, 단 주어진 *,+,- 활용하니 가장 아래 계층인 File 부터 시작하여 File - Ethernet - ChatApp - ChatDlg 순으로 
void CLayerManager::LinkLayer(PNODE pNode)
{
	CBaseLayer* pLayer = NULL ;

	while ( pNode )
	{
		if ( !pLayer ) // 반복문 처음시
			pLayer = GetLayer( pNode->token ) ; 
		else
		{
			if ( *pNode->token == '(' ) // '(' 면 추가
				Push( pLayer ) ;
			else if ( *pNode->token == ')' ) // ')'면 제거
				Pop( ) ;
			else
			{
				char cMode = *pNode->token ;
				char* pcName = pNode->token + 1 ;
				
				pLayer = GetLayer( pcName ) ;

				switch( cMode )
				{
				case '*' : Top( )->SetUpperUnderLayer( pLayer ) ; break ; // 현재의 상위 계층을 파라미터로, 파라미터의 하위 계층을 현재로
				case '+' : Top( )->SetUpperLayer( pLayer ) ; break ;
				case '-' : Top( )->SetUnderLayer( pLayer ) ; break ;
				}
			}
		}

		pNode = pNode->next ;
	}
}	

void CLayerManager::DeAllocLayer()
{
	for ( int i = 0 ; i < this->m_nLayerCount ; i ++ )
		delete this->mp_aLayers[ i ] ;
}

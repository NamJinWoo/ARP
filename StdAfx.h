// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__119ECB1B_6E70_4662_A2A9_A20B5201CA81__INCLUDED_)
#define AFX_STDAFX_H__119ECB1B_6E70_4662_A2A9_A20B5201CA81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define MAX_LAYER_NUMBER		0xff
//{{AFX_INSERT_LOCATION}}
#define ETHER_MAX_SIZE			1514
#define ETHER_HEADER_SIZE		14
#define ETHER_MAX_DATA_SIZE		( ETHER_MAX_SIZE - ETHER_HEADER_SIZE )

#define TCP_HEADER_SIZE			( sizeof(unsigned int) * 2 +	\
								  sizeof(unsigned short) * 5 +	\
								  sizeof(unsigned char) * 6	)
#define TCP_DATA_SIZE			( ETHER_MAX_SIZE - ( TCP_HEADER_SIZE + \
													 IP_HEADER_SIZE + \
													 ETHER_HEADER_SIZE ) )
#define IP_HEADER_SIZE			( sizeof(unsigned short) * 4 +	\
								  sizeof(unsigned char) * 12 )
#define IP_DATA_SIZE			( ETHER_MAX_SIZE - ( IP_HEADER_SIZE + \
													 ETHER_HEADER_SIZE ) )

// NIC count
#define	NI_COUNT_NIC		10
#define TIMEOFCOMPLETE		10
#define TIMEOFIMCOMPLETE	0
#define TIMEOFDELETE		-5

// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__119ECB1B_6E70_4662_A2A9_A20B5201CA81__INCLUDED_)

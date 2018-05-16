// ARPAppDlg.h : header file
//

#if !defined(AFX_ARPAppDlg_H__FE9C37CB_4C90_4772_99AD_50A9B24BCE62__INCLUDED_)
#define AFX_ARPAppDlg_H__FE9C37CB_4C90_4772_99AD_50A9B24BCE62__INCLUDED_

#include "LayerManager.h"	// Added by ClassView
#include "ARPLayer.h"	// Added by ClassView
#include "IPLayer.h"		// Added by ClassView
#include "EthernetLayer.h"	// Added by ClassView
#include "NILayer.h"		// Added by ClassView



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CARPAppDlg dialog

class CARPAppDlg 
: public CDialog, 
  public CBaseLayer
{
// Construction
public:
	CARPAppDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CARPAppDlg)
	enum { IDD = IDD_arp_DIALOG };
	CListBox			m_ListChat;

	CString				m_unDstEthAddr;
	CIPAddressCtrl		m_unDstIPAddr;

	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CARPAppDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CARPAppDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);	
	
	afx_msg void OnBnClickedButtonItemdelete();
	afx_msg void OnBnClickedButtonAlldelete();
	afx_msg void OnBnClickedButtonSendarp();
	afx_msg void OnBnClickedButtonSendgratuitousarp();
	afx_msg void OnBnClickedButtonCancle();
	afx_msg void OnBnClickedButtonExit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	BOOL			Receive( unsigned char* ppayload );
	inline void		SendData( );

private:
	CLayerManager	m_LayerMgr;

	enum {			arp_INITIALIZING, 
					arp_SENDARP,
					arp_SENDGARP,
					arp_ERROR,
					arp_ADDR_SET,
					arp_ADDR_RESET } ;

	void			SetDlgState( int state );

	inline void		EndofProcess( );
	inline void		SetRegstryMessage( );

	LRESULT			OnUpdateDataFalse(WPARAM wParam, LPARAM lParam);

	BOOL			m_bSendReady;

// Object App
	CARPLayer*	m_ARP;
	CIPLayer*		m_IP;
	CEthernetLayer*	m_ETH;
	CNILayer*		m_NI;

// Implementation
	UINT			m_wParam;
	DWORD			m_lParam;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARPAppDlg_H__FE9C37CB_4C90_4772_99AD_50A9B24BCE62__INCLUDED_)

// PingInterfaceDlg.h : header file
//

#if !defined(AFX_PINGINTERFACEDLG_H__511161CB_87BC_4B69_A926_FCDBE68CEA6D__INCLUDED_)
#define AFX_PINGINTERFACEDLG_H__511161CB_87BC_4B69_A926_FCDBE68CEA6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPingInterfaceDlg dialog
#include "ping.h"

class CPingInterfaceDlg : public CDialog
{
// Construction
public:
	CPingInterfaceDlg(CWnd* pParent = NULL);	// standard constructor
	friend UINT startScanLanHost(LPVOID param);
    friend UINT showOnLineHosts(LPVOID param);
// Dialog Data
	//{{AFX_DATA(CPingInterfaceDlg)
	enum { IDD = IDD_PINGINTERFACE_DIALOG };
	CListBox	m_onLineHostInfo;
	CString	m_startIp;
	CString	m_endIp;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPingInterfaceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPingInterfaceDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnStartScan();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PINGINTERFACEDLG_H__511161CB_87BC_4B69_A926_FCDBE68CEA6D__INCLUDED_)

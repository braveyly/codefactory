// PingInterfaceDlg.cpp : implementation file
//
#include <Afxmt.h>
#include "stdafx.h"
#include "PingInterface.h"
#include "PingInterfaceDlg.h"
#include "comDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

vector<OnLineHost> hostList;
CEvent eventUpdateView;
int finishFlag;
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPingInterfaceDlg dialog

CPingInterfaceDlg::CPingInterfaceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPingInterfaceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPingInterfaceDlg)
	m_startIp = _T("");
	m_endIp = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPingInterfaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPingInterfaceDlg)
	DDX_Control(pDX, IDC_LIST1, m_onLineHostInfo);
	DDX_Text(pDX, IDC_EDIT1, m_startIp);
	DDV_MaxChars(pDX, m_startIp, 100);
	DDX_Text(pDX, IDC_EDIT2, m_endIp);
	DDV_MaxChars(pDX, m_endIp, 100);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPingInterfaceDlg, CDialog)
	//{{AFX_MSG_MAP(CPingInterfaceDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnStartScan)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPingInterfaceDlg message handlers

BOOL CPingInterfaceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPingInterfaceDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPingInterfaceDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPingInterfaceDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CPingInterfaceDlg::OnOK() 
{
	// TODO: Add extra validation here

    CDialog::OnOK();

}

extern CEvent eventUpdateView;
UINT startScanLanHost(LPVOID param)
{
    CPingInterfaceDlg *pDlgHandle = ( CPingInterfaceDlg *) param;
	char* argv[10];
   	int argc;
    
	if( TRUE == pDlgHandle->m_startIp.IsEmpty() || TRUE == pDlgHandle->m_endIp.IsEmpty() )
    {
        argc = 1;
        demo_main(argc,(char**)&argv);
	}
	else
	{
		LPCTSTR pstartIp = pDlgHandle->m_startIp;
    	LPCTSTR pendIp = pDlgHandle->m_endIp;
       	argc = 3;
    	argv[0]=NULL;
    	argv[1]=(char*)pstartIp;
    	argv[2]=(char*)pendIp;
    	demo_main(argc,(char**)&argv);
	}

	return 1;
}

UINT showOnLineHosts(LPVOID param)
{
   CPingInterfaceDlg *pDlgHandle = ( CPingInterfaceDlg *) param;
   while(1)
    {
	   WaitForSingleObject( eventUpdateView, INFINITE );
	   pDlgHandle->m_onLineHostInfo.ResetContent();
       CString oneHostRecord;
	   for( vector<OnLineHost>::size_type ix = 0 ; ix != hostList.size(); ++ix )
	   {
          oneHostRecord.Format( _T("%20s %5d %5d\n"), hostList[ix].ip, hostList[ix].icmp_seq, hostList[ix].time );
	      pDlgHandle->m_onLineHostInfo.AddString(oneHostRecord);
	   }
       
	   if( finishFlag == SCAN_LAN_HOST_FINISH )
	   {
	      oneHostRecord.Format("Find %u online host(s) totally", hostList.size() );
	      AfxMessageBox(oneHostRecord);
		  break;
	   }
	 }
   return 1;
}
void CPingInterfaceDlg::OnStartScan() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	
    AfxBeginThread(startScanLanHost,this);
	AfxBeginThread(showOnLineHosts,this);
    finishFlag = SCAN_LAN_HOST_START;
}

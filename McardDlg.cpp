//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 2002-2004 SCM Microsystems, Inc.
//
//==========================================================================;


// McardDlg.cpp : implementation file
//
#include "stdinc.h"
#include "stdafx.h"
#include <initguid.h>
#include <dbt.h>
#include <winsvc.h>
#include <tlhelp32.h>

#include "winscard.h"
#include "winsmcrd.h"
#include "hexeditctrl.h"
#include "Mcard.h"
#include "McardDlg.h"
#include "MCardAPI.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HMODULE mhDLL = NULL;
SCARDCONTEXT	ContextHandle;
SCARDHANDLE		CardHandle;
DWORD			ActiveProtocol;
char			ReaderInFocus [100];
BYTE			byCardInUse;
BOOL			bAppClosed;

BOOL			bConnected;
BOOL			bSecurityFeature;
BOOL			bVerified;

BOOL			bStatus = FALSE;
LONG			lStatus = MCARD_S_SUCCESS;
MCARDCONTEXT	hMCardContext = 0;
int				nSelectedAttribute = 0;


char aszCardNames [][100] = \
{ 
  "UNKNOWN",
  "SLE 4406","SLE 4418","SLE 4428","SLE 4432","SLE 4436/SLE 5536","SLE 4442","SLE 4436/5536",
  "AT24C01ASC", "AT24C02SC",  "AT24C04SC",  "AT24C08SC",  "AT24C16SC", "AT24C32SC",
  "AT24C64SC",  "AT24C128SC", "AT24C256SC", "AT24C512SC", "AT88SC153",  "AT88SC1608"
};


char aszProtocolNames [][100] = {"UNKNOWN", "2 WIRE", "3 WIRE", "IIC", "EXT_IIC",  "AT88_IIC", "SLE4406"};


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
// CMcardDlg dialog

CMcardDlg::CMcardDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMcardDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMcardDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMcardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMcardDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
		DDX_Control(pDX, IDC_MEMBUFFER, m_Edit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMcardDlg, CDialog)
	//{{AFX_MSG_MAP(CMcardDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(Exit, OnExit)
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_CBN_SELCHANGE(IDC_READER_NAMES, OnSelchangeReaderNames)
	ON_BN_CLICKED(IDC_GETATTRIB, OnGetattrib)
	ON_BN_CLICKED(IDC_DISCONNECT, OnDisconnect)
	ON_BN_CLICKED(IDC_READMEM, OnReadmem)
	ON_BN_CLICKED(IDC_SETATTRIB, OnSetattrib)
	ON_BN_CLICKED(IDC_PROTECTREAD, OnProtectread)
	ON_BN_CLICKED(IDC_WRITEMEM, OnWritemem)
	ON_BN_CLICKED(IDC_PROTECTWRITE, OnProtectwrite)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_VERIFYPIN, OnVerifypin)
	ON_BN_CLICKED(IDC_CHANGEPIN, OnChangepin)
	ON_BN_CLICKED(IDC_GETRESPONSE, OnGetresponse)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_SHUTDOWN, OnShutdown)
	ON_BN_CLICKED(IDC_MEMINIT, OnMeminit)
	ON_CBN_EDITCHANGE(IDC_ATTRS, OnEditchangeAttrs)
	ON_WM_CTLCOLOR ()
	ON_CBN_SELCHANGE(IDC_ATTRS, OnSelchangeAttrs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMcardDlg message handlers



// DLL function Pointers
pfnMCardInitialize					MCardInitialize;
pfnMCardShutdown					MCardShutdown;
pfnMCardConnect						MCardConnect;
pfnMCardDisconnect					MCardDisconnect;
pfnMCardGetAttrib					MCardGetAttrib;
pfnMCardSetAttrib					MCardSetAttrib;
pfnMCardReadMemory					MCardReadMemory;
pfnMCardWriteMemory					MCardWriteMemory;
pfnMCardSetMemoryWriteProtection	MCardSetMemoryWriteProtection;
pfnMCardSetMemoryReadProtection		MCardSetMemoryReadProtection;
pfnMCardReadMemoryStatus			MCardReadMemoryStatus;
pfnMCardVerifyPIN					MCardVerifyPIN;
pfnMCardChangePIN					MCardChangePIN;
pfnMCardChallengeResponse			MCardChallengeResponse;
pfnMCardDeductCounter				MCardDeductCounter;
pfnMCardWaitForCardState			MCardWaitForCardState;


BOOL CMcardDlg::OnInitDialog()
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

	// Fill the reader list
	
	FillReaderList ();
	LoadMcardDLL ();
	InitializeProcAddresses ();

	((CButton*) GetDlgItem (IDC_AUTODETECT))->SetCheck (1);
	SetDlgItemText (IDC_MEMZONE, "0");
	bConnected			=	FALSE;
	bSecurityFeature	=	FALSE;
	bVerified			=	FALSE;
	

	((CButton *) GetDlgItem (IDC_GETATTRIB))->EnableWindow (FALSE);
	((CButton *) GetDlgItem (IDC_SETATTRIB))->EnableWindow (FALSE);
	((CButton *) GetDlgItem (IDC_VERIFYPIN))->EnableWindow (FALSE);
	((CButton *) GetDlgItem (IDC_CHANGEPIN))->EnableWindow (FALSE);
	((CButton *) GetDlgItem (IDC_DISCONNECT))->EnableWindow (FALSE);
	((CButton *) GetDlgItem (IDC_READMEM))->EnableWindow (FALSE);
	((CButton *) GetDlgItem (IDC_WRITEMEM))->EnableWindow (FALSE);
	((CButton *) GetDlgItem (IDC_PROTECTREAD))->EnableWindow (FALSE);
	((CButton *) GetDlgItem (IDC_PROTECTWRITE))->EnableWindow (FALSE);
	
	
	m_Edit.SetOptions(TRUE, TRUE, TRUE, TRUE); 
	m_Edit.SetBPR (8);

	
	CComboBox *pbox1 = (CComboBox*)GetDlgItem(IDC_ATTRS);
	pbox1->SetCurSel (0);
	byCardInUse = 0x00;
	bAppClosed = FALSE;

	//AfxBeginThread (MyThreadProc, (LPVOID) this);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMcardDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMcardDlg::OnPaint() 
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
HCURSOR CMcardDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMcardDlg::LoadMcardDLL ()
{
	BOOL bRetVal;
	WIN32_FIND_DATA FileData;  
	HANDLE hFind;	

	do 
	{

		bRetVal = TRUE;		

		hFind = FindFirstFile("MC*.DLL", &FileData);
		if (INVALID_HANDLE_VALUE == hFind) 
		{
			AfxMessageBox ("The Memory Card DLL Could not be found.\nMake sure that it is in the same path as the application");
			exit (1);
			break;			
		} 

		mhDLL = LoadLibrary (FileData.cFileName);

		if (NULL == mhDLL) 
		{
			DWORD dwError = GetLastError ();
			// Dll cannot be opened or is not valid
			SetDlgItemText (IDC_STATUS, "The Memory Card DLL could not be loaded.\nLoadLibrary failed.");
			bStatus = FALSE;
			break;
		}

		SetDlgItemText (IDC_STATUS, "The Memory Card DLL loaded successfully.");		
		bStatus = TRUE;

		
	} while (FALSE);

	if (hFind != INVALID_HANDLE_VALUE) {

		FindClose(hFind); 
	}
	
}

void CMcardDlg::InitializeProcAddresses ()
{
	LPCSTR lpcszProcName;

	do
	{
		
		lpcszProcName = "MCardInitialize";
		MCardInitialize = (pfnMCardInitialize)GetProcAddress (mhDLL, lpcszProcName);

		if (NULL == MCardInitialize)
		{
			break;
		}

		
		lpcszProcName = "MCardShutdown";
		MCardShutdown = (pfnMCardShutdown)GetProcAddress (mhDLL, lpcszProcName);

		if (NULL == MCardShutdown)
		{
			break;
		}

		lpcszProcName = "MCardConnect";
		MCardConnect = (pfnMCardConnect)GetProcAddress (mhDLL, lpcszProcName);

		if (NULL == MCardConnect)
		{
			break;
		}

		lpcszProcName = "MCardDisconnect";
		MCardDisconnect = (pfnMCardDisconnect)GetProcAddress (mhDLL, lpcszProcName);

		if (NULL == MCardDisconnect)
		{
			break;
		}


		lpcszProcName = "MCardGetAttrib";
		MCardGetAttrib = (pfnMCardGetAttrib)GetProcAddress (mhDLL, lpcszProcName);

		if (NULL == MCardGetAttrib)
		{
			break;
		}

		lpcszProcName = "MCardSetAttrib";
		MCardSetAttrib = (pfnMCardSetAttrib)GetProcAddress (mhDLL, lpcszProcName);

		if (NULL == MCardSetAttrib)
		{
			break;
		}


		lpcszProcName = "MCardReadMemory";
		MCardReadMemory = (pfnMCardReadMemory)GetProcAddress (mhDLL, lpcszProcName);

		if (NULL == MCardReadMemory)
		{
			break;
		}


		lpcszProcName = "MCardWriteMemory";
		MCardWriteMemory = (pfnMCardWriteMemory)GetProcAddress (mhDLL, lpcszProcName);

		if (NULL == MCardWriteMemory)
		{
			break;
		}


		lpcszProcName = "MCardSetMemoryWriteProtection";
		MCardSetMemoryWriteProtection = (pfnMCardSetMemoryWriteProtection)GetProcAddress (mhDLL, lpcszProcName);

		if (NULL == MCardSetMemoryWriteProtection)
		{
			break;
		}


		lpcszProcName = "MCardSetMemoryReadProtection";
		MCardSetMemoryReadProtection = (pfnMCardSetMemoryReadProtection)GetProcAddress (mhDLL, lpcszProcName);

		if (NULL == MCardSetMemoryReadProtection)
		{
			break;
		}


		lpcszProcName = "MCardReadMemoryStatus";
		MCardReadMemoryStatus = (pfnMCardReadMemoryStatus)GetProcAddress (mhDLL, lpcszProcName);

		if (NULL == MCardReadMemoryStatus)
		{
			break;
		}


		lpcszProcName = "MCardVerifyPIN";
		MCardVerifyPIN = (pfnMCardVerifyPIN)GetProcAddress (mhDLL, lpcszProcName);

		if (NULL == MCardVerifyPIN)
		{
			break;
		}


		lpcszProcName = "MCardChangePIN";
		MCardChangePIN = (pfnMCardChangePIN)GetProcAddress (mhDLL, lpcszProcName);

		if (NULL == MCardChangePIN)
		{
			break;
		}


		lpcszProcName = "MCardChallengeResponse";
		MCardChallengeResponse = (pfnMCardChallengeResponse)GetProcAddress (mhDLL, lpcszProcName);

		if (NULL == MCardChallengeResponse)
		{
			break;
		}

		lpcszProcName = "MCardDeductCounter";
		MCardDeductCounter = (pfnMCardDeductCounter)GetProcAddress (mhDLL, lpcszProcName);

		if (NULL == MCardDeductCounter)
		{
			break;
		}

		lpcszProcName = "MCardWaitForCardState";
		MCardWaitForCardState = (pfnMCardWaitForCardState)GetProcAddress (mhDLL, lpcszProcName);

		if (NULL == MCardWaitForCardState)
		{
			break;
		}
	}
	while (FALSE);
	
}

void CMcardDlg::FillReaderList ()
{
	do
	{
		CString MessageBuffer;
		short nReaderCount = 0;
		long ret = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &ContextHandle);
		
		if (ret != SCARD_S_SUCCESS) 
		{
			MessageBuffer.Format("Function SCardEstablishContext returned 0x%X error code.", ret);
			AfxMessageBox((LPCTSTR) MessageBuffer, MB_OK| MB_ICONSTOP);
		}

		DWORD len = 1000;
		char szSelectedReaderName [100];		
		PBYTE	pbyReaderNames;
		pbyReaderNames = (PBYTE) malloc( 1000 );
		ASSERT( pbyReaderNames );

		ret = SCardListReaders(ContextHandle, 0, (LPTSTR) pbyReaderNames, &len);

		if (ret != SCARD_S_SUCCESS) 
		{
			MessageBuffer.Format("Probably no readers are connected.\nPlease make sure that the reader is connected properly", ret);
			AfxMessageBox((LPCTSTR) MessageBuffer, MB_OK| MB_ICONSTOP);
			free(pbyReaderNames);
			exit (1);
			break; 
		}
		else 
		{
			unsigned int StringLen = 0;
			while ( len > StringLen+1)
			{
				strcpy(szSelectedReaderName, (LPCTSTR) pbyReaderNames+StringLen);
				ret = SCardConnect(ContextHandle, szSelectedReaderName, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &CardHandle, &ActiveProtocol);
				if (ret != SCARD_E_UNKNOWN_READER)
				{	
					nReaderCount++;
					CComboBox *pbox1 = (CComboBox*)GetDlgItem(IDC_READER_NAMES);
					pbox1->InsertString (0, szSelectedReaderName);

					if (NULL != strstr(szSelectedReaderName, "CCID"))
					{
						pbox1->SetCurSel (nReaderCount - 1);
						pbox1->GetLBText  (pbox1->GetCurSel (), ReaderInFocus);
					}
				}
				if (ret == SCARD_S_SUCCESS)
				{
					SCardDisconnect(CardHandle, SCARD_UNPOWER_CARD );
				}
				StringLen += strlen((LPCTSTR) pbyReaderNames+StringLen+1);
				StringLen += 2;
			}
		}

		free(pbyReaderNames);

		if (0 == nReaderCount)
		{
			MessageBox("No Readers is available for use with the resource manager!", "MCTEST", MB_ICONSTOP);
			break;
		}
		else
		{
			CComboBox *pbox1 = (CComboBox*)GetDlgItem(IDC_READER_NAMES);
			pbox1->SetCurSel (0);
			pbox1->GetLBText  (0, ReaderInFocus);
		}
	}
	while (FALSE);

}


void CMcardDlg::OnSelchangeReaderNames() 
{
	int nSelectedReader;
	CComboBox *pbox1 = (CComboBox*)GetDlgItem(IDC_READER_NAMES);
	nSelectedReader = pbox1->GetCurSel ();
	pbox1->GetLBText  (nSelectedReader, ReaderInFocus);
}


void CMcardDlg::OnExit() 
{
	bAppClosed = TRUE;
	EndDialog (1);
}


void CMcardDlg::OnConnect() 
{
	// The Memory card is to be connected to the reader
	// First we get a dummy CardHandle by ScardConnect using Direct.
	// This is done just to get the card handle which will be later used by
	// the ScardConnect function to set the reader mode.
	
	CardHandle = 0;
	ActiveProtocol = 0;
	BYTE byCardType = 0;
	char szCardType[20];
	

	do
	{
		GetDlgItemText (IDC_CARDTYPE, szCardType, 20);
		sscanf (szCardType, "%x", &byCardType); 
		
		if (MCARD_S_SUCCESS == (lStatus = MCardConnect (hMCardContext, (((CButton*) GetDlgItem(IDC_AUTODETECT))->GetCheck ()), byCardType, &CardHandle)))
		{
			SetDlgItemText (IDC_STATUS, "Memory Card connected successfully.");
			bStatus = TRUE;
			((CButton *) GetDlgItem (IDC_GETATTRIB))->EnableWindow (TRUE);
			((CButton *) GetDlgItem (IDC_SETATTRIB))->EnableWindow (TRUE);
			((CButton *) GetDlgItem (IDC_DISCONNECT))->EnableWindow (TRUE);
			((CButton *) GetDlgItem (IDC_READMEM))->EnableWindow (TRUE);
			((CButton *) GetDlgItem (IDC_WRITEMEM))->EnableWindow (TRUE);
			((CButton *) GetDlgItem (IDC_PROTECTWRITE))->EnableWindow (TRUE);
			((CButton *) GetDlgItem (IDC_VERIFYPIN))->EnableWindow (TRUE);
			((CButton *) GetDlgItem (IDC_CHANGEPIN))->EnableWindow (TRUE);
			OnGetattrib ();
		}
		else
		{
			PutErrorCode (lStatus);
			bStatus = FALSE;
			((CButton *) GetDlgItem (IDC_GETATTRIB))->EnableWindow (FALSE);
			((CButton *) GetDlgItem (IDC_SETATTRIB))->EnableWindow (FALSE);
			((CButton *) GetDlgItem (IDC_VERIFYPIN))->EnableWindow (FALSE);
			((CButton *) GetDlgItem (IDC_CHANGEPIN))->EnableWindow (FALSE);
			((CButton *) GetDlgItem (IDC_DISCONNECT))->EnableWindow (FALSE);
			((CButton *) GetDlgItem (IDC_READMEM))->EnableWindow (FALSE);
			((CButton *) GetDlgItem (IDC_WRITEMEM))->EnableWindow (FALSE);
			((CButton *) GetDlgItem (IDC_PROTECTWRITE))->EnableWindow (FALSE);
		}
	}
	while (FALSE);

}

void CMcardDlg::OnDisconnect() 
{

	if (MCARD_S_SUCCESS == (lStatus = MCardDisconnect (CardHandle, SCARD_UNPOWER_CARD)))
	{
		SetDlgItemText (IDC_STATUS, "Memory Card disconnected.");
		bStatus = TRUE;
	}
	else
	{
		PutErrorCode (lStatus);
		bStatus = FALSE;
	}

	byCardInUse = 0x00;
	
	SetDlgItemText (IDC_MEMBUFFER, "");
	SetDlgItemText (IDC_CRBUFFER, "");
	SetDlgItemText (IDC_MEMOFFSET, "");
	SetDlgItemText (IDC_MEMLEN, "");
	SetDlgItemText (IDC_CHALLENGELEN, "");
	SetDlgItemText (IDC_CHALLENGEID, "");
	SetDlgItemText (IDC_CURPIN, "");
	SetDlgItemText (IDC_CURPINLEN, "");
	SetDlgItemText (IDC_BITORDER, "");
	SetDlgItemText (IDC_NEWPIN, "");
	SetDlgItemText (IDC_NEWPINLEN, "");
	SetDlgItemText (IDC_PROTOCOL, "");
	SetDlgItemText (IDC_TYPE, "");
	SetDlgItemText (IDC_ZONES, "");
	SetDlgItemText (IDC_PINS, "");
	SetDlgItemText (IDC_CLOCKRATE, "");
	SetDlgItemText (IDC_PAGESIZE, "");

	((CButton *) GetDlgItem (IDC_GETATTRIB))->EnableWindow (FALSE);
	((CButton *) GetDlgItem (IDC_SETATTRIB))->EnableWindow (FALSE);
	((CButton *) GetDlgItem (IDC_VERIFYPIN))->EnableWindow (FALSE);
	((CButton *) GetDlgItem (IDC_CHANGEPIN))->EnableWindow (FALSE);
	((CButton *) GetDlgItem (IDC_DISCONNECT))->EnableWindow (FALSE);
	((CButton *) GetDlgItem (IDC_READMEM))->EnableWindow (FALSE);
	((CButton *) GetDlgItem (IDC_WRITEMEM))->EnableWindow (FALSE);
	((CButton *) GetDlgItem (IDC_PROTECTREAD))->EnableWindow (FALSE);
	((CButton *) GetDlgItem (IDC_PROTECTWRITE))->EnableWindow (FALSE);
}



void CMcardDlg::OnGetattrib() 
{
	do
	{
		BYTE	abyAttr [100];
		DWORD	dwLen;
		char szDispBuffer [100];
		DWORD i;
		MCARD_FEATURES MCardFeatures;
		MCARD_MEMORY MCardMemory[10];
		MCARD_PIN MCardPIN [20];
		MCARD_CR MCardCR [5];
		MCARD_COUNTER MCardCounter[2];
		
		// Get the Type of memory card
		if (MCARD_S_SUCCESS != MCardGetAttrib (
			CardHandle,
			MCARD_ATTR_TYPE,
			abyAttr,
			&dwLen
			))
		{
			SetDlgItemText (IDC_STATUS, "Error in retrieving Card type.");
			bStatus = FALSE;
			break;
		}
		byCardInUse = abyAttr[0];

		SetDlgItemText (IDC_TYPE, aszCardNames [abyAttr[0]]);

		// Get the Protocol of the memory card
		if (MCARD_S_SUCCESS != MCardGetAttrib (
			CardHandle,
			MCARD_ATTR_PROTOCOL,
			abyAttr,
			&dwLen
			))
		{
			SetDlgItemText (IDC_STATUS, "Error in retrieving Protocol.");
			bStatus = FALSE;
			break;
		}

		SetDlgItemText (IDC_PROTOCOL, (const char *) aszProtocolNames[abyAttr[0]]);

		
		// Get the memory card general features
		if (MCARD_S_SUCCESS != MCardGetAttrib (
			CardHandle,
			MCARD_ATTR_FEATURES,
			(unsigned char *) &MCardFeatures,
			&dwLen
			))
		{
			SetDlgItemText (IDC_STATUS, "Error in retrieving Protocol.");
			bStatus = FALSE;
			break;
		}
		itoa (MCardFeatures.byMemoryZones, szDispBuffer, 10);
		SetDlgItemText (IDC_ZONES, szDispBuffer);

		itoa (MCardFeatures.byPINs, szDispBuffer, 10);
		SetDlgItemText (IDC_PINS, szDispBuffer);

		itoa (MCardFeatures.byCounters, szDispBuffer, 10);
		SetDlgItemText (IDC_COUNTERS, szDispBuffer);

		itoa (MCardFeatures.byCRs, szDispBuffer, 10);
		SetDlgItemText (IDC_CRS, szDispBuffer);


		// Get the clock rate
		if (MCARD_S_SUCCESS != MCardGetAttrib (
			CardHandle,
			MCARD_ATTR_CLOCK,
			abyAttr,
			&dwLen
			))
		{
			SetDlgItemText (IDC_STATUS, "Error in retrieving clock rate.");
			bStatus = FALSE;
			break;
		}

		itoa (abyAttr[0], szDispBuffer, 16);
		SetDlgItemText (IDC_CLOCKRATE, szDispBuffer);


		// Get the Bit Order
		if (MCARD_S_SUCCESS != MCardGetAttrib (
			CardHandle,
			MCARD_ATTR_BIT_ORDER,
			abyAttr,
			&dwLen
			))
		{
			SetDlgItemText (IDC_STATUS, "Error in retrieving bit order.");
			bStatus = FALSE;
			break;
		}

		if (LSB == abyAttr[0])
		{
			SetDlgItemText (IDC_BITORDER, "LSB");
		}
		else if (MSB == abyAttr[0])
		{
			SetDlgItemText (IDC_BITORDER, "MSB");
		}
		else
		{
			SetDlgItemText (IDC_BITORDER, "NotSet");
		}

		switch (nSelectedAttribute)
		{
		case 0:
			// Get the Memory Zones
			if (MCARD_S_SUCCESS != MCardGetAttrib (
					CardHandle,
					MCARD_ATTR_MEMORY,
					(unsigned char *) &MCardMemory,
					&dwLen
					))
			{
				SetDlgItemText (IDC_STATUS, "Error in retrieving memory organization.");
				bStatus = FALSE;
				break;
			}

			char szTempBuffer [30];
			strcpy (szDispBuffer, "");
			for (i = 0; i < MCardFeatures.byMemoryZones; i++)
			{
				ultoa (MCardMemory[i].dwSize , szTempBuffer, 16);
				strcat (szDispBuffer, szTempBuffer);
				strcat (szDispBuffer, "  ");
			}
			SetDlgItemText (IDC_DETAILS, szDispBuffer);
			break;

		case 1:
			// Get the PINS
			if (MCardFeatures.byPINs)
			{
				if (MCARD_S_SUCCESS != MCardGetAttrib (
						CardHandle,
						MCARD_ATTR_PIN,
						(unsigned char *) &MCardPIN,
						&dwLen
						))
				{
					SetDlgItemText (IDC_STATUS, "Error in retrieving PIN organization.");
					bStatus = FALSE;
					break;
				}

				strcpy (szDispBuffer, "");
				for (i = 0; i < MCardFeatures.byPINs; i++)
				{
					ultoa (MCardPIN[i].bySize , szTempBuffer, 16);
					strcat (szDispBuffer, szTempBuffer);
					strcat (szDispBuffer, "  ");
					ultoa (MCardPIN[i].byRetries , szTempBuffer, 16);
					strcat (szDispBuffer, szTempBuffer);
					strcat (szDispBuffer, "  ");
				}
				SetDlgItemText (IDC_DETAILS, szDispBuffer);
			}
			break;

		case 2:
			// Get the Challenge Response details
			
			if (MCardFeatures.byCRs)
			{
				if (MCARD_S_SUCCESS != MCardGetAttrib (
						CardHandle,
						MCARD_ATTR_CR,
						(unsigned char *) &MCardCR,
						&dwLen
						))
				{
					SetDlgItemText (IDC_STATUS, "Error in retrieving memory organization.");
					bStatus = FALSE;
					break;
				}

				strcpy (szDispBuffer, "");
				for (i = 0; i < MCardFeatures.byCRs; i++)
				{
					ultoa (MCardCR[i].dwChallengeLen , szTempBuffer, 16);
					strcat (szDispBuffer, szTempBuffer);
					strcat (szDispBuffer, "  ");
					ultoa (MCardCR[i].dwResponseLen , szTempBuffer, 16);
					strcat (szDispBuffer, szTempBuffer);
					strcat (szDispBuffer, "  ");
					ultoa (MCardCR[i].byRetries , szTempBuffer, 16);
					strcat (szDispBuffer, szTempBuffer);
					strcat (szDispBuffer, "  ");
				}
				SetDlgItemText (IDC_DETAILS, szDispBuffer);
			}
			break;

		case 3:
			// Get the Counter details

			if (MCardFeatures.byCounters)
			{
				if (MCARD_S_SUCCESS != MCardGetAttrib (
						CardHandle,
						MCARD_ATTR_COUNTERS,
						(unsigned char *) &MCardCounter,
						&dwLen
						))
				{
					SetDlgItemText (IDC_STATUS, "Error in retrieving memory organization.");
					bStatus = FALSE;
					break;
				}

				strcpy (szDispBuffer, "");
				for (i = 0; i < MCardFeatures.byCounters; i++)
				{
					ultoa (MCardCounter[i].dwSize , szTempBuffer, 16);
					strcat (szDispBuffer, szTempBuffer);
					strcat (szDispBuffer, "  ");

					ultoa (MCardCounter[i].dwUnits , szTempBuffer, 16);
					strcat (szDispBuffer, szTempBuffer);
					strcat (szDispBuffer, "  ");
				}
				SetDlgItemText (IDC_DETAILS, szDispBuffer);
			}
			break;
		}

		SetDlgItemText (IDC_STATUS, "All attributes successfully got.");
		bStatus = TRUE;
	}
	while (FALSE);

}


void CMcardDlg::OnSetattrib() 
{
	BYTE	abyAttr [100];
	char szTempBuffer [10];

	do
	{
		GetDlgItemText (IDC_BITORDER, szTempBuffer, 10);

		if (0 == stricmp(szTempBuffer, "LSB"))
		{
			abyAttr [0] = LSB;
		}
		else if (0 == stricmp(szTempBuffer, "MSB"))
		{
			abyAttr [0] = MSB;
		}
		else if (0 == stricmp(szTempBuffer, "NOTSET"))
		{
			abyAttr [0] = DEFAULT;
		}
		else
		{
			SetDlgItemText (IDC_STATUS, "Invalid Bit order.");
			bStatus = FALSE;
			break;
		}


		if (MCARD_S_SUCCESS != MCardSetAttrib(
		    CardHandle,
			MCARD_ATTR_BIT_ORDER,
			abyAttr,
			1
			))
		{
			SetDlgItemText (IDC_STATUS, "Config Info Set : Failed setting bit order.");	
			bStatus = FALSE;
			break;
		}

		GetDlgItemText (IDC_CLOCKRATE, szTempBuffer, 10);

		if (0 == strcmp (szTempBuffer, ""))
		{
			SetDlgItemText (IDC_STATUS, "Please enter the clock rate.");
			bStatus = FALSE;
			break;
		}
		sscanf (szTempBuffer, "%x", &abyAttr[0]);
		if (MCARD_S_SUCCESS != MCardSetAttrib(
			CardHandle,
			MCARD_ATTR_CLOCK,
			abyAttr,
			1
			))
		{
			SetDlgItemText (IDC_STATUS, "Config Info and bit order set : Failed in setting Clock rate.");	
			bStatus = FALSE;
			break;
		}

		SetDlgItemText (IDC_STATUS, "All Attributes successfully set. ");
		bStatus = TRUE;
	}
	while (FALSE);

}



void CMcardDlg::OnReadmem() 
{

	do
	{
		BYTE	byMemZone;
		DWORD	dwOffset;
		DWORD	dwLen;
		LPBYTE	pbyData;
		LPBYTE	pbyStatus;
		char	szTemp[10];
		
		GetDlgItemText (IDC_MEMZONE, szTemp, 10);
		sscanf (szTemp, "%x", &byMemZone);

		if (0 == strcmp (szTemp,""))
		{
			SetDlgItemText (IDC_STATUS, "Please enter the memory zone.");
			bStatus = FALSE;
			break;
		}

		GetDlgItemText (IDC_MEMLEN, szTemp, 10);
		sscanf (szTemp, "%x", &dwLen);

		if (0 == strcmp (szTemp,""))
		{
			SetDlgItemText (IDC_STATUS, "Please enter the memory length.");
			bStatus = FALSE;
			break;
		}

		GetDlgItemText (IDC_MEMOFFSET, szTemp, 10);
		sscanf (szTemp, "%x", &dwOffset);

		if (0 == strcmp (szTemp,""))
		{
			SetDlgItemText (IDC_STATUS, "Please enter the memory offset.");
			bStatus = FALSE;
			break;
		}
		
		
		pbyData = (LPBYTE) malloc ((sizeof (BYTE) * dwLen) + 10);
		pbyStatus = (LPBYTE) malloc ((sizeof (BYTE) * dwLen) + 10);
		
		SetDlgItemText (IDC_STATUS, "Reading data from Memory card...... WAIT");
		RedrawWindow ();
		LONG lStatus;
		
		if (MCARD_S_SUCCESS != (lStatus = MCardReadMemory (
				CardHandle,
				byMemZone,
				dwOffset,
				pbyData,
			    &dwLen
				)))
		{
			PutErrorCode (lStatus);
			bStatus = FALSE;
		}
		else
		{
			MCardReadMemoryStatus (
				CardHandle,
				byMemZone,
				dwOffset,
				pbyStatus,
				&dwLen
				);
			
			m_Edit.SetData (pbyData, pbyStatus, dwLen);
			SetDlgItemText (IDC_STATUS, "Data successfully read from Memory card.");
			bStatus = TRUE;
		}
		free (pbyData);
		free (pbyStatus);
	}
	while (FALSE);
}


void CMcardDlg::OnWritemem() 
{

#define		MAX_WRITE		1024
	do
	{	
		BYTE	byMemZone;
		DWORD	dwOffset;
		DWORD	dwLen;
		LPBYTE	pbyData;
		char	szTemp[10];

		GetDlgItemText (IDC_MEMZONE, szTemp, 10);
		sscanf (szTemp, "%x", &byMemZone);

		if (0 == strcmp (szTemp,""))
		{
			SetDlgItemText (IDC_STATUS, "Please enter the memory zone.");
			bStatus = FALSE;
			break;
		}

		GetDlgItemText (IDC_MEMLEN, szTemp, 10);
		sscanf (szTemp, "%x", &dwLen);

		if (0 == strcmp (szTemp,""))
		{
			SetDlgItemText (IDC_STATUS, "Please enter the memory length.");
			bStatus = FALSE;
			break;
		}

		GetDlgItemText (IDC_MEMOFFSET, szTemp, 10);
		sscanf (szTemp, "%x", &dwOffset);

		if (0 == strcmp (szTemp,""))
		{
			SetDlgItemText (IDC_STATUS, "Please enter the memory offset.");
			bStatus = FALSE;
			break;
		}

		if (	(MCARDTYPE_SLE4406 == byCardInUse) || 
				(MCARDTYPE_SLE4436 == byCardInUse) ||
				(MCARDTYPE_SLE5536 == byCardInUse)    )
		{
			int nResult = AfxMessageBox("Writing to this card is not advisable as it has some sensitive data.\nMake sure that the write is intentional.\nContinue Anyway ?", MB_YESNO| MB_ICONSTOP | MB_DEFBUTTON2);

			if (IDNO == nResult)
			{
				SetDlgItemText (IDC_STATUS, "Write Aborted by User.");
				bStatus = FALSE;
				break;
			}
		}

		if (	(MCARDTYPE_AT88SC153 == byCardInUse) || 
				(MCARDTYPE_AT88SC1608 == byCardInUse) )
		{
			if (0 == byMemZone)
			{
				int nResult = AfxMessageBox("This zone is the security zone for this card.\nAltering this can change the existing PIN and/or access rights for other zones.\nContinue Anyway ?", MB_YESNO| MB_ICONSTOP | MB_DEFBUTTON2);

				if (IDNO == nResult)
				{
					SetDlgItemText (IDC_STATUS, "Write Aborted by User.");
					bStatus = FALSE;
					break;
				}
			}
		}

		pbyData = (LPBYTE) malloc ((sizeof (BYTE) * dwLen) + 10);

		m_Edit.GetData (pbyData, dwLen);
		SetDlgItemText (IDC_STATUS, "Writing data to Memory card ...... ");
		RedrawWindow ();

		if (MCARD_S_SUCCESS != (lStatus = MCardWriteMemory (
				CardHandle,
				byMemZone,
				dwOffset,
				pbyData,
				&dwLen
				)))
		{
			PutErrorCode (lStatus);
			bStatus = FALSE;
		}
		else
		{
			SetDlgItemText (IDC_STATUS, "Data successfully written to the card.");
			bStatus = TRUE;
		}
		free (pbyData);
	}
	while (FALSE);
}




void CMcardDlg::OnProtectread() 
{
	do
	{
		BYTE	byMemZone;
		DWORD	dwOffset;
		DWORD	dwLen;
		char	szTempBuffer [10];

		GetDlgItemText (IDC_MEMZONE, szTempBuffer, 10);
		byMemZone = atoi (szTempBuffer);
		
		if (0 == strcmp (szTempBuffer,""))
		{
			SetDlgItemText (IDC_STATUS, "Please enter the memory zone.");
			bStatus = FALSE;
			break;
		}

		GetDlgItemText (IDC_MEMLEN, szTempBuffer, 10);
		dwLen = atol (szTempBuffer);

		if (0 == strcmp (szTempBuffer,""))
		{
			SetDlgItemText (IDC_STATUS, "Please enter the memory length.");
			bStatus = FALSE;
			break;
		}

		GetDlgItemText (IDC_MEMOFFSET, szTempBuffer, 10);
		dwOffset = atol (szTempBuffer);

		if (0 == strcmp (szTempBuffer,""))
		{
			SetDlgItemText (IDC_STATUS, "Please enter the memory offset.");
			bStatus = FALSE;
			break;
		}

		MCardSetMemoryReadProtection (
			CardHandle,
			byMemZone,
			dwOffset,
			&dwLen
			);
	}
	while (FALSE);
}



void CMcardDlg::OnProtectwrite() 
{
	do
	{
		BYTE	byMemZone;
		DWORD	dwOffset;
		DWORD	dwLen;
		char	szTemp[10];
			

		GetDlgItemText (IDC_MEMZONE, szTemp, 10);
		sscanf (szTemp, "%x", &byMemZone);
		
		if (0 == strcmp (szTemp,""))
		{
			SetDlgItemText (IDC_STATUS, "Please enter the memory zone.");
			bStatus = FALSE;
			break;
		}

		GetDlgItemText (IDC_MEMLEN, szTemp, 10);
		sscanf (szTemp, "%x", &dwLen);

		if (0 == strcmp (szTemp,""))
		{
			SetDlgItemText (IDC_STATUS, "Please enter the memory length.");
			bStatus = FALSE;
			break;
		}

		GetDlgItemText (IDC_MEMOFFSET, szTemp, 10);
		sscanf (szTemp, "%x", &dwOffset);

		if (0 == strcmp (szTemp,""))
		{
			SetDlgItemText (IDC_STATUS, "Please enter the memory offset.");
			bStatus = FALSE;
			break;
		}
		
		int nResult = AfxMessageBox("You are about to permanently write protect \nsome bytes. \nContinue Anyway ?", MB_YESNO| MB_ICONSTOP | MB_DEFBUTTON2);

		if (IDNO == nResult)
		{
			SetDlgItemText (IDC_STATUS, "Write protection aborted.");
			bStatus = FALSE;
			break;
		}

		
		if (MCARD_S_SUCCESS != (lStatus = MCardSetMemoryWriteProtection (
				CardHandle,
				byMemZone,
				dwOffset,
				&dwLen
				)))
		{
			PutErrorCode (lStatus);
			bStatus = FALSE;
		}
		else
		{
			SetDlgItemText (IDC_STATUS, "Memory area successfully write protected.");
			bStatus = TRUE;
		}
	}
	while (FALSE);
}

void CMcardDlg::OnClear() 
{
}

void CMcardDlg::OnVerifypin() 
{
	do
	{
		BYTE	byPINNumber;
		BYTE	byPINLength;
		BYTE	byPIN [10];
		char	szTemp [10];
		char	szDispBuffer [150];
		char	szTempBuffer [50];
		char	szPINBuffer [35];
		MCARD_PIN PIN[16];
		DWORD   dwLen;
		

		GetDlgItemText (IDC_PINNUM, szTemp, 10);
		sscanf (szTemp, "%x", &byPINNumber);

		if (0 == strcmp (szTemp,""))
		{
			SetDlgItemText (IDC_STATUS, "Please enter the PIN number.");
			bStatus = FALSE;
			break;
		}

		GetDlgItemText (IDC_CURPINLEN, szTemp, 10);
		sscanf (szTemp, "%x", &byPINLength);

		if (0 == strcmp (szTemp,""))
		{
			SetDlgItemText (IDC_STATUS, "Please enter the PIN length.");
			bStatus = FALSE;
			break;
		}

		GetDlgItemText (IDC_CURPIN, szPINBuffer, 30);

		if (0 == strcmp (szPINBuffer,""))
		{
			SetDlgItemText (IDC_STATUS, "Please enter the PIN.");
			bStatus = FALSE;
			break;
		}

		for (int i = 0, j = 0; j < byPINLength; i+=3, j++)
		{
			strncpy (szTemp, (szPINBuffer + i), 2);
			szTemp[2] = '\0';
			sscanf (szTemp, "%x", &byPIN[j]);
		}

		if (MCARD_S_SUCCESS != (lStatus = MCardVerifyPIN (
				CardHandle,
				byPINNumber,
			    byPIN,
			    byPINLength
				)))
		{
			PutErrorCode (lStatus);

			// Get the retries left.
			if (MCARD_S_SUCCESS != MCardGetAttrib (
			CardHandle,
			MCARD_ATTR_PIN,
			(unsigned char *)PIN,
			&dwLen
			))
			{
				SetDlgItemText (IDC_STATUS, "Error in retrieving PIN organization.");
				bStatus = FALSE;
				break;
			}

			GetDlgItemText (IDC_STATUS, szDispBuffer, 100);
			sprintf (szTempBuffer, " %x retries left", PIN[byPINNumber].byRetries);
			strcat (szDispBuffer, szTempBuffer);
			SetDlgItemText (IDC_STATUS, szDispBuffer);
			bStatus = FALSE;
			((CButton *) GetDlgItem (IDC_WRITEMEM))->EnableWindow (FALSE);
			((CButton *) GetDlgItem (IDC_PROTECTWRITE))->EnableWindow (FALSE);

			if (0 == PIN[byPINNumber].byRetries)
			{
				AfxMessageBox ("There are no retries left in this card.\nThe card is probably blocked", MB_OK| MB_ICONSTOP);
			}
			else if (1 == PIN[byPINNumber].byRetries)
			{
				AfxMessageBox ("There are only one retry left in this card.\nThe card might get probably get blocked if it fails for another PIN verification.\nMake sure you enter the correct PIN or use another card", MB_OK| MB_ICONSTOP);
			}


		}
		else
		{
			((CButton *) GetDlgItem (IDC_WRITEMEM))->EnableWindow (TRUE);
			((CButton *) GetDlgItem (IDC_PROTECTWRITE))->EnableWindow (TRUE);
			SetDlgItemText (IDC_STATUS, "PIN Successfully verified.");
			bStatus = TRUE;
		}
	}
	while (FALSE);


}

void CMcardDlg::OnChangepin() 
{
	do
	{
		BYTE	byPINNumber;
		BYTE	byCurrentPINLength;
		BYTE	byNewPINLength;
		BYTE	byCurrentPIN [10];
		BYTE	byNewPIN [10];
		char	szTemp [10];
		char	szPINBuffer [35];
		

		GetDlgItemText (IDC_PINNUM, szTemp, 10);
		sscanf (szTemp, "%x", &byPINNumber);

		if (0 == strcmp (szTemp,""))
		{
			SetDlgItemText (IDC_STATUS, "Please enter the PIN number.");
			bStatus = FALSE;
			break;
		}

		GetDlgItemText (IDC_CURPINLEN, szTemp, 10);
		sscanf (szTemp, "%x", &byCurrentPINLength);

		if (0 == strcmp (szTemp,""))
		{
			SetDlgItemText (IDC_STATUS, "Please enter the current PIN length.");
			bStatus = FALSE;
			break;
		}

		GetDlgItemText (IDC_NEWPINLEN, szTemp, 10);
		sscanf (szTemp, "%x", &byNewPINLength);

		if (0 == strcmp (szTemp,""))
		{
			SetDlgItemText (IDC_STATUS, "Please enter the new PIN length.");
			bStatus = FALSE;
			break;
		}

		GetDlgItemText (IDC_CURPIN, szPINBuffer, 30);

		if (0 == strcmp (szPINBuffer,""))
		{
			SetDlgItemText (IDC_STATUS, "Please enter the current PIN.");
			bStatus = FALSE;
			break;
		}

		for (int i = 0, j = 0; j < byCurrentPINLength; i+=3, j++)
		{
			strncpy (szTemp, (szPINBuffer + i), 2);
			szTemp[2] = '\0';
			sscanf (szTemp, "%x", &byCurrentPIN[j]);
		}

		GetDlgItemText (IDC_NEWPIN, szPINBuffer, 30);

		if (0 == strcmp (szPINBuffer,""))
		{
			SetDlgItemText (IDC_STATUS, "Please enter the new PIN.");
			bStatus = FALSE;
			break;
		}

		for (i = 0, j = 0; j < byNewPINLength; i+=3, j++)
		{
			strncpy (szTemp, (szPINBuffer + i), 2);
			szTemp[2] = '\0';
			sscanf (szTemp, "%x", &byNewPIN[j]);
		}


		if (MCARD_S_SUCCESS != (lStatus = MCardChangePIN (
				CardHandle,
				byPINNumber,
				byCurrentPIN,
				byCurrentPINLength,
				byNewPIN,
				byNewPINLength
				)))
		{
			PutErrorCode (lStatus);
			bStatus = FALSE;
		}
		else
		{
			SetDlgItemText (IDC_STATUS, "The PIN was successfully changed.");
			bStatus = TRUE;
		}
	}
	while (FALSE);
}

void CMcardDlg::OnGetresponse() 
{

	do
	{
		BYTE	byChallengeID;
		BYTE	byChallengeLength;
		BYTE	byChallenge [10];
		BYTE	byResponse [10];
		BYTE	byResponseLength;
		char	szTemp [10];
		char	szChallegeBuffer [100];
		
		char szDispBuffer [100];
		

		GetDlgItemText (IDC_CHALLENGEID, szTemp, 10);
		sscanf (szTemp, "%x", &byChallengeID);
		
		if (0 == strcmp (szTemp,""))
		{
			SetDlgItemText (IDC_STATUS, "Please enter the Challenge ID.");
			bStatus = FALSE;
			break;
		}


		GetDlgItemText (IDC_CHALLENGELEN, szTemp, 10);
		sscanf (szTemp, "%x", &byChallengeLength);

		if (0 == strcmp (szTemp,""))
		{
			SetDlgItemText (IDC_STATUS, "Please enter the Challenge length.");
			bStatus = FALSE;
			break;
		}

		GetDlgItemText (IDC_CBUFFER, szChallegeBuffer, 100);

		if (0 == strcmp (szChallegeBuffer,""))
		{
			SetDlgItemText (IDC_STATUS, "Please enter the Challenge.");
			bStatus = FALSE;
			break;
		}

		for (int i = 0, j = 0; j < byChallengeLength; i+=3, j++)
		{
			strncpy (szTemp, (szChallegeBuffer + i), 2);
			szTemp[2] = '\0';
			sscanf (szTemp, "%x", &byChallenge[j]);
		}

		if (MCARD_S_SUCCESS != (lStatus = MCardChallengeResponse(
				CardHandle,
				byChallengeID,
				byChallenge,
				byChallengeLength,
				byResponse,
				&byResponseLength
				)))
		{
			PutErrorCode (lStatus);
			bStatus = FALSE;
		}
		else
		{

			char szTempBuffer [30];
			strcpy (szDispBuffer, "");
			for (i = 0; i < byResponseLength; i++)
			{
				
				itoa (byResponse[i] , szTempBuffer, 16);
				strcat (szDispBuffer, szTempBuffer);
				strcat (szDispBuffer, "  ");
			}
			SetDlgItemText (IDC_RBUFFER, szDispBuffer);
			bStatus = TRUE;
		}
	}
	while (FALSE);
}


void CMcardDlg::PutErrorCode (LONG lStatus)
{
	switch (lStatus)
	{
	case MCARD_S_SUCCESS:
		SetDlgItemText (IDC_STATUS, "MCARD_S_SUCCESS");
		break;

	case MCARD_E_INTERNAL_ERROR:
		SetDlgItemText (IDC_STATUS, "An Internal ERROR occured in the DLL");
		break;

	case MCARD_E_NOT_IMPLEMENTED:
		SetDlgItemText (IDC_STATUS, "This API has not been implemented in the DLL");
		break;

	case MCARD_E_NOT_INITIALIZED:
		SetDlgItemText (IDC_STATUS, "The MCardInitialize was not called.");
		break;

	case MCARD_E_UNKNOWN_CARD:
		SetDlgItemText (IDC_STATUS, "The Memory Card could not be identified by the DLL"); 
		break;

	case MCARD_E_INVALID_PARAMETER:
		SetDlgItemText (IDC_STATUS, "Atleast one parameter is invalid in this API call"); 
		break;

	case MCARD_E_CHAL_RESP_FAILED:
		SetDlgItemText (IDC_STATUS, "The Challenge/Response failed"); 
		break;

	case MCARD_E_INVALID_MEMORY_RANGE:
		SetDlgItemText (IDC_STATUS, "The Memory range specified is invalid"); 
		break;

	case MCARD_E_INVALID_MEMORY_ZONE_ID:
		SetDlgItemText (IDC_STATUS, "The Memory zone specified is invalid"); 
		break;

	case MCARD_E_INVALID_PIN_ID:
		SetDlgItemText (IDC_STATUS, "The PIN ID is invalid"); 
		break;

	case MCARD_E_INVALID_CHAL_RESP_ID:
		SetDlgItemText (IDC_STATUS, "The Challenge/Response ID is invalid"); 
		break;

	case MCARD_W_NOT_ALL_DATA_READ:
		SetDlgItemText (IDC_STATUS, "All the requested data could not be read"); 
		break;

	case MCARD_W_NOT_ALL_DATA_WRITTEN:
		SetDlgItemText (IDC_STATUS, "All the requested data could not be written"); 
		break;

	case MCARD_W_PIN_VERIFY_NEEDED:
		SetDlgItemText (IDC_STATUS, "PIN Verification is required for this operation"); 
		break;

	case MCARD_W_PIN_VERIFY_FAILED:
		SetDlgItemText (IDC_STATUS, "PIN Verification FAILED!!!"); 
		break;

	case MCARD_W_NO_PIN_ATTEMPTS_LEFT:
		SetDlgItemText (IDC_STATUS, "No PIN retries are left for this card. Card might be blocked"); 
		break;

	case MCARD_W_NO_UNITS_TO_DECREMENT:
		SetDlgItemText (IDC_STATUS,"There are no units left in the card for decrementing");
		break;

	case MCARD_W_REMOVED_CARD:
		SetDlgItemText (IDC_STATUS,"The card has been removed.");
		break;

	default:
		SetDlgItemText (IDC_STATUS, "This error code is not recognized by this app");
		break;
	}
}


HBRUSH CMcardDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	static LONG lPrevStatus = 0;
   // Call the base class implementation first! Otherwise, it may
   // undo what we're trying to accomplish here.
   HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
   CString csString;

   GetDlgItemText (pWnd->GetDlgCtrlID(), csString);

   // Are we painting the IDC_MYSTATIC control? We can use
   // CWnd::GetDlgCtrlID() to perform the most efficient test.

   switch (pWnd->GetDlgCtrlID())
   {
   case IDC_TYPE:
   
	   pDC->SetTextColor(RGB (0, 0, 255));
	   break;

   case IDC_MEMBUFFER:
	   
	   pDC->SetTextColor(RGB (53, 38, 28));
	   break;

   case IDC_PROTOCOL:
	   pDC->SetTextColor(RGB (255, 0, 0));
	   break;


   case IDC_STATUS:
	   if (bStatus)
	   {
			//pDC->SetTextColor(RGB (0, 100, 200));
		   pDC->SetTextColor(RGB (80, 130, 0));
	   }
	   else
	   {	
		   pDC->SetTextColor(RGB (150, 0, 0));
	   }
	   break;

   default:
	   break;
   }
   return hbr;
}

void CMcardDlg::OnButton1() 
{
	do
	{
		char	szTemp[10];
		DWORD	dwCounterVal;

		GetDlgItemText (IDC_COUNTER, szTemp, 10);
		sscanf (szTemp, "%x", &dwCounterVal);

		if (0 == strcmp (szTemp,""))
		{
			SetDlgItemText (IDC_STATUS, "Please enter the value to be decremented");
			bStatus = FALSE;
			break;
		}

		int nResult = AfxMessageBox("This operation will permanantly decrement \nthe given value from the card. \nContinue Anyway ?", MB_YESNO| MB_ICONSTOP | MB_DEFBUTTON2);

		if (IDNO == nResult)
		{
			SetDlgItemText (IDC_STATUS, "Write protection aborted.");
			bStatus = FALSE;
			break;
		}

		if (MCARD_S_SUCCESS != (lStatus = MCardDeductCounter (
				CardHandle,
				0,
				dwCounterVal
				)))
		{
			PutErrorCode (lStatus);
			bStatus = FALSE;
		}
		else
		{
			SetDlgItemText (IDC_STATUS, "Counter successfully decremented.");
			bStatus = TRUE;
		}

	}
	while (FALSE);
	
}

void CMcardDlg::OnShutdown() 
{
	LONG lReturn = MCardShutdown (hMCardContext);

	if (lReturn)
	{
		bStatus = FALSE;
		PutErrorCode (lReturn);
	}
	else
	{
		bStatus = TRUE;
		SetDlgItemText (IDC_STATUS, "The Memory Card DLL was properly shutdown.");
	}

	
}

void CMcardDlg::OnMeminit() 
{
	DWORD dwDllVersion = 0;
	LONG lReturn = MCardInitialize  (ContextHandle,ReaderInFocus , &hMCardContext, &dwDllVersion);

	if (lReturn)
	{
		bStatus = FALSE;
		PutErrorCode (lReturn);
	}
	else
	{
		bStatus = TRUE;
		SetDlgItemText (IDC_STATUS, "The Memory Card DLL has been properly initialized");
	}

}

void CMcardDlg::OnEditchangeAttrs() 
{
	
}

void CMcardDlg::OnSelchangeAttrs() 
{
	CComboBox *pbox1 = (CComboBox*)GetDlgItem(IDC_ATTRS);
	nSelectedAttribute = pbox1->GetCurSel ();
	
}

UINT CMcardDlg::MyThreadProc(LPVOID app)
{
	CMcardDlg *mcarddlg = (CMcardDlg *)app;
	char szOldString[40];

	while (true)
	{
		if (TRUE == bAppClosed)
		{
			break;
		}

		mcarddlg->GetDlgItemText (IDC_CARDSTATUS, szOldString, 40);

		if (0x01 == ((CButton*) mcarddlg->GetDlgItem(IDC_CARDSTATEENABLE))->GetCheck ())
		{
			DWORD dwTimeOut = 100;
			DWORD dwExpectedCardState = 0x01;
			DWORD dwCardState = 500;
			char szTemp [10];

			mcarddlg->GetDlgItemText (IDC_CARDTIMEOUT, szTemp, 10);
			sscanf (szTemp, "%x", &dwTimeOut);

			mcarddlg->SetDlgItemText (IDC_CARDSTATUS, "");
			LONG lReturn = MCardWaitForCardState(0x01, dwExpectedCardState, \
				&dwCardState, INFINITE);

			if (0 == lReturn)
			{
				if (1 == dwCardState) 
				{
					if (0 != stricmp(szOldString, "ABSENT"))
					{
						mcarddlg->SetDlgItemText (IDC_CARDSTATUS, "ABSENT");
					}
				}
				else if (2 == dwCardState)
				{
					if (0 != stricmp(szOldString, "PRESENT"))
					{
						mcarddlg->SetDlgItemText (IDC_CARDSTATUS, "PRESENT");
					}
				}
				else if (6 == dwCardState)
				{
					if (0 != stricmp(szOldString, "POWERED"))
					{
						mcarddlg->SetDlgItemText (IDC_CARDSTATUS, "POWERED");
					}
				}
				else if (0 != stricmp(szOldString, "UNKNOWN"))
				{
					mcarddlg->SetDlgItemText (IDC_CARDSTATUS, "UNKNOWN");
				}
			}
			else if (0 != stricmp(szOldString, "FAILED"))
			{
				mcarddlg->SetDlgItemText (IDC_CARDSTATUS, "FAILED");
			}
		}
		else if (0 != stricmp(szOldString, "DISABLED") )
		{
			mcarddlg->SetDlgItemText (IDC_CARDSTATUS, "DISABLED");
		}
		Sleep (1000);
	}
	AfxMessageBox ("Thread Killed");
	AfxEndThread(0);

	return 0;
}

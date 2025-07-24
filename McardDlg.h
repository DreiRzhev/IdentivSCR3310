// McardDlg.h : header file
//

#if !defined(AFX_MCARDDLG_H__F5A07DC7_7259_11D6_A66A_0010B5C7D9C6__INCLUDED_)
#define AFX_MCARDDLG_H__F5A07DC7_7259_11D6_A66A_0010B5C7D9C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "hexeditctrl.h"
/////////////////////////////////////////////////////////////////////////////
// CMcardDlg dialog

class CMcardDlg : public CDialog
{
// Construction
public:
	CHexEdit	m_Edit;
	HANDLE m_hThread;
	CMcardDlg(CWnd* pParent = NULL); // standard constructor
	void FillReaderList ();
	void LoadMcardDLL ();
	void InitializeProcAddresses ();
	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	void PutErrorCode (LONG);
	static UINT MyThreadProc(LPVOID);
	
// Dialog Data
	//{{AFX_DATA(CMcardDlg)
	enum { IDD = IDD_MCARD_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMcardDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	// Generated message map functions
	//{{AFX_MSG(CMcardDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnExit();
	afx_msg void OnConnect();
	afx_msg void OnSelchangeReaderNames();
	afx_msg void OnGetattrib();
	afx_msg void OnDisconnect();
	afx_msg void OnReadmem();
	afx_msg void OnSetattrib();
	afx_msg void OnProtectread();
	afx_msg void OnWritemem();
	afx_msg void OnProtectwrite();
	afx_msg void OnClear();
	afx_msg void OnVerifypin();
	afx_msg void OnChangepin();
	afx_msg void OnGetresponse();
	afx_msg void OnButton1();
	afx_msg void OnShutdown();
	afx_msg void OnMeminit();
	afx_msg void OnEditchangeAttrs();
	afx_msg void OnSelchangeAttrs();
	afx_msg void OnCardstate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


// DLL Related definitions
#define IN
#define OUT

// 1. MCARDCONTEXT

typedef SCARDHANDLE			MCARDCONTEXT;
typedef MCARDCONTEXT*		PMCARDCONTEXT;


// 2. MCARDHANDLE 

typedef SCARDHANDLE MCARDHANDLE;
typedef MCARDHANDLE* PMCARDHANDLE;



// DLL Related Export functions typedefs

typedef LONG  (WINAPI *pfnMCardInitialize) (
	IN	SCARDCONTEXT  hSCardContext,
	IN  LPCTSTR szReaderName,
    OUT PMCARDCONTEXT phMCardContext,
    OUT PDWORD pdwDllVersion
    );

typedef LONG  (WINAPI *pfnMCardShutdown) (
	IN MCARDCONTEXT hMCardContext
	);

typedef LONG  (WINAPI *pfnMCardConnect) (
	IN MCARDCONTEXT hMCardContext,
    IN DWORD dwConnectMode,
	IN BYTE byCardType,
    OUT PMCARDHANDLE phMCard
    );

typedef LONG  (WINAPI *pfnMCardDisconnect) (
    IN MCARDHANDLE hMCard,
    IN DWORD dwDisposition
    );

typedef LONG  (WINAPI *pfnMCardGetAttrib) (
    IN MCARDHANDLE hMCard,
    IN DWORD dwAttrId,
    OUT LPBYTE pbAttr,
    IN OUT LPDWORD pcbAttrLen
    );

typedef LONG  (WINAPI *pfnMCardSetAttrib) (
    IN MCARDHANDLE hMCard,
    IN DWORD dwAttrId,
    IN LPBYTE pbAttr,
    IN DWORD cbAttrLen
    );

typedef LONG  (WINAPI *pfnMCardReadMemory) (
    IN MCARDHANDLE hMCard,
    IN BYTE bMemZone,
    IN DWORD dwOffset,
    IN LPBYTE pbReadBuffer,
    IN OUT LPDWORD pbReadLen
    );

typedef LONG  (WINAPI *pfnMCardWriteMemory) (
    IN MCARDHANDLE hMCard,
    IN BYTE bMemZone,
    IN DWORD dwOffset,
    IN LPBYTE pbWriteBuffer,
    IN OUT LPDWORD pcbWriteLen
    );

typedef LONG  (WINAPI *pfnMCardSetMemoryWriteProtection) (
    IN MCARDHANDLE hMCard,
    IN BYTE bMemZone,
    IN DWORD dwOffset,
    IN OUT LPDWORD pbcLen
    );

typedef LONG  (WINAPI *pfnMCardSetMemoryReadProtection) (
    MCARDHANDLE hMCard,
    IN BYTE bMemZone,
    IN DWORD dwOffset,
    IN OUT LPDWORD pbLen
    );

typedef LONG  (WINAPI *pfnMCardReadMemoryStatus) (
    MCARDHANDLE hMCard,
    IN BYTE bMemZone,
    IN DWORD dwOffset,
    OUT PBYTE pbBuffer,
    IN OUT LPDWORD pcbLen
    );

typedef LONG  (WINAPI *pfnMCardVerifyPIN) (
    IN MCARDHANDLE hMCard,
    IN BYTE bPinNumber,
    IN PBYTE pbBufferWithPIN,
    IN BYTE pbcLen
    );

typedef LONG  (WINAPI *pfnMCardChangePIN) (
    IN MCARDHANDLE hMCard,
    IN BYTE bPinNumber,
    IN PBYTE pbBufferWithOldPIN,
    IN BYTE cbOldLen,
    IN PBYTE pbBufferWithNewPIN,
    IN BYTE cbNewLen
    );

typedef LONG  (WINAPI *pfnMCardChallengeResponse) (
    IN MCARDHANDLE hMCard,
    IN BYTE bChallengeID,
    IN PBYTE pbChallengeBuffer,
    IN BYTE cbChallengeLen,
    OUT PBYTE pbResponseBuffer,
    OUT PBYTE cbResponseLen
    );

typedef LONG  (WINAPI *pfnMCardDeductCounter) (
    IN MCARDHANDLE hMCard,
	IN BYTE bCounterID,
    IN DWORD dwUnits
    );

typedef LONG  (WINAPI *pfnMCardWaitForCardState) (
    IN  MCARDCONTEXT hMCardContext,
    IN  DWORD        dwExpectedCardState,
    OUT DWORD*       pdwCardState,
    IN  DWORD        dwTimeOut
    );

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
 
#endif // !defined(AFX_MCARDDLG_H__F5A07DC7_7259_11D6_A66A_0010B5C7D9C6__INCLUDED_)

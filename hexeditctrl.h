#ifndef _HEXEDIT_H
#define _HEXEDIT_H
/////////////////////////////////////////////////////////////////////////////
// CHexEdit window

class CHexEdit : public CEdit
{
// Construction
public:
	CHexEdit();

	enum EDITMODE{	
			EDIT_NONE,
			EDIT_ASCII,
			EDIT_HIGH,
			EDIT_LOW
	} ;
// Attributes
public:
	LPBYTE		m_pData;			// pointer to data
	PBYTE		m_pStatus;			// Read/Write Protection Status
	int			m_length;			// length of data
	int			m_topindex;			// offset of first visible byte on screen

	int			m_currentAddress;	// address under cursor
	EDITMODE	m_currentMode;		// current editing mode: address/hex/ascii
	int			m_selStart;			// start address of selection
	int			m_selEnd;			// end address of selection

	int			m_bpr;				// byte per row 
	int			m_lpp;				// lines per page
	BOOL		m_bShowAddress;
	BOOL		m_bShowAscii;
	BOOL		m_bShowHex;
	BOOL		m_bAddressIsWide;
	
	BOOL		m_bNoAddressChange;	// internally used
	BOOL		m_bHalfPage;
	
	CFont		m_Font;
	int			m_lineHeight;
	int			m_nullWidth;
	BOOL		m_bUpdate;

	int			m_offHex,
				m_offAscii,
				m_offAddress;

	CPoint		m_editPos;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHexEdit)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	int GetData(LPBYTE p, int len);
	void SetData(LPBYTE p, LPBYTE pbyStatus, int len);
	CSize GetSel(void);
	void SetSel(int s, int e);
	void SetBPR(int bpr);
	void SetOptions(BOOL a, BOOL h, BOOL c, BOOL w);
	virtual ~CHexEdit();

	// Generated message map functions
protected:
	void		ScrollIntoView(int p);
	void		RepositionCaret(int p);
	void		Move(int x, int y);
	inline BOOL IsSelected(void);
	void		UpdateScrollbars(void);
	void		CreateEditCaret(void);
	void		CreateAddressCaret(void);
	CPoint		CalcPos(int x, int y);
	void		SelInsert(int s, int l);
	void		SelDelete(int s, int e);
	inline void NormalizeSel(void);
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	//{{AFX_MSG(CHexEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnPaint();
	afx_msg void OnNcPaint ();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg UINT OnGetDlgCode();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEditClear();
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnEditSelectAll();
	afx_msg void OnEditUndo();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#define		RED		RGB(255,0,0)
#define		BLUE	RGB(0,0,255)
#define		GREEN	RGB(0,255,0)
#define		BLACK	RGB(0,0,0)

#endif
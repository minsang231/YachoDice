#pragma once
#include "afxdialogex.h"


// CIntroDlg 대화 상자

class CIntroDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CIntroDlg)

public:
	CIntroDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CIntroDlg();
	CImage m_imgBg;
	virtual BOOL OnInitDialog();
	CDatabase m_db;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INTRO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnHelp();
	afx_msg void OnBnClickedGameStart();
	afx_msg void OnPaint();
	CString m_ID;
	CString m_PW;
	BOOL ConnectDB();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButton2();
};

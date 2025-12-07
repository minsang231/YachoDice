#pragma once
#include "afxdialogex.h"


// CPlayer2LoginDlg 대화 상자

class CPlayer2LoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPlayer2LoginDlg)

public:
	CPlayer2LoginDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CPlayer2LoginDlg();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_P2_LOGIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strValP2ID;
	afx_msg void OnBnClickedOk();
};

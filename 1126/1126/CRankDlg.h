#pragma once
#include "afxdialogex.h"
#include <afxdb.h>

// CRankDlg 대화 상자

class CRankDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRankDlg)

public:
	CRankDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CRankDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_RANK };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnCustomdrawList(NMHDR* pNMHDR, LRESULT* pResult);
public:
	void LoadRankings();
	DECLARE_MESSAGE_MAP()
	CListCtrl m_listScore;
	CListCtrl m_listWin;
};

#pragma once
#include "afxdialogex.h"


// CStartDlg 대화 상자

class CStartDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CStartDlg)

public:
	CStartDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CStartDlg();
	int m_nSelectedMode; // 1이면 1인용, 2면 2인용
	// ★★★ [추가] 배경 이미지 변수
	CImage m_imgBg;

	// ★★★ [추가] 그리기 함수 선언
	afx_msg void OnPaint();
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
};

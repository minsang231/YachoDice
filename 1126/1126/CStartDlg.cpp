// CStartDlg.cpp: 구현 파일
//

#include "pch.h"
#include "1126.h"
#include "afxdialogex.h"
#include "CStartDlg.h"


// CStartDlg 대화 상자

IMPLEMENT_DYNAMIC(CStartDlg, CDialogEx)

CStartDlg::CStartDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CStartDlg::~CStartDlg()
{
}

void CStartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStartDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &CStartDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CStartDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CStartDlg 메시지 처리기

void CStartDlg::OnBnClickedButton1()
{
	m_nSelectedMode = 1;
	OnOK(); // 다이얼로그 닫고 성공 리턴
}


void CStartDlg::OnBnClickedButton2()
{
	m_nSelectedMode = 2;
	OnOK(); // 다이얼로그 닫고 성공 리턴
}


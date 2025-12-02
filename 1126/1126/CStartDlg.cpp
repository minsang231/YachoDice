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
	ON_BN_CLICKED(IDC_BUTTON3, &CStartDlg::OnBnClickedButton3)

	ON_WM_PAINT()
END_MESSAGE_MAP()


// CStartDlg 메시지 처리기
BOOL CStartDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ★★★ [추가] 이미지 로드 (파일 이름은 본인 파일명으로 수정하세요)
	if (m_imgBg.IsNull())
	{
		m_imgBg.Load(_T("BackGround.png"));
	}

	return TRUE;
}

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



void CStartDlg::OnBnClickedButton3()
{
	// IDCANCEL을 반환하며 창을 닫음
	// (InitInstance에서 이걸 신호로 받아서 처리할 예정)
	EndDialog(IDCANCEL);
}

void CStartDlg::OnPaint()
{
	CPaintDC dc(this); // 그리기 도구

	// 1. 화면 크기 구하기
	CRect rect;
	GetClientRect(&rect);

	// 2. 이미지가 있으면 꽉 차게 그리기
	if (!m_imgBg.IsNull())
	{
		// ★★★ [추가] 이미지 확대/축소 시 화질 보정 옵션 (깨짐 방지) ★★★
		dc.SetStretchBltMode(HALFTONE);

		m_imgBg.Draw(dc.m_hDC, 0, 0, rect.Width(), rect.Height());
	}
}
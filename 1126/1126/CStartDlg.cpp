// CStartDlg.cpp: 구현 파일
//

#include "pch.h"
#include "1126.h"
#include "afxdialogex.h"
#include "CStartDlg.h"
#include "CPlayer2LoginDlg.h"

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
	CMy1126App* pApp = (CMy1126App*)AfxGetApp();

	// ★★★ [수정] 로그인한 경우에만 두 번째 플레이어 로그인 창 띄우기 ★★★
	if (!pApp->m_strCurrentUserID.IsEmpty())
	{
		// 첫 번째 플레이어가 로그인되어 있으면 두 번째 플레이어 로그인 창 띄우기
		CPlayer2LoginDlg dlg;

		// 창을 띄웠는데 '취소'를 누르면 게임 시작 안 하고 리턴
		if (dlg.DoModal() != IDOK)
		{
			return;
		}
	}
	else
	{
		// ★★★ [추가] 게스트 모드일 경우 바로 2인 게임 시작 (두 명 모두 게스트) ★★★
		// Player 2 ID도 빈 문자열로 유지 (게스트)
		pApp->m_strPlayer2ID = _T("");
	}

	// 두 번째 플레이어 로그인 성공 또는 게스트 모드 -> 게임 시작
	m_nSelectedMode = 2;
	CDialogEx::OnOK();
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

BOOL CStartDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE) return TRUE; // ESC 무시
		if (pMsg->wParam == VK_RETURN) return TRUE; // Enter 무시
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
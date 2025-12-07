// CPlayer2LoginDlg.cpp: 구현 파일
//

#include "pch.h"
#include "1126.h"
#include "afxdialogex.h"
#include "CPlayer2LoginDlg.h"

// CPlayer2LoginDlg 대화 상자

IMPLEMENT_DYNAMIC(CPlayer2LoginDlg, CDialogEx)

CPlayer2LoginDlg::CPlayer2LoginDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_P2_LOGIN, pParent)
	, m_strValP2ID(_T(""))
{

}

CPlayer2LoginDlg::~CPlayer2LoginDlg()
{
}

void CPlayer2LoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_P2_ID, m_strValP2ID);
}


BEGIN_MESSAGE_MAP(CPlayer2LoginDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CPlayer2LoginDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CPlayer2LoginDlg 메시지 처리기

void CPlayer2LoginDlg::OnBnClickedOk()
{
    // 1. Edit Control(ID 입력칸)의 값을 m_strValP2ID 변수로 가져옴
    UpdateData(TRUE);

    // 2. 비밀번호 입력값 가져오기 (GetDlgItemText 사용)
    CString strPW;
    GetDlgItemText(IDC_EDIT_P2_PW, strPW);

    // [유효성 검사 1] 빈칸 체크
    if (m_strValP2ID.IsEmpty() || strPW.IsEmpty())
    {
        AfxMessageBox(_T("ID와 비밀번호를 모두 입력해주세요."));
        return;
    }

    CMy1126App* pApp = (CMy1126App*)AfxGetApp();

    // [유효성 검사 2] 자기 자신과 같은지 체크
    if (m_strValP2ID == pApp->m_strCurrentUserID)
    {
        AfxMessageBox(_T("이미 Player 1으로 로그인되어 있습니다.\n다른 ID를 입력해주세요."));
        return;
    }

    try
    {
        CRecordset rs(&pApp->m_db);
        CString strQuery;

        // ★★★ [핵심 수정] ID와 PW가 모두 일치하는지 확인하는 쿼리 ★★★
        // 주의: DB 테이블의 비밀번호 컬럼명이 'PW'가 맞는지 꼭 확인하세요! 
        // (만약 'PASSWORD'라면 PW 대신 PASSWORD로 적어야 합니다)
        strQuery.Format(_T("SELECT ID FROM DC WHERE ID = '%s' AND PW = '%s'"), m_strValP2ID, strPW);

        rs.Open(CRecordset::forwardOnly, strQuery, CRecordset::readOnly);

        // 결과가 없으면(EOF) -> ID가 없거나 비밀번호가 틀린 것임
        if (rs.IsEOF())
        {
            AfxMessageBox(_T("ID가 존재하지 않거나 비밀번호가 일치하지 않습니다."));
            rs.Close();
            return;
        }

        // 여기까지 왔으면 로그인 성공!
        rs.Close();

        // App 클래스에 상대방 ID 저장
        pApp->m_strPlayer2ID = m_strValP2ID;

        AfxMessageBox(_T("Player 2 로그인 성공! 게임을 시작합니다.")); // 확인 메시지 (선택사항)
        CDialogEx::OnOK();
    }
    catch (CDBException* e)
    {
        AfxMessageBox(_T("DB 에러: ") + e->m_strError);
        e->Delete();
    }
}

BOOL CPlayer2LoginDlg::PreTranslateMessage(MSG* pMsg)
{
    // 키보드 누름 메시지인지 확인
    if (pMsg->message == WM_KEYDOWN)
    {
        // 1. ESC 키 차단 (게임 종료 방지)
        if (pMsg->wParam == VK_ESCAPE)
        {
            return TRUE; // "내가 처리했으니 윈도우는 신경 꺼라" (무시됨)
        }

        // 2. Enter 키 차단 (의도치 않은 버튼 클릭 방지)
        // 채팅 기능이 없다면 막는 게 좋습니다.
        if (pMsg->wParam == VK_RETURN)
        {
            return TRUE; // Enter 키 무시
        }
    }

    // 다른 키들은 원래대로 처리하도록 부모 클래스에게 넘김
    return CDialogEx::PreTranslateMessage(pMsg);
}
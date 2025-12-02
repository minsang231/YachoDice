// CIntroDlg.cpp: 구현 파일
//

#include "pch.h"
#include "1126.h"
#include "afxdialogex.h"
#include "CIntroDlg.h"
#include "CRankDlg.h"

// CIntroDlg 대화 상자

IMPLEMENT_DYNAMIC(CIntroDlg, CDialogEx)

CIntroDlg::CIntroDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INTRO, pParent)
    , m_ID(_T(""))
    , m_PW(_T(""))
{

}

CIntroDlg::~CIntroDlg()
{
}

void CIntroDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT1, m_ID);
    DDX_Text(pDX, IDC_EDIT2, m_PW);
}


BEGIN_MESSAGE_MAP(CIntroDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_HELP, &CIntroDlg::OnBnClickedBtnHelp)
	ON_BN_CLICKED(IDC_GAME_START, &CIntroDlg::OnBnClickedGameStart)
    ON_WM_PAINT()
    ON_BN_CLICKED(IDC_BUTTON4, &CIntroDlg::OnBnClickedButton4)
    ON_BN_CLICKED(IDC_BUTTON1, &CIntroDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON3, &CIntroDlg::OnBnClickedButton3)
    ON_WM_CTLCOLOR()
    ON_BN_CLICKED(IDC_BUTTON2, &CIntroDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CIntroDlg 메시지 처리기

BOOL CIntroDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();


    // =========================================================
    // ★★★ [추가] 배경 이미지 불러오기 ★★★
    // =========================================================
    // 파일이 프로젝트 폴더에 있어야 합니다.
    if (m_imgBg.IsNull())
    {
        m_imgBg.Load(_T("다이스인트로2.png"));
    }

    return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CIntroDlg::OnBnClickedBtnHelp()
{
	CString strHelp;
	strHelp = _T("🎲 [요트 다이스 게임 규칙] 🎲\n\n");

    strHelp += _T("[ 기본 진행 ]\n");
    strHelp += _T("1. 주사위를 최대 3번까지 굴릴 수 있습니다.\n");
    strHelp += _T("2. 주사위를 클릭하여 Keep(보관) 할 수 있습니다.\n");
    strHelp += _T("3. 원하는 족보를 선택해 점수를 등록하세요.\n\n");

    strHelp += _T("================================\n");
    strHelp += _T("[ 상단 항목 (숫자) ]\n");
    strHelp += _T("• Ones ~ Sixes : 해당 주사위 눈의 총합\n");
    strHelp += _T("• 보너스 : 상단 합계 63점 이상 시 +35점\n\n");

    strHelp += _T("[ 하단 항목 (족보) ]\n");
    strHelp += _T("• 초이스 (Chance) : 모든 주사위 눈의 총합\n");
    strHelp += _T("• 3 of a Kind : 같은 수 3개 이상 → 총합\n");
    strHelp += _T("• 4 of a Kind : 같은 수 4개 이상 → 총합\n");
    strHelp += _T("• 풀하우스 : 같은 수 3개 + 2개 → 25점\n");
    strHelp += _T("• S.스트레이트 : 연속된 숫자 4개 → 30점\n");
    strHelp += _T("• L.스트레이트 : 연속된 숫자 5개 → 40점\n");
    strHelp += _T("• 요트 (Yacht) : 5개 모두 같은 수 → 50점\n");

    // 메시지 박스 띄우기 (정보 아이콘 포함)
    AfxMessageBox(strHelp, MB_ICONINFORMATION);
}

void CIntroDlg::OnBnClickedGameStart()
{
    // 1. 확인 메시지 띄우기
    CString strMsg = _T("게스트 모드는 랭킹에 저장되지 않습니다.\n괜찮으십니까?");

    // [예] / [아니요] 버튼이 있는 메시지 박스 실행
    if (AfxMessageBox(strMsg, MB_YESNO | MB_ICONQUESTION) == IDYES)
    {
        // 2. [예]를 눌렀을 때 -> 게임 시작 진행

        // ★ 중요: 게스트니까 아이디 변수를 확실하게 비워줍니다.
        CMy1126App* pApp = (CMy1126App*)AfxGetApp();
        pApp->m_strCurrentUserID = _T(""); // 빈 문자열 = 게스트

        // 다음 단계(모드 선택)로 넘어감
        EndDialog(IDOK);
    }

    // [아니요]를 누르면 아무 일도 안 일어나고 현재 창에 머무릅니다.
}

void CIntroDlg::OnPaint()
{
    CPaintDC dc(this); // 그리기를 위한 도구 (필수)

    // 1. 화면 크기 구하기
    CRect rect;
    GetClientRect(&rect);

    // 2. 이미지가 로드되어 있으면 화면 꽉 차게 그리기
    if (!m_imgBg.IsNull())
    {
        // (HDC, x, y, width, height)
        m_imgBg.Draw(dc.m_hDC, 0, 0, rect.Width(), rect.Height());
    }
}


void CIntroDlg::OnBnClickedButton4()
{
    // 1. 화면에 적힌 글자를 변수(m_strID, m_strPW)로 가져옴
    UpdateData(TRUE);

    if (m_ID.IsEmpty() || m_PW.IsEmpty()) {
        AfxMessageBox(_T("아이디와 비밀번호를 입력해주세요."));
        return;
    }

    // 2. DB 연결

    CMy1126App* pApp = (CMy1126App*)AfxGetApp();

    // 3. 회원가입 쿼리 (INSERT)
    CString strQuery;

    // TBL_USER 테이블에 ID, PW, 점수(0) 저장
    strQuery.Format(_T("INSERT INTO DC (ID, PW, SCORE) VALUES ('%s', '%s', 0)"), m_ID, m_PW);

    try
    {
        pApp->m_db.ExecuteSQL(strQuery);
        AfxMessageBox(_T("회원가입 성공! 이제 로그인 해주세요."));
    }
    catch (CDBException* e)
    {
        // ★★★ [수정] 거짓말하지 말고 진짜 에러 내용을 보여줘! ★★★
        CString msg;
        msg.Format(_T("에러 발생!\n\n진짜 이유: %s"), e->m_strError);
        AfxMessageBox(msg);

        e->Delete();
    }
}


void CIntroDlg::OnBnClickedButton1()
{
    // 1. 화면 값 가져오기
    UpdateData(TRUE);
    CMy1126App* pApp = (CMy1126App*)AfxGetApp();

    if (m_ID.IsEmpty() || m_PW.IsEmpty()) {
        AfxMessageBox(_T("아이디와 비밀번호를 입력해주세요."));
        return;
    }

    // 2. DB 연결
    //if (ConnectDB() == FALSE) return;

    // 3. 로그인 쿼리 (SELECT)
    CString strQuery;
    strQuery.Format(_T("SELECT * FROM DC WHERE ID='%s' AND PW='%s'"), m_ID, m_PW);

    // 4. 조회 결과 확인
    CRecordset rs(&(pApp->m_db));

    try
    {
        rs.Open(CRecordset::forwardOnly, strQuery);

        if (!rs.IsEOF()) // 데이터가 있다 = 로그인 성공
        {
            AfxMessageBox(_T("로그인 성공! 게임을 시작합니다."));

            // ★ 중요: 로그인이 성공했으므로 다이얼로그를 닫고 게임으로 넘어감
           
            pApp->m_strCurrentUserID = m_ID;  // <--- 이게 있어야 함

            EndDialog(IDOK);
        }
        else // 데이터가 없다 = 실패
        {
            AfxMessageBox(_T("로그인 실패.\n아이디나 비밀번호를 확인하세요."));
        }
        rs.Close();
    }
    catch (CDBException* e)
    {
        AfxMessageBox(e->m_strError);
        e->Delete();
    }
}


void CIntroDlg::OnBnClickedButton3()
{
    // 1. 랭킹 다이얼로그 객체 생성
    CRankDlg dlg;
    // 2. 창 띄우기
    dlg.DoModal();
}


HBRUSH CIntroDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    if (nCtlColor == CTLCOLOR_STATIC)
    {
        pDC->SetBkMode(TRANSPARENT); // 배경 투명
        pDC->SetTextColor(RGB(255, 255, 255)); // 글씨 색상 (흰색 추천)
        return (HBRUSH)GetStockObject(NULL_BRUSH); // 투명 브러시 반환
    }

    return hbr;
}


BOOL CIntroDlg::PreTranslateMessage(MSG* pMsg)
{
    // 키보드를 눌렀는데, 그게 '엔터(Enter)' 키라면?
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
        // 로그인 버튼 함수 강제 호출
        OnBnClickedButton1(); // (로그인 버튼 함수 이름)
        return TRUE; // "내가 처리했으니 윈도우는 신경 꺼" (삑 소리 방지)
    }

    return CDialogEx::PreTranslateMessage(pMsg);
}


void CIntroDlg::OnBnClickedButton2()
{
    // 1. 종료 확인 메시지 (실수로 눌렀을 때 방지)
    if (AfxMessageBox(_T("정말 게임을 종료하시겠습니까?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
    {
        // 2. 프로그램 완전 종료 요청
        // 이 함수를 호출하면 윈도우에게 "나 꺼줘"라고 신호를 보냅니다.
        PostQuitMessage(0);
    }
}

// CRankDlg.cpp: 구현 파일
//

#include "pch.h"
#include "1126.h"
#include "afxdialogex.h"
#include "CRankDlg.h"

// CRankDlg 대화 상자

IMPLEMENT_DYNAMIC(CRankDlg, CDialogEx)

CRankDlg::CRankDlg(CWnd *pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_RANK, pParent)
{
}

CRankDlg::~CRankDlg()
{
}

void CRankDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTScoreRank, m_listScore); // 점수 랭킹 리스트
	DDX_Control(pDX, IDC_LISTWinRank, m_listWin);	  // 승리 랭킹 리스트
}

BEGIN_MESSAGE_MAP(CRankDlg, CDialogEx)
// ★★★ [추가] 리스트 컨트롤 색상 변경을 위한 메시지 연결 ★★★
// IDC_LIST3 (점수)와 IDC_LIST1 (승리) 모두 같은 함수로 연결합니다.
ON_NOTIFY(NM_CUSTOMDRAW, IDC_LISTScoreRank, &CRankDlg::OnCustomdrawList)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_LISTWinRank, &CRankDlg::OnCustomdrawList)
END_MESSAGE_MAP()

// CRankDlg 메시지 처리기

BOOL CRankDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 1. 리스트 컨트롤 컬럼(제목) 만들기

	// [점수 랭킹]
	m_listScore.InsertColumn(0, _T("순위"), LVCFMT_CENTER, 60); // 너비 조금 늘림
	m_listScore.InsertColumn(1, _T("ID"), LVCFMT_CENTER, 100);
	m_listScore.InsertColumn(2, _T("최고 점수"), LVCFMT_CENTER, 100);

	// [승리 랭킹]
	m_listWin.InsertColumn(0, _T("순위"), LVCFMT_CENTER, 60); // 너비 조금 늘림
	m_listWin.InsertColumn(1, _T("ID"), LVCFMT_CENTER, 100);
	m_listWin.InsertColumn(2, _T("승리 횟수"), LVCFMT_CENTER, 80);
	m_listWin.InsertColumn(3, _T("게임 횟수"), LVCFMT_CENTER, 80);

	// 2. DB에서 데이터 가져오기
	LoadRankings();

	return TRUE;
}

// ★★★ [수정] 실제 DB 조회 및 출력 함수 (이모지 추가됨)
void CRankDlg::LoadRankings()
{
	CMy1126App *pApp = (CMy1126App *)AfxGetApp();

	CRecordset rs(&(pApp->m_db)); // DB 연결 객체 사용
	CString strQuery;
	int nItem = 0;
	CString strRank; // 순위 문자열

	// =========================================================
	// [1] 점수 랭킹 조회 (SCORE 높은 순서대로 10명)
	// =========================================================
	strQuery = _T("SELECT ID, SCORE FROM DC ORDER BY SCORE DESC LIMIT 10");

	try
	{
		rs.Open(CRecordset::forwardOnly, strQuery);
		int nRank = 1;

		while (!rs.IsEOF())
		{
			CString dbID, dbScore;
			rs.GetFieldValue((short)0, dbID);	 // ID
			rs.GetFieldValue((short)1, dbScore); // SCORE

			// ★★★ [수정] 1, 2, 3등에 이모지 붙이기 ★★★
			if (nRank == 1)
				strRank = _T("🥇 1위");
			else if (nRank == 2)
				strRank = _T("🥈 2위");
			else if (nRank == 3)
				strRank = _T("🥉 3위");
			else
				strRank.Format(_T("%d위"), nRank);

			// 리스트에 추가
			nItem = m_listScore.InsertItem(nRank - 1, strRank); // 행 추가
			m_listScore.SetItemText(nItem, 1, dbID);			// ID 설정
			m_listScore.SetItemText(nItem, 2, dbScore);			// 점수 설정

			nRank++; // 순위 증가
			rs.MoveNext();
		}
		rs.Close();
	}
	catch (CDBException *e)
	{
		e->Delete();
	}

	// =========================================================
	// [2] 승리 랭킹 조회 (WINS 높은 순서대로 10명)
	// =========================================================
	strQuery = _T("SELECT ID, WINS FROM DC ORDER BY WINS DESC LIMIT 10");

	try
	{
		rs.Open(CRecordset::forwardOnly, strQuery);
		int nRank = 1;

		while (!rs.IsEOF())
		{
			CString dbID, dbWins, dblose, Games;

			rs.GetFieldValue((short)0, dbID);
			rs.GetFieldValue((short)1, dbWins);
			rs.GetFieldValue((short)2, dblose);
			int totalGames = _ttoi(dbWins) + _ttoi(dblose);
			Games.Format(_T("%d"), totalGames);

			// ★★★ [수정] 1, 2, 3등에 이모지 붙이기 ★★★
			if (nRank == 1)
				strRank = _T("🥇 1위");
			else if (nRank == 2)
				strRank = _T("🥈 2위");
			else if (nRank == 3)
				strRank = _T("🥉 3위");
			else
				strRank.Format(_T("%d위"), nRank);

			nItem = m_listWin.InsertItem(nRank - 1, strRank);
			m_listWin.SetItemText(nItem, 1, dbID);
			m_listWin.SetItemText(nItem, 2, dbWins);
			m_listWin.SetItemText(nItem, 3, Games);

			nRank++;
			rs.MoveNext();
		}
		rs.Close();
	}
	catch (CDBException *e)
	{
		e->Delete();
	}
}

// ★★★ [추가] 리스트 컨트롤 색상 변경 함수 (금/은/동) ★★★
void CRankDlg::OnCustomdrawList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW *pLVCD = reinterpret_cast<NMLVCUSTOMDRAW *>(pNMHDR);
	*pResult = CDRF_DODEFAULT;

	switch (pLVCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;

	case CDDS_ITEMPREPAINT:
		// 행 번호 (0행 = 1위, 1행 = 2위 ...)
		int nRow = (int)pLVCD->nmcd.dwItemSpec;

		// 1위: 금색 배경
		if (nRow == 0)
		{
			pLVCD->clrTextBk = RGB(255, 223, 0); // 황금색
			pLVCD->clrText = RGB(0, 0, 0);		 // 검은 글씨
		}
		// 2위: 은색 배경
		else if (nRow == 1)
		{
			pLVCD->clrTextBk = RGB(211, 211, 211); // 은색(회색)
			pLVCD->clrText = RGB(0, 0, 0);
		}
		// 3위: 동색 배경
		else if (nRow == 2)
		{
			pLVCD->clrTextBk = RGB(205, 127, 50); // 동색(구리색)
			pLVCD->clrText = RGB(255, 255, 255);  // 흰색 글씨 (잘 보이게)
		}
		// 나머지: 흰색 배경
		else
		{
			pLVCD->clrTextBk = RGB(255, 255, 255);
			pLVCD->clrText = RGB(0, 0, 0);
		}

		*pResult = CDRF_NEWFONT;
		break;
	}
}
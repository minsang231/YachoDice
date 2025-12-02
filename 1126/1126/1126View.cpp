
// 1126View.cpp: CMy1126View 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "1126.h"
#endif

#include "1126Doc.h"
#include "1126View.h"

#include <math.h>
#include "CStartDlg.h"
#include "CIntroDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMy1126View

IMPLEMENT_DYNCREATE(CMy1126View, CFormView)

BEGIN_MESSAGE_MAP(CMy1126View, CFormView)
// 표준 인쇄 명령입니다.
ON_COMMAND(ID_FILE_PRINT, &CFormView::OnFilePrint)
ON_COMMAND(ID_FILE_PRINT_DIRECT, &CFormView::OnFilePrint)
ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CFormView::OnFilePrintPreview)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST2, &CMy1126View::OnNMCustomdrawList2)
ON_WM_TIMER()
ON_BN_CLICKED(IDC_BUTTON3, &CMy1126View::OnBnClickedButton3)
ON_WM_LBUTTONDOWN()
ON_NOTIFY(NM_CLICK, IDC_LIST2, &CMy1126View::OnClickList2)
ON_BN_CLICKED(IDC_BUTTON4, &CMy1126View::OnBnClickedButton4)
ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CMy1126View 생성/소멸

CMy1126View::CMy1126View() noexcept
	: CFormView(IDD_MY1126_FORM)
{
	// TODO: 여기에 생성 코드를 추가합니다.

	// ★★★ [추가] 미리보기 점수 배열 초기화 ★★★
	for (int i = 0; i < 15; i++)
	{
		m_nPreviewScores[i] = -1; // -1은 "표시 안 함"
		m_bCanScore[i] = FALSE;
	}
}

CMy1126View::~CMy1126View()
{
}

void CMy1126View::DoDataExchange(CDataExchange *pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, mlist);
}

BOOL CMy1126View::PreCreateWindow(CREATESTRUCT &cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	// ★★★ [추가] 전체 윈도우에 더블 버퍼링 적용 (깜빡임 제거) ★★★
	cs.dwExStyle |= WS_EX_COMPOSITED;

	return CFormView::PreCreateWindow(cs);
}

void CMy1126View::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	// =========================================================
	// ★★★ [추가] 버튼 폰트 설정 (크고 굵게) ★★★
	// =========================================================
	if (m_fontButton.GetSafeHandle() == NULL)
	{
		// 1. 기본 크기 설정 (250 = 25포인트, 원하는 크기로 조절 가능)
		CFont tempFont;
		tempFont.CreatePointFont(150, _T("맑은 고딕"));

		// 2. 폰트 속성 가져와서 굵게(Bold) 변경
		LOGFONT lf;
		tempFont.GetLogFont(&lf);
		lf.lfWeight = FW_BOLD; // 굵게 설정

		// 3. 최종 폰트 생성
		m_fontButton.CreateFontIndirect(&lf);
	}

	// 버튼 3 (굴리기)에 폰트 적용
	CWnd *pBtn3 = GetDlgItem(IDC_BUTTON3);
	if (pBtn3)
		pBtn3->SetFont(&m_fontButton);

	// 버튼 4 (나가기)에 폰트 적용
	CWnd *pBtn4 = GetDlgItem(IDC_BUTTON4);
	if (pBtn4)
		pBtn4->SetFont(&m_fontButton);
	// =========================================================

	// 1. 게임 모드 가져오기
	CMy1126App *pApp = (CMy1126App *)AfxGetApp();
	m_nGameMode = pApp->m_nGameMode;

	// 2. 리스트 스타일 설정
	mlist.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

	// 3. 리스트 크기 및 컬럼 설정
	CRect rect;
	mlist.GetClientRect(&rect);
	int nTotalWidth = rect.Width();
	int nTotalHeight = rect.Height();

	// 헤더 높이 계산
	int nHeaderHeight = 0;
	if (mlist.GetHeaderCtrl())
	{
		CRect rectHeader;
		mlist.GetHeaderCtrl()->GetClientRect(&rectHeader);
		nHeaderHeight = rectHeader.Height();
	}
	if (nHeaderHeight <= 0)
		nHeaderHeight = 25;

	// 행 높이 설정 (전체 높이에서 헤더 빼고 15등분)
	int nRowHeight = (nTotalHeight - nHeaderHeight - 5) / 15;
	if (nRowHeight < 15)
		nRowHeight = 15;

	m_ListHeightAdjust.DeleteImageList();
	m_ListHeightAdjust.Create(1, nRowHeight, ILC_COLOR, 1, 1);
	mlist.SetImageList(&m_ListHeightAdjust, LVSIL_SMALL);

	// 컬럼 비율 설정
	int nCol1, nCol2, nCol3;

	if (m_nGameMode == 1) // 1인용
	{
		nCol1 = nTotalWidth * 0.6;
		nCol2 = nTotalWidth - nCol1;
		nCol3 = 0;
	}
	else // 2인용
	{
		nCol1 = nTotalWidth * 0.4;
		nCol2 = nTotalWidth * 0.3;
		nCol3 = nTotalWidth - nCol1 - nCol2;
	}

	// 컬럼 생성 또는 업데이트
	if (mlist.GetHeaderCtrl()->GetItemCount() == 0)
	{
		mlist.InsertColumn(0, _T("항목"), LVCFMT_LEFT, nCol1);
		mlist.InsertColumn(1, _T("PLAYER 1"), LVCFMT_CENTER, nCol2);
		mlist.InsertColumn(2, _T("PLAYER 2"), LVCFMT_CENTER, nCol3);
	}
	else
	{
		mlist.SetColumnWidth(0, nCol1);
		mlist.SetColumnWidth(1, nCol2);
		mlist.SetColumnWidth(2, nCol3);
	}

	// 아이템 초기화
	mlist.DeleteAllItems();
	CString strItems[] = {_T("원 (Ones)"), _T("투 (Twos)"), _T("쓰리 (Threes)"), _T("포 (Fours)"), _T("파이브 (Fives)"), _T("식스 (Sixes)"), _T("합계 (Sum)"), _T("보너스 (Bonus)"), _T("세 개 같은 수"), _T("네 개 같은 수"), _T("풀 하우스"), _T("작은 스트레이트"), _T("큰 스트레이트"), _T("요트"), _T("찬스")};
	for (int i = 0; i < 15; i++)
		mlist.InsertItem(i, strItems[i]);

	// 변수 초기화
	for (int p = 0; p < 2; p++)
	{
		for (int i = 0; i < 15; i++)
			m_bScoreFixed[p][i] = FALSE;
		m_bYachtFixed[p] = FALSE;
	}
	m_nCurrentPlayer = 0;

	// ---------------------------------------------------------
	// 이미지 로드 (PNG 파일에서 상대 경로로 로드)
	// ---------------------------------------------------------

	// 실행 파일 경로 얻기
	TCHAR szExePath[MAX_PATH];
	GetModuleFileName(NULL, szExePath, MAX_PATH);

	// 파일명 제거하고 폴더 경로만 남기기
	CString strExePath(szExePath);
	int nPos = strExePath.ReverseFind(_T('\\'));
	if (nPos != -1)
		strExePath = strExePath.Left(nPos);

	CString strResPath;
	strResPath.Format(_T("%s\\res\\"), (LPCTSTR)strExePath);

	// 주사위 이미지 로드 (PNG 파일)
	for (int i = 1; i <= 6; i++)
	{
		if (m_image[i].IsNull())
		{
			CString strFile;
			strFile.Format(_T("%sdice%d.png"), (LPCTSTR)strResPath, i);
			HRESULT hr = m_image[i].Load(strFile);
			if (FAILED(hr))
			{
				AfxMessageBox(_T("주사위 이미지 로드 실패: ") + strFile);
			}
		}
	}

	// 배경 이미지 로드 (PNG 파일)
	if (m_imgBg.IsNull())
	{
		CString strBgFile;
		strBgFile.Format(_T("%sBackGround.png"), (LPCTSTR)strResPath);
		HRESULT hr = m_imgBg.Load(strBgFile);
		if (FAILED(hr))
		{
			AfxMessageBox(_T("배경 이미지 로드 실패: ") + strBgFile);
		}
	}
	// ---------------------------------------------------------

	// 주사위 및 애니메이션 상태 초기화
	m_bIsAnimating = FALSE;
	m_nAniFrame = 0;
	for (int i = 0; i < 5; i++)
	{
		m_dices[i].x = 0;
		m_dices[i].y = 0;
		m_dices[i].value = 1;
		m_dices[i].angle = 0;
		m_dices[i].bRolling = FALSE;
		m_dices[i].bKeep = FALSE;
	}

	// 게임 시작 준비 (첫 턴 세팅)
	NextTurn();
	mlist.ShowScrollBar(SB_VERT, FALSE);
	SetTimer(1, 30, NULL);
	SetTimer(2, 1000, NULL);
}

// CMy1126View 인쇄

BOOL CMy1126View::OnPreparePrinting(CPrintInfo *pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CMy1126View::OnBeginPrinting(CDC * /*pDC*/, CPrintInfo * /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CMy1126View::OnEndPrinting(CDC * /*pDC*/, CPrintInfo * /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CMy1126View::OnPrint(CDC *pDC, CPrintInfo * /*pInfo*/)
{
	// TODO: 여기에 사용자 지정 인쇄 코드를 추가합니다.
}

// CMy1126View 진단

#ifdef _DEBUG
void CMy1126View::AssertValid() const
{
	CFormView::AssertValid();
}

void CMy1126View::Dump(CDumpContext &dc) const
{
	CFormView::Dump(dc);
}

CMy1126Doc *CMy1126View::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMy1126Doc)));
	return (CMy1126Doc *)m_pDocument;
}
#endif //_DEBUG

// CMy1126View 메시지 처리기

void CMy1126View::OnNMCustomdrawList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW *pLVCD = reinterpret_cast<NMLVCUSTOMDRAW *>(pNMHDR);
	*pResult = CDRF_DODEFAULT;

	switch (pLVCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;

	case CDDS_ITEMPREPAINT:
		*pResult = CDRF_NOTIFYSUBITEMDRAW | CDRF_NOTIFYPOSTPAINT;
		break;

	case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
	{
		// [1] 배경색 & 글자색 칠하기 (기존 코드 유지)
		int nRow = (int)pLVCD->nmcd.dwItemSpec;
		int nCol = pLVCD->iSubItem;

		// 배경색
		if (nRow == 6 || nRow == 7) // 합계/보너스
		{
			pLVCD->clrTextBk = RGB(100, 110, 120);
		}
		else
		{
			// ★★★ [추가] 현재 턴인 플레이어의 줄을 살짝 밝게 표시 ★★★
			// 항목(0)은 그대로,
			// 현재 플레이어가 0(나)이면 1번 컬럼, 1(상대)이면 2번 컬럼 강조
			if (nCol == m_nCurrentPlayer + 1)
			{
				// ★★★ [추가] 점수를 얻을 수 있는 족보는 연두색 하이라이트! ★★★
				if (m_bCanScore[nRow] == TRUE &&
					m_bScoreFixed[m_nCurrentPlayer][nRow] == FALSE)
				{
					pLVCD->clrTextBk = RGB(144, 238, 144); // 연두색 (Light Green)
				}
				// ★★★ [추가] 0점이지만 선택 가능한 칸은 연한 노란색 ★★★
				else if (m_nPreviewScores[nRow] == 0 &&
						 m_bScoreFixed[m_nCurrentPlayer][nRow] == FALSE &&
						 m_bRolled == TRUE)
				{
					pLVCD->clrTextBk = RGB(255, 255, 200); // 연한 노란색
				}
				// 기본 현재 플레이어 컨럼 색상
				else
				{
					pLVCD->clrTextBk = RGB(255, 250, 205); // 레몬 쉬폰색
				}
			}
			else
			{
				pLVCD->clrTextBk = RGB(240, 235, 230); // 기본 베이지색
			}
		}

		// 글자색
		if (nRow == 6 || nRow == 7)
		{
			pLVCD->clrText = RGB(255, 255, 255);
		}
		else
		{
			if (nCol == 0)
				pLVCD->clrText = RGB(0, 0, 0);
			else if (nCol == 1)
				pLVCD->clrText = m_bScoreFixed[0][nRow] ? RGB(0, 0, 0) : RGB(160, 160, 160);
			else if (nCol == 2)
				pLVCD->clrText = m_bScoreFixed[1][nRow] ? RGB(0, 0, 0) : RGB(160, 160, 160);
		}

		*pResult = CDRF_NEWFONT | CDRF_NOTIFYPOSTPAINT;
	}
	break;

	// ★★★ [수정] 좌표 누적 방식으로 선 긋기 (완벽한 격자) ★★★
	case CDDS_ITEMPOSTPAINT:
	{
		CDC *pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
		int nRow = (int)pLVCD->nmcd.dwItemSpec;

		// 현재 행의 전체 영역을 가져옴
		CRect rectItem;
		mlist.GetItemRect(nRow, &rectItem, LVIR_BOUNDS);

		// 검은색 펜 준비
		CPen penGrid(PS_SOLID, 1, RGB(0, 0, 0));
		CPen *pOldPen = pDC->SelectObject(&penGrid);

		// 1. 바닥 가로선 긋기
		pDC->MoveTo(rectItem.left, rectItem.bottom - 1);
		pDC->LineTo(rectItem.right, rectItem.bottom - 1);

		// 2. 세로선 긋기 (컬럼 너비를 더해가며 위치 계산)
		CHeaderCtrl *pHeader = mlist.GetHeaderCtrl();
		int nColCount = pHeader->GetItemCount();

		// 시작 위치 (맨 왼쪽)
		int currentX = rectItem.left;

		// [맨 왼쪽 벽] 먼저 긋기
		pDC->MoveTo(currentX, rectItem.top);
		pDC->LineTo(currentX, rectItem.bottom);

		// [각 컬럼 사이 벽] 긋기
		for (int i = 0; i < nColCount; i++)
		{
			// 해당 컬럼의 너비를 가져와서 x좌표를 이동
			int nColWidth = mlist.GetColumnWidth(i);
			currentX += nColWidth;

			// 이동한 위치(오른쪽 끝)에 선 긋기
			// (currentX - 1)을 해야 선이 겹치지 않고 예쁘게 나옴
			pDC->MoveTo(currentX - 1, rectItem.top);
			pDC->LineTo(currentX - 1, rectItem.bottom);
		}

		// 3. 맨 윗줄 천장 긋기 (0번 행일 때만)
		if (nRow == 0)
		{
			pDC->MoveTo(rectItem.left, rectItem.top);
			pDC->LineTo(rectItem.right, rectItem.top);
		}

		pDC->SelectObject(pOldPen);

		// ★★★ [추가] 미리보기 점수 표시 ★★★
		// 조건: 주사위를 굴렸고, 아직 확정 안 된 칸이고, 미리보기 점수가 있을 때
		if (m_bRolled &&
			nRow != 6 && nRow != 7 &&
			m_bScoreFixed[m_nCurrentPlayer][nRow] == FALSE &&
			m_nPreviewScores[nRow] >= 0)
		{
			// 현재 플레이어의 컨럼 영역 계산
			int nCol = m_nCurrentPlayer + 1;
			CRect rectSubItem;
			mlist.GetSubItemRect(nRow, nCol, LVIR_BOUNDS, rectSubItem);

			// 미리보기 점수 문자열
			CString strPreview;
			strPreview.Format(_T("%d"), m_nPreviewScores[nRow]);

			// 폰트 설정 (이탤릭체로 미리보기임을 표시)
			CFont fontPreview;
			fontPreview.CreateFont(
				16, 0, 0, 0,
				FW_BOLD, // 굵게
				TRUE,	 // 이탤릭
				FALSE, FALSE, DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS,
				_T("맑은 고딕"));

			CFont *pOldFont = pDC->SelectObject(&fontPreview);

			// 배경 투명
			pDC->SetBkMode(TRANSPARENT);

			// 점수가 0보다 크면 진한 초록색, 0이면 회색
			if (m_nPreviewScores[nRow] > 0)
				pDC->SetTextColor(RGB(0, 100, 0)); // 진한 초록색
			else
				pDC->SetTextColor(RGB(150, 150, 150)); // 회색

			// 중앙 정렬로 텍스트 출력
			pDC->DrawText(strPreview, &rectSubItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			pDC->SelectObject(pOldFont);
			fontPreview.DeleteObject();
		}

		*pResult = CDRF_DODEFAULT;
	}
	break;
	}
}

void CMy1126View::DrawGame()
{
	// 1. 그릴 대상(픽처 컨트롤) 찾기
	CWnd *pGameScreen = GetDlgItem(IDC_GAME_SCREEN);
	if (pGameScreen == nullptr)
		return;

	// 2. 픽처 컨트롤 크기
	CClientDC dc(pGameScreen);
	CRect rect;
	pGameScreen->GetClientRect(&rect);

	int CX = rect.Width() / 2;
	int CY = rect.Height() / 2;

	// 3. 더블 버퍼링 준비
	CDC memDC;
	CBitmap bitmap;
	memDC.CreateCompatibleDC(&dc);
	bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	memDC.SelectObject(&bitmap);

	// ==========================================================
	// ★★★ [핵심 수정] 배경 비율 정확히 계산 (검은색 제거) ★★★
	// ==========================================================

	// (1) 일단 안전하게 갈색으로 전체를 칠해둡니다 (빈틈 방지용)
	memDC.FillSolidRect(&rect, RGB(60, 40, 20));

	// (2) 배경 이미지가 있다면, 비율에 맞춰 정확한 위치를 잘라옵니다.
	if (!m_imgBg.IsNull())
	{
		// 1. 전체 화면(View)의 크기를 구합니다.
		CRect rectView;
		GetClientRect(&rectView);

		// 2. 픽처 컨트롤이 전체 화면에서 어디에 있는지 구합니다.
		CRect rectCtrl;
		pGameScreen->GetWindowRect(&rectCtrl);
		ScreenToClient(&rectCtrl);

		// 3. 비율 계산 (원본 이미지 크기 / 현재 화면 크기)
		// 화면 해상도와 이미지 해상도가 다르기 때문에 이 비율을 곱해야 정확한 위치가 나옵니다.
		double ratioX = (double)m_imgBg.GetWidth() / (double)rectView.Width();
		double ratioY = (double)m_imgBg.GetHeight() / (double)rectView.Height();

		// 4. 이미지 파일 안에서의 좌표(Source) 계산
		int srcX = (int)(rectCtrl.left * ratioX);
		int srcY = (int)(rectCtrl.top * ratioY);
		int srcW = (int)(rect.Width() * ratioX);
		int srcH = (int)(rect.Height() * ratioY);

		// ★ 안전장치: 계산된 너비가 이미지 범위를 넘지 않게 보정
		if (srcX + srcW > m_imgBg.GetWidth())
			srcW = m_imgBg.GetWidth() - srcX;
		if (srcY + srcH > m_imgBg.GetHeight())
			srcH = m_imgBg.GetHeight() - srcY;

		// 5. 그리기
		m_imgBg.Draw(memDC.m_hDC,
					 0, 0, rect.Width(), rect.Height(), // 목적지 (픽처 컨트롤 전체)
					 srcX, srcY, srcW, srcH);			// 원본 (계산된 위치)
	}

	// ==========================================================
	// [디자인] 테이블 그리기 (기존 코드 유지)
	// ==========================================================

	CBrush brushFelt(RGB(34, 100, 34));
	CPen penBorder(PS_SOLID, 15, RGB(139, 69, 19));

	CPen *pOldPen = memDC.SelectObject(&penBorder);
	CBrush *pOldBrush = (CBrush *)memDC.SelectObject(&brushFelt);

	// 둥근 모서리 사각형
	memDC.RoundRect(rect.left + 10, rect.top + 10, rect.right - 10, rect.bottom - 10, 60, 60);

	memDC.SelectObject(pOldPen);
	memDC.SelectObject(pOldBrush);

	// ==========================================================
	// [로직] 주사위 그리기 (기존 코드 유지)
	// ==========================================================

	SetGraphicsMode(memDC.m_hDC, GM_ADVANCED);

	int nDiceSize = 60; // 90 -> 60으로 축소
	int nGapX = 80;		// 간격도 축소

	for (int i = 0; i < 5; i++)
	{
		int targetX = 0;
		int targetY = 0;

		// 물리 애니메이션 중이면 계산된 위치 사용
		if (m_bIsAnimating && m_dices[i].bRolling)
		{
			targetX = (int)m_dices[i].x;
			targetY = (int)m_dices[i].y;
		}
		else
		{
			// 정지 상태일 때 고정 위치
			if (i == 0)
			{
				targetX = CX - nGapX;
				targetY = CY - 55;
			}
			if (i == 1)
			{
				targetX = CX;
				targetY = CY - 55;
			}
			if (i == 2)
			{
				targetX = CX + nGapX;
				targetY = CY - 55;
			}
			if (i == 3)
			{
				targetX = CX - 55;
				targetY = CY + 55;
			}
			if (i == 4)
			{
				targetX = CX + 55;
				targetY = CY + 55;
			}

			if (m_bIsAnimating && m_dices[i].bRolling)
			{
				targetX += (rand() % 20 - 10);
				targetY += (rand() % 20 - 10);
			}

			int nVal = m_dices[i].value;

			if (nVal >= 1 && nVal <= 6 && !m_image[nVal].IsNull())
			{
				int nSaveDC = memDC.SaveDC();

				float radian = m_dices[i].angle * 3.14159f / 180.0f;
				float fCos = cos(radian);
				float fSin = sin(radian);

				XFORM xForm;
				xForm.eM11 = fCos;
				xForm.eM12 = fSin;
				xForm.eM21 = -fSin;
				xForm.eM22 = fCos;
				xForm.eDx = (float)targetX;
				xForm.eDy = (float)targetY;

				SetWorldTransform(memDC.m_hDC, &xForm);

				m_image[nVal].Draw(memDC.m_hDC, -nDiceSize / 2, -nDiceSize / 2, nDiceSize, nDiceSize);

				// Keep 상태 테두리
				if (m_dices[i].bKeep)
				{
					CPen penKeep(PS_SOLID, 5, RGB(255, 255, 0));
					CPen *pKeepOldPen = memDC.SelectObject(&penKeep);
					CBrush *pKeepOldBrush = (CBrush *)memDC.SelectStockObject(NULL_BRUSH);

					memDC.Rectangle(-nDiceSize / 2, -nDiceSize / 2, nDiceSize / 2, nDiceSize / 2);

					memDC.SelectObject(pKeepOldPen);
					memDC.SelectObject(pKeepOldBrush);
				}

				memDC.RestoreDC(nSaveDC);
			}
		}

		dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
	}

	void CMy1126View::OnTimer(UINT_PTR nIDEvent)
	{
		if (nIDEvent == 1)
		{
			// 1. 애니메이션 시간 관리
			if (m_bIsAnimating)
			{
				m_nAniFrame--;

				// 2. 픽처 컨트롤 크기 구하기
				CWnd *pGameScreen = GetDlgItem(IDC_GAME_SCREEN);
				if (pGameScreen)
				{
					CRect rect;
					pGameScreen->GetClientRect(&rect);
					int nDiceSize = 60;
					float friction = 0.98f;	  // 마찰력 (속도 감쇠)
					float restitution = 0.7f; // 반발 계수 (튕김 정도)

					// 3. 물리 시뮬레이션
					for (int i = 0; i < 5; i++)
					{
						if (m_dices[i].bRolling)
						{
							// 위치 업데이트
							m_dices[i].x += m_dices[i].vx;
							m_dices[i].y += m_dices[i].vy;

							// 마찰력 적용 (속도 감소)
							m_dices[i].vx *= friction;
							m_dices[i].vy *= friction;

							// 회전 업데이트
							m_dices[i].angle += m_dices[i].angleSpeed;
							m_dices[i].angleSpeed *= friction;

							// 주사위 눈금 무작위 변경 (굴리는 효과)
							if (m_nAniFrame > 10) // 마지막 10프레임은 값 고정
								m_dices[i].value = (rand() % 6) + 1;

							// 벽 충돌 감지 (픽처 컨트롤 경계)
							float halfSize = nDiceSize / 2.0f;

							// 왼쪽 벽
							if (m_dices[i].x - halfSize < rect.left + 10)
							{
								m_dices[i].x = rect.left + 10 + halfSize;
								m_dices[i].vx = -m_dices[i].vx * restitution;
							}
							// 오른쪽 벽
							if (m_dices[i].x + halfSize > rect.right - 10)
							{
								m_dices[i].x = rect.right - 10 - halfSize;
								m_dices[i].vx = -m_dices[i].vx * restitution;
							}
							// 위쪽 벽
							if (m_dices[i].y - halfSize < rect.top + 10)
							{
								m_dices[i].y = rect.top + 10 + halfSize;
								m_dices[i].vy = -m_dices[i].vy * restitution;
							}
							// 아래쪽 벽
							if (m_dices[i].y + halfSize > rect.bottom - 10)
							{
								m_dices[i].y = rect.bottom - 10 - halfSize;
								m_dices[i].vy = -m_dices[i].vy * restitution;
							}
						}
					}

					// 4. 주사위 간 충돌 감지
					for (int i = 0; i < 5; i++)
					{
						if (!m_dices[i].bRolling)
							continue;

						for (int j = i + 1; j < 5; j++)
						{
							if (!m_dices[j].bRolling)
								continue;

							// 두 주사위 간 거리 계산
							float dx = m_dices[j].x - m_dices[i].x;
							float dy = m_dices[j].y - m_dices[i].y;
							float distance = sqrt(dx * dx + dy * dy);
							float minDist = (float)nDiceSize; // 충돌 거리

							// 충돌 발생!
							if (distance < minDist && distance > 0)
							{
								// 충돌 법선 벡터
								float nx = dx / distance;
								float ny = dy / distance;

								// 겹침 해소 (주사위 분리)
								float overlap = minDist - distance;
								m_dices[i].x -= nx * overlap * 0.5f;
								m_dices[i].y -= ny * overlap * 0.5f;
								m_dices[j].x += nx * overlap * 0.5f;
								m_dices[j].y += ny * overlap * 0.5f;

								// 속도 교환 (탄성 충돌)
								float vix = m_dices[i].vx;
								float viy = m_dices[i].vy;
								float vjx = m_dices[j].vx;
								float vjy = m_dices[j].vy;

								// 법선 방향 속도 성분
								float vi_n = vix * nx + viy * ny;
								float vj_n = vjx * nx + vjy * ny;

								// 속도 교환 (반발 계수 적용)
								m_dices[i].vx += (vj_n - vi_n) * nx * restitution;
								m_dices[i].vy += (vj_n - vi_n) * ny * restitution;
								m_dices[j].vx += (vi_n - vj_n) * nx * restitution;
								m_dices[j].vy += (vi_n - vj_n) * ny * restitution;
							}
						}
					}
				}

				// 5. 시간 종료 체크
				if (m_nAniFrame <= 0)
				{
					m_bIsAnimating = FALSE;

					for (int i = 0; i < 5; i++)
					{
						// ★★★ [수정] 굴러가던 주사위만 최종 값을 확정해야 함! ★★★
						if (m_dices[i].bRolling)
						{
							m_dices[i].bRolling = FALSE; // 이제 멈춤
							m_dices[i].angle = 0;		 // 각도 정렬
							m_dices[i].vx = 0;
							m_dices[i].vy = 0;
							m_dices[i].angleSpeed = 0;
						}
						// Keep(bRolling == FALSE)이었던 애들은 건드리지 않음 -> 값 유지됨
					}

					// ★★★ [추가] 애니메이션 끝나면 미리보기 점수 업데이트! ★★★
					UpdateScorePreview();
				}
			}

			// 4. 화면 갱신 요청 (DrawGame 호출)
			DrawGame();
		}

		// ★★★ [핵심 수정] 타이머 2: 깜빡임 방지를 위해 텍스트 영역만 무효화 ★★★
		if (nIDEvent == 2)
		{
			// 시간이 남아있으면 1초씩 뺌
			if (m_nLeftTime > 0)
			{
				m_nLeftTime--;

				// 시간이 0이 되면? (강제로 턴을 넘기거나 메시지 띄우기)
				if (m_nLeftTime == 0)
				{
					// 예: 시간 초과 처리 (필요하면 주석 해제하여 사용)
					// AfxMessageBox(_T("시간 초과! 턴이 넘어갑니다."));
					// OnBnClickedButton3(); // 강제로 굴리거나 등등
				}

				// ★★★ [핵심 수정] 전체 화면 대신 텍스트 영역만 무효화 ★★★
				// 이렇게 하면 리스트뷰와 버튼이 깜빡이지 않습니다!

				CRect rectClient;
				GetClientRect(&rectClient);

				// 상단 텍스트 영역만 무효화 (플레이어 표시 + 타이머)
				// y=0 ~ y=60 정도의 상단 영역만 다시 그림
				CRect rectTextArea(0, 0, rectClient.right, 60);
				InvalidateRect(&rectTextArea, FALSE);
			}
		}

		CFormView::OnTimer(nIDEvent);
	}

	void CMy1126View::OnDraw(CDC * pDC)
	{
		CMy1126Doc *pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		if (!pDoc)
			return;

		// 1. 클라이언트 영역 구하기
		CRect rectClient;
		GetClientRect(&rectClient);

		// ★★★ [추가] 더블 버퍼링 시작 ★★★
		CDC memDC;
		CBitmap memBitmap;
		memDC.CreateCompatibleDC(pDC);
		memBitmap.CreateCompatibleBitmap(pDC, rectClient.Width(), rectClient.Height());
		CBitmap *pOldBitmap = memDC.SelectObject(&memBitmap);

		// 2. 픽처 컨트롤(게임판) 영역 제외하고 배경 그리기 (기존 코드 유지)
		CWnd *pGameScreen = GetDlgItem(IDC_GAME_SCREEN);
		CRect rectGameScreen;
		if (pGameScreen)
		{
			pGameScreen->GetWindowRect(&rectGameScreen);
			ScreenToClient(&rectGameScreen);
		}

		if (!m_imgBg.IsNull())
		{
			if (pGameScreen)
				memDC.ExcludeClipRect(&rectGameScreen);
			m_imgBg.Draw(memDC.GetSafeHdc(), rectClient);
			memDC.SelectClipRgn(NULL);
		}

		// =========================================================
		// ★★★ [수정] 텍스트 그리기 (위치 변경 & 타이머 추가) ★★★
		// =========================================================

		// 폰트 설정 (기존에 만든 굵은 폰트가 있다면 그걸 써도 됩니다)
		CFont font;
		font.CreatePointFont(200, _T("Arial Bold")); // 크기 200
		CFont *pOldFont = memDC.SelectObject(&font);
		memDC.SetBkMode(TRANSPARENT); // 배경 투명

		// ---------------------------------------------------------
		// [1] 왼쪽 리스트 컨트롤 위에 "현재 플레이어" 표시
		// ---------------------------------------------------------
		CString strTurn;
		COLORREF clrText;

		CMy1126App *pApp = (CMy1126App *)AfxGetApp();
		CString strUserID = pApp->m_strCurrentUserID;

		if (m_nGameMode == 1) // 1인용
		{
			if (strUserID.IsEmpty())
			{
				strTurn = _T("Single Player (Guest)"); // 로그인 안 함
			}
			else
			{
				strTurn.Format(_T("[%s] 님의 게임"), strUserID); // 로그인 함
			}
			clrText = RGB(255, 255, 255);
		}
		else // 2인용
		{
			if (m_nCurrentPlayer == 0) // 내 차례 (왼쪽)
			{
				if (strUserID.IsEmpty())
					strTurn = _T("<<< Player 1 (나) <<<");
				else
					strTurn.Format(_T("<<< %s (나) <<<"), strUserID); // 내 아이디 표시

				clrText = RGB(255, 215, 0); // 금색
			}
			else // 상대방 차례 (오른쪽)
			{
				strTurn = _T(">>> Guest (상대) >>>"); // 상대는 무조건 게스트
				clrText = RGB(100, 100, 255);		  // 파란색
			}
		}

		// ★ 위치: 왼쪽(x=20), 위쪽(y=10) -> 리스트 컨트롤 윗부분
		memDC.SetTextColor(RGB(0, 0, 0)); // 그림자(검정)
		memDC.TextOut(22, 12, strTurn);
		memDC.SetTextColor(clrText); // 본문 색상
		memDC.TextOut(20, 10, strTurn);

		// ---------------------------------------------------------
		// [2] 화면 중앙(주사위 위)에 "남은 시간" 표시
		// ---------------------------------------------------------

		// 시간 문자열 만들기
		CString strTimer;
		strTimer.Format(_T("Time: %d"), m_nLeftTime);

		// 색상: 시간이 10초 이하로 남으면 빨간색으로 경고!
		COLORREF clrTimer = RGB(255, 255, 255); // 기본 흰색
		if (m_nLeftTime <= 10)
			clrTimer = RGB(255, 0, 0); // 빨간색

		// ★ 위치: 화면 가로 중앙 계산
		int nCenterX = rectClient.Width() / 2 + 100; // 살짝 오른쪽 보정 (리스트 컨트롤 때문)
		if (pGameScreen)
			nCenterX = rectGameScreen.CenterPoint().x; // 게임판이 있으면 그 중앙

		// 텍스트의 정중앙을 맞추기 위해 정렬 옵션 변경
		memDC.SetTextAlign(TA_CENTER);

		// 출력 (그림자 효과 포함)
		memDC.SetTextColor(RGB(0, 0, 0));
		memDC.TextOut(nCenterX + 2, 22, strTimer);

		memDC.SetTextColor(clrTimer);
		memDC.TextOut(nCenterX, 20, strTimer);

		// 정렬 원상복구 (중요)
		memDC.SetTextAlign(TA_LEFT);

		// ---------------------------------------------------------

		memDC.SelectObject(pOldFont);
		font.DeleteObject();

		// ★★★ [추가] 더블 버퍼링 완료: 메모리 DC를 화면에 복사 ★★★
		pDC->BitBlt(0, 0, rectClient.Width(), rectClient.Height(), &memDC, 0, 0, SRCCOPY);

		// 메모리 정리
		memDC.SelectObject(pOldBitmap);
		memBitmap.DeleteObject();
		memDC.DeleteDC();

		// 마지막으로 게임판(주사위) 그리기
		DrawGame();
	}

	void CMy1126View::OnBnClickedButton3()
	{
		// 1. 애니메이션 중이면 무시
		if (m_bIsAnimating)
			return;

		// ★★★ [추가] 3번 다 썼으면 경고하고 리턴 ★★★
		if (m_nRollCount >= 3)
		{
			AfxMessageBox(_T("기회를 모두 사용했습니다!\n왼쪽 점수판을 선택해서 점수를 등록하세요."));
			return;
		}

		// 2. 굴리기 시작
		m_bIsAnimating = TRUE;
		m_bRolled = TRUE; // [추가] 굴렸음! 이제 점수 등록 가능!
		m_nAniFrame = 90; // 애니메이션 시간 늘림 (30 -> 90)

		// 3. 굴리기 횟수 증가 및 UI 업데이트
		m_nRollCount++;

		CString strBtn;
		strBtn.Format(_T("굴리기 (남은 횟수: %d)"), 3 - m_nRollCount);
		SetDlgItemText(IDC_BUTTON3, strBtn);

		// 4. 픽처 컨트롤 크기 구하기 (물리 시뮬레이션용)
		CWnd *pGameScreen = GetDlgItem(IDC_GAME_SCREEN);
		if (!pGameScreen)
			return;

		CRect rect;
		pGameScreen->GetClientRect(&rect);
		int CX = rect.Width() / 2;
		int CY = rect.Height() / 2;

		// 5. 주사위 굴림 설정 및 초기 위치/속도 설정
		for (int i = 0; i < 5; i++)
		{
			if (m_dices[i].bKeep == false)
			{
				m_dices[i].bRolling = TRUE;

				// 초기 위치 설정 (화면 중앙 근처에서 랜덤하게)
				m_dices[i].x = (float)(CX + (rand() % 100 - 50));
				m_dices[i].y = (float)(CY + (rand() % 100 - 50));

				// 무작위 속도 설정 (방향과 속도)
				float speed = 5.0f + (rand() % 10);				  // 5~15 범위
				float angle = (rand() % 360) * 3.14159f / 180.0f; // 0~360도
				m_dices[i].vx = cos(angle) * speed;
				m_dices[i].vy = sin(angle) * speed;

				// 무작위 회전 속도
				m_dices[i].angleSpeed = (float)(rand() % 30 + 10); // 10~40
			}
			else
			{
				m_dices[i].bRolling = FALSE;
			}
		}

		// 3번 다 썼으면 버튼 비활성화 (회색으로 만들기)
		if (m_nRollCount >= 3)
		{
			GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
		}
	}

	void CMy1126View::OnLButtonDown(UINT nFlags, CPoint point)
	{
		// ★★★ [추가] 주사위 클릭 체크 (Keep 토글) ★★★

		// 1. 픽처 컨트롤 영역 구하기
		CWnd *pGameScreen = GetDlgItem(IDC_GAME_SCREEN);
		if (pGameScreen)
		{
			CRect rectGame;
			pGameScreen->GetWindowRect(&rectGame);
			ScreenToClient(&rectGame);

			// 2. 클릭 좌표가 게임판 안에 있는지 확인
			if (rectGame.PtInRect(point))
			{
				// 3. 게임판 내부 좌표로 변환
				int localX = point.x - rectGame.left;
				int localY = point.y - rectGame.top;

				int CX = rectGame.Width() / 2;
				int CY = rectGame.Height() / 2;

				int nDiceSize = 60; // DrawGame과 동일하게
				int nGapX = 80;

				// 4. 각 주사위 위치와 비교
				for (int i = 0; i < 5; i++)
				{
					int targetX = 0, targetY = 0;

					if (i == 0)
					{
						targetX = CX - nGapX;
						targetY = CY - 55;
					}
					if (i == 1)
					{
						targetX = CX;
						targetY = CY - 55;
					}
					if (i == 2)
					{
						targetX = CX + nGapX;
						targetY = CY - 55;
					}
					if (i == 3)
					{
						targetX = CX - 55;
						targetY = CY + 55;
					}
					if (i == 4)
					{
						targetX = CX + 55;
						targetY = CY + 55;
					}

					// 주사위 영역 계산
					CRect rectDice(
						targetX - nDiceSize / 2,
						targetY - nDiceSize / 2,
						targetX + nDiceSize / 2,
						targetY + nDiceSize / 2);

					// 5. 클릭했으면 Keep 상태 토글
					if (rectDice.PtInRect(CPoint(localX, localY)))
					{
						m_dices[i].bKeep = !m_dices[i].bKeep;
						DrawGame(); // 화면 갱신
						break;
					}
				}
			}
		}

		CFormView::OnLButtonDown(nFlags, point);
	}

	// ★★★ [추가] 다음 턴 준비 함수 ★★★
	void CMy1126View::NextTurn()
	{
		// 1. 굴린 횟수 초기화
		m_nRollCount = 0;
		m_bRolled = FALSE; // [추가] 아직 안 굴림!

		// 2. 주사위 상태 초기화
		for (int i = 0; i < 5; i++)
		{
			m_dices[i].bKeep = FALSE;
			m_dices[i].bRolling = FALSE;
			m_dices[i].angle = 0;
		}

		// 3. 버튼 텍스트 원상복구
		SetDlgItemText(IDC_BUTTON3, _T("굴리기 (남은 횟수: 3)"));

		// 4. 버튼 활성화
		GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);

		// 5. 화면 갱신
		DrawGame();

		// ▼▼▼ [수정] 6. 점수판 글씨 지우기 (초기화) ▼▼▼
		for (int i = 0; i < 15; i++)
		{
			// ★★★ [핵심 수정] 합계(6)랑 보너스(7)는 지우지 마! ★★★
			if (i == 6 || i == 7)
				continue;

			// 확정 안 된 나머지 칸들만 빈칸으로 만듦 (현재 플레이어 기준)
			if (m_bScoreFixed[m_nCurrentPlayer][i] == FALSE)
			{
				mlist.SetItemText(i, m_nCurrentPlayer + 1, _T(""));
			}
		}

		// ★★★ [추가] 미리보기 점수 초기화 ★★★
		for (int i = 0; i < 15; i++)
		{
			m_nPreviewScores[i] = -1;
			m_bCanScore[i] = FALSE;
		}

		m_nLeftTime = 40;

		Invalidate(FALSE);
	}

	int CMy1126View::GetScore(int nRow)
	{
		// 1. 주사위 눈금 개수 세기 (빈도수 배열)
		// counts[1]은 1의 개수, counts[6]은 6의 개수
		int counts[7] = {0};
		int nSumAll = 0; // 주사위 5개의 총합

		for (int i = 0; i < 5; i++)
		{
			int val = m_dices[i].value;
			if (val >= 1 && val <= 6)
				counts[val]++;
			nSumAll += val;
		}

		// 2. 족보별 점수 계산
		switch (nRow)
		{
		case 0:
			return counts[1] * 1; // Ones
		case 1:
			return counts[2] * 2; // Twos
		case 2:
			return counts[3] * 3; // Threes
		case 3:
			return counts[4] * 4; // Fours
		case 4:
			return counts[5] * 5; // Fives
		case 5:
			return counts[6] * 6; // Sixes

			// 6, 7번은 합계/보너스 칸이므로 계산 안 함
		case 6:
			return 0;
		case 7:
			return 0;

		case 8: // Three of a Kind (3개 이상 같으면 총합)
			for (int i = 1; i <= 6; i++)
			{
				if (counts[i] >= 3)
					return nSumAll;
			}
			return 0;

		case 9: // Four of a Kind (4개 이상 같으면 총합)
			for (int i = 1; i <= 6; i++)
			{
				if (counts[i] >= 4)
					return nSumAll;
			}
			return 0;

		case 10: // Full House (3개 같고 + 2개 같음) -> 점수 25점
		{
			bool b3 = false; // 3개짜리가 있는가?
			bool b2 = false; // 2개짜리가 있는가?
			for (int i = 1; i <= 6; i++)
			{
				if (counts[i] == 3)
					b3 = true;
				if (counts[i] == 2)
					b2 = true;
				if (counts[i] == 5)
					return 25; // 5개 다 같아도 풀하우스 인정 (Yacht is FH)
			}
			return (b3 && b2) ? 25 : 0;
		}

		case 11: // Small Straight (4개 연속) -> 점수 30점
			// 가능한 경우의 수: 1234, 2345, 3456
			if (counts[1] && counts[2] && counts[3] && counts[4])
				return 30;
			if (counts[2] && counts[3] && counts[4] && counts[5])
				return 30;
			if (counts[3] && counts[4] && counts[5] && counts[6])
				return 30;
			return 0;

		case 12: // Large Straight (5개 연속) -> 점수 40점
			// 가능한 경우의 수: 12345, 23456
			if (counts[1] && counts[2] && counts[3] && counts[4] && counts[5])
				return 40;
			if (counts[2] && counts[3] && counts[4] && counts[5] && counts[6])
				return 40;
			return 0;

		case 13: // Yacht (5개 모두 같음) -> 점수 50점
			for (int i = 1; i <= 6; i++)
			{
				if (counts[i] == 5) // 5개가 다 똑같다면?
				{
					// ★ 이미 요트를 한 번 따놨으면(50점 기록했으면) -> 100점!
					if (m_bYachtFixed[m_nCurrentPlayer] == TRUE)
						return 100;

					// 처음이면 -> 50점
					return 50;
				}
			}
			return 0;

		case 14: // Chance (무조건 총합)
			return nSumAll;
		}

		return 0;
	}

	void CMy1126View::OnClickList2(NMHDR * pNMHDR, LRESULT * pResult)
	{
		LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
		*pResult = 0;

		int nRow = pNMItemActivate->iItem; // 클릭한 행 번호

		// 1. 유효성 검사
		if (nRow == -1)
			return; // 빈공간 클릭
		if (nRow == 6 || nRow == 7)
			return; // 합계/보너스 칸 클릭 금지

		if (m_bRolled == FALSE)
		{
			AfxMessageBox(_T("먼저 주사위를 굴려주세요! 🎲"));
			return;
		}

		// ★ [중요] '현재 차례인 사람'이 이 칸을 이미 채웠는지 확인
		if (m_bScoreFixed[m_nCurrentPlayer][nRow] == TRUE)
			return;

		// 2. 점수 계산 및 입력
		int nScore = GetScore(nRow);
		CString strScore;
		strScore.Format(_T("%d"), nScore);

		// ★ 컬럼 선택: 플레이어 0이면 1번열(나), 1이면 2번열(상대)
		int nCol = m_nCurrentPlayer + 1;
		mlist.SetItemText(nRow, nCol, strScore);

		// 확정 플래그 설정
		m_bScoreFixed[m_nCurrentPlayer][nRow] = TRUE;

		// 요트(50점) 확정 시 해당 플레이어에게 보너스 플래그 켜기
		if (nRow == 13 && nScore == 50)
		{
			m_bYachtFixed[m_nCurrentPlayer] = TRUE;
		}

		// 3. 상단 합계(Sum) 및 보너스 자동 계산 (현재 플레이어 것만)
		int nSubTotal = 0;

		// 0~5번(Ones~Sixes) 점수 합산
		for (int i = 0; i <= 5; i++)
		{
			if (m_bScoreFixed[m_nCurrentPlayer][i] == TRUE)
			{
				CString strVal = mlist.GetItemText(i, nCol);
				nSubTotal += _ttoi(strVal);
			}
		}

		// 6번 줄(합계) 업데이트
		CString strSum;
		strSum.Format(_T("%d / 63"), nSubTotal);
		mlist.SetItemText(6, nCol, strSum);

		// 7번 줄(보너스) 업데이트
		if (nSubTotal >= 63)
		{
			mlist.SetItemText(7, nCol, _T("35"));
		}
		else
		{
			mlist.SetItemText(7, nCol, _T("0"));
		}

		// =========================================================
		// ★★★ [4. 게임 종료 체크 및 턴 넘기기] ★★★
		// =========================================================

		// 게임이 끝났는지(TRUE) 확인하고, 아직 안 끝났으면(FALSE) 턴 교체
		if (CheckGameOver() == FALSE)
		{
			// ★ 2인용 모드일 때만 플레이어 교체!
			if (m_nGameMode == 2)
			{
				m_nCurrentPlayer = 1 - m_nCurrentPlayer;

				if (m_nCurrentPlayer == 0)
					AfxMessageBox(_T("당신(나)의 차례입니다! 🎲"));
				else
					AfxMessageBox(_T("상대방의 차례입니다! 🎲"));
			}

			// 1인용이면 m_nCurrentPlayer는 계속 0으로 유지됨 -> 내 점수판에만 계속 기록

			NextTurn();

			mlist.SetItemState(nRow, 0, LVIS_SELECTED | LVIS_FOCUSED);
		}
	}

	BOOL CMy1126View::CheckGameOver()
	{
		// 1. 아직 빈칸이 있는지 검사
		int nMaxPlayer = (m_nGameMode == 1) ? 1 : 2;
		for (int p = 0; p < nMaxPlayer; p++)
		{
			for (int i = 0; i < 15; i++)
			{
				if (i == 6 || i == 7)
					continue;
				if (m_bScoreFixed[p][i] == FALSE)
					return FALSE; // 빈칸 있으면 계속 진행
			}
		}

		// ======================================================
		// ★ 게임 종료! 점수 집계 ★
		// ======================================================

		int nScoreMe = 0;
		int nScoreOpp = 0;

		// 내 점수 계산
		for (int i = 0; i < 15; i++)
		{
			if (i == 6)
				continue;
			nScoreMe += _ttoi(mlist.GetItemText(i, 1));
		}

		// 2인용일 때만 상대 점수 계산
		if (m_nGameMode == 2)
		{
			for (int i = 0; i < 15; i++)
			{
				if (i == 6)
					continue;
				nScoreOpp += _ttoi(mlist.GetItemText(i, 2));
			}
		}

		// ★★★ [추가] DB에 결과 저장! (이제 에러 안 남) ★★★
		SaveResultToDB(nScoreMe, nScoreOpp);

		CString strMsg;

		if (m_nGameMode == 1) // 1인용 결과창
		{
			strMsg.Format(_T("게임 종료!\n\n🏆 최종 점수: [ %d점 ]\n\n(로그인 시 최고 점수가 갱신됩니다)"), nScoreMe);
		}
		else // 2인용 결과창
		{
			strMsg.Format(_T("게임 종료!\n\n[나] : %d점\n[상대] : %d점\n\n"), nScoreMe, nScoreOpp);
			if (nScoreMe > nScoreOpp)
				strMsg += _T("축하합니다! 승리했습니다! 🏆");
			else if (nScoreMe < nScoreOpp)
				strMsg += _T("패배했습니다. 😢");
			else
				strMsg += _T("무승부입니다!");
		}

		// 여기에 안내 메시지 추가
		strMsg += _T("\n\n새 게임을 시작하시겠습니까?");

		// 재시작 확인
		if (AfxMessageBox(strMsg, MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			OnInitialUpdate();
		}
		else
		{
			GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
			SetDlgItemText(IDC_BUTTON3, _T("게임 종료"));
		}

		return TRUE;
	}

	void CMy1126View::OnBnClickedButton4()
	{
		// 1. 종료 확인 메시지
		if (AfxMessageBox(_T("현재 게임을 중단하고 메인 화면으로 돌아가시겠습니까?"),
						  MB_YESNO | MB_ICONQUESTION) == IDNO)
		{
			return; // '아니요' 누르면 게임 계속
		}

		// 2. 메인 윈도우(게임 화면) 숨기기
		// 이렇게 하면 화면에서 게임이 싹 사라집니다.
		CWnd *pMainWnd = AfxGetMainWnd();
		if (pMainWnd)
		{
			pMainWnd->ShowWindow(SW_HIDE);
		}

		while (TRUE)
		{
			// 1. 모드 선택 화면 띄우기
			CStartDlg startDlg;
			INT_PTR nStartResult = startDlg.DoModal();

			if (nStartResult == IDOK)
			{
				// [1인/2인 선택 함] -> 게임 재시작
				CMy1126App *pApp = (CMy1126App *)AfxGetApp();
				pApp->m_nGameMode = startDlg.m_nSelectedMode;

				// 게임 화면 다시 보이기 & 초기화
				if (pMainWnd)
					pMainWnd->ShowWindow(SW_SHOW);
				OnInitialUpdate();

				break; // 반복문 탈출 (게임으로 돌아감)
			}
			else if (nStartResult == IDCANCEL)
			{
				// [메인 화면으로 가기] 버튼을 누름 -> 인트로 화면 띄우기
				CIntroDlg introDlg;

				// 인트로에서 다시 [게임 시작]을 누르면? -> 루프 돌아서 CStartDlg가 다시 뜸
				if (introDlg.DoModal() != IDOK)
				{
					// 인트로에서도 닫기(X)나 종료를 누르면 -> 진짜 프로그램 종료
					PostQuitMessage(0);
					break;
				}
				// 인트로에서 OK(게임시작) 누르면 while문 처음으로 돌아가서 CStartDlg 뜸
			}
		}
	}

	// ★★★ [추가] 배경 지우기 방지 (깜빡임 제거) ★★★
	BOOL CMy1126View::OnEraseBkgnd(CDC * pDC)
	{
		// 배경을 지우지 않음 (OnDraw에서 모두 그리므로)
		// 이렇게 하면 깜빡임이 사라집니다
		return TRUE;
	}

	// ★★★ [추가] 점수 미리보기 업데이트 함수 ★★★
	void CMy1126View::UpdateScorePreview()
	{
		// 아직 주사위를 안 굴렸으면 미리보기 안 함
		if (m_bRolled == FALSE)
		{
			for (int i = 0; i < 15; i++)
			{
				m_nPreviewScores[i] = -1; // -1은 "표시 안 함"
				m_bCanScore[i] = FALSE;
			}
			mlist.Invalidate(FALSE);
			return;
		}

		// 각 족보별 점수 계산
		for (int i = 0; i < 15; i++)
		{
			// 합계(6), 보너스(7)는 건너뛰
			if (i == 6 || i == 7)
			{
				m_nPreviewScores[i] = -1;
				m_bCanScore[i] = FALSE;
				continue;
			}

			// 이미 확정된 칸은 미리보기 안 함
			if (m_bScoreFixed[m_nCurrentPlayer][i] == TRUE)
			{
				m_nPreviewScores[i] = -1;
				m_bCanScore[i] = FALSE;
				continue;
			}

			// 점수 계산
			int nScore = GetScore(i);
			m_nPreviewScores[i] = nScore;

			// 0점 초과면 하이라이트 대상
			m_bCanScore[i] = (nScore > 0) ? TRUE : FALSE;
		}

		// 리스트 컨트롤 다시 그리기
		mlist.Invalidate(FALSE);
	}

	void CMy1126View::SaveResultToDB(int myScore, int oppScore)
	{
		// 1. 로그인 정보 가져오기
		CMy1126App *pApp = (CMy1126App *)AfxGetApp();

		// ★★★ [수정] 변수 이름을 strUserID 로 통일했습니다! ★★★
		CString strUserID = pApp->m_strCurrentUserID;

		// 로그인 안 했으면 저장 안 함
		if (strUserID.IsEmpty())
			return;

		// 2. DB 연결
		CDatabase db;
		try
		{
			// DSN, ID, PW 확인 필수
			db.OpenEx(_T("DSN=dice;UID=swuser02;PWD=SWUser02;PORT=32065;DATABASE=swuser02;"), CDatabase::noOdbcDialog);
		}
		catch (CDBException *e)
		{
			e->Delete();
			return;
		}

		// 3. 쿼리 만들기
		CString strQuery;

		if (m_nGameMode == 1)
		{
			// 1인용: 여기서도 strUserID 사용
			strQuery.Format(_T("UPDATE DC SET SCORE = %d WHERE ID = '%s' AND SCORE < %d"), myScore, strUserID, myScore);
		}
		else
		{
			// 2인용: 여기서도 strUserID 사용
			if (myScore > oppScore)
			{
				strQuery.Format(_T("UPDATE DC SET WINS = WINS + 1 WHERE ID = '%s'"), strUserID);
			}
			else if (myScore < oppScore)
			{
				strQuery.Format(_T("UPDATE DC SET LOSES = LOSES + 1 WHERE ID = '%s'"), strUserID);
			}
			else
			{
				db.Close();
				return;
			}
		}

		// 4. 실행
		try
		{
			db.ExecuteSQL(strQuery);
		}
		catch (CDBException *e)
		{
			e->Delete();
		}

		db.Close();
	}
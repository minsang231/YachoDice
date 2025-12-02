
// 1126View.h: CMy1126View 클래스의 인터페이스
//

#pragma once

struct DiceInfo {
	float x, y;       // 위치 (float로 변경 - 부드러운 움직임)
	float vx, vy;     // 속도 (velocity)
	float angle;      // 회전 각도
	float angleSpeed; // 회전 속도
	int value;        // 주사위 눈 (1~6)
	bool bRolling;    // 지금 구르는 중인가?
	bool bKeep;       // 주사위 고정 여부
};

class CMy1126View : public CFormView
{
protected: // serialization에서만 만들어집니다.
	CMy1126View() noexcept;
	DECLARE_DYNCREATE(CMy1126View)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_MY1126_FORM };
#endif

// 특성입니다.
public:
	CMy1126Doc* GetDocument() const;
	
// 작업입니다.
public:
	CImage m_image[7];
	DiceInfo m_dices[5];  // 주사위 5개 배열

	BOOL m_bIsAnimating;  // 애니메이션 진행 중인지 체크
	int m_nAniFrame;      // 애니메이션 남은 시간
	void DrawGame();
	int m_nLeftTime;
	// [추가] 현재 굴린 횟수 (0 ~ 3)
	int m_nRollCount;

	// [추가] 리스트 행 높이 조절용 이미지 리스트
	CImageList m_ListHeightAdjust;

	// [추가] 다음 턴으로 초기화하는 함수
	void NextTurn();

	// [추가] 해당 족보(행 번호)의 점수를 계산해주는 함수
	int GetScore(int nRow);

	// [추가] 계산된 점수를 리스트 컨트롤에 '미리보기'로 띄워주는 함수
	void UpdateScorePreview();

	// ★★★ [추가] 점수 미리보기용 변수 ★★★
	int m_nPreviewScores[15];      // 각 족보의 미리보기 점수 저장 (-1이면 표시 안 함)
	BOOL m_bCanScore[15];          // 해당 족보에 점수를 넣을 수 있는지 (0점 초과 여부)

	// [추가] 요트(50점)를 이미 확정 지었는지 기억하는 변수
	BOOL m_bYachtFixed[2];            // -> 배열로 변경

	// [추가] 각 줄(0~14)의 점수가 확정되었는지 체크하는 배열
	BOOL m_bScoreFixed[2][15];        // -> 2차원 배열로 변경

	// [추가] 게임 종료 확인 및 점수 집계 함수
	BOOL CheckGameOver();

	// [추가] 현재 플레이어 (0: 나, 1: 상대)
	int m_nCurrentPlayer;

	// [추가] 게임 모드 저장 (1: 1인용, 2: 2인용)
	int m_nGameMode;

	// ★★★ [추가] 배경 지우기 방지 함수 선언
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	CFont m_fontButton;

	BOOL m_bRolled; // [추가] 주사위를 한 번이라도 굴렸는지 체크
	CImage m_imgBg; // [추가] 전체 배경 이미지

	// ★★★ [추가] 남은 시간 저장 변수 (초 단위)
// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CMy1126View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl mlist;
	afx_msg void OnNMCustomdrawList2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void OnDraw(CDC* /*pDC*/);
	afx_msg void OnBnClickedButton3();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnClickList2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButton4();
	void SaveResultToDB(int myScore, int oppScore);
};

#ifndef _DEBUG  // 1126View.cpp의 디버그 버전
inline CMy1126Doc* CMy1126View::GetDocument() const
   { return reinterpret_cast<CMy1126Doc*>(m_pDocument); }
#endif


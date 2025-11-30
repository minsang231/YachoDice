
// 1126.cpp: 애플리케이션에 대한 클래스 동작을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "1126.h"
#include "MainFrm.h"

#include "1126Doc.h"
#include "1126View.h"
#include "CStartDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy1126App

BEGIN_MESSAGE_MAP(CMy1126App, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CMy1126App::OnAppAbout)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// 표준 인쇄 설정 명령입니다.
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CMy1126App 생성

CMy1126App::CMy1126App() noexcept
{

	// 다시 시작 관리자 지원
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 애플리케이션을 공용 언어 런타임 지원을 사용하여 빌드한 경우(/clr):
	//     1) 이 추가 설정은 다시 시작 관리자 지원이 제대로 작동하는 데 필요합니다.
	//     2) 프로젝트에서 빌드하려면 System.Windows.Forms에 대한 참조를 추가해야 합니다.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 아래 애플리케이션 ID 문자열을 고유 ID 문자열로 바꾸십시오(권장).
	// 문자열에 대한 서식: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("My1126.AppID.NoVersion"));

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

// 유일한 CMy1126App 개체입니다.

CMy1126App theApp;


// CMy1126App 초기화

BOOL CMy1126App::InitInstance()
{
	// 애플리케이션 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다. 
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// RichEdit 컨트롤을 사용하려면 AfxInitRichEdit2()가 있어야 합니다.
	// AfxInitRichEdit2();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 애플리케이션 마법사에서 생성된 애플리케이션"));
	LoadStdProfileSettings(4);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.

	// =========================================================
	// ★★★ [추가] 여기서 시작 메뉴 띄우기! ★★★
	// =========================================================
	CStartDlg dlg;

	// 다이얼로그를 띄우고, OK(버튼 클릭)가 아니면(취소/X누름) 프로그램 종료
	if (dlg.DoModal() != IDOK)
	{
		return FALSE;
	}

	// 선택한 모드 저장 (App 클래스 멤버변수에 저장)
	m_nGameMode = dlg.m_nSelectedMode;
	// =========================================================

	// 애플리케이션의 문서 템플릿을 등록합니다.  문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMy1126Doc),
		RUNTIME_CLASS(CMainFrame),       // 주 SDI 프레임 창입니다.
		RUNTIME_CLASS(CMy1126View));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// 명령줄에 지정된 명령을 디스패치합니다.
	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}

int CMy1126App::ExitInstance()
{
	//TODO: 추가한 추가 리소스를 처리합니다.
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

// CMy1126App 메시지 처리기


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;
	CImage m_image;   // 이미지 객체
	int m_posX;       // 이미지 X 좌표
	int m_posY;       // 이미지 Y 좌표

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();

public:
//	afx_msg void OnNMCustomdrawList2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	// ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST2, &CAboutDlg::OnNMCustomdrawList2)
	ON_WM_TIMER()
	ON_WM_PAINT()
END_MESSAGE_MAP()

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CMy1126App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CMy1126App 메시지 처리기
BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog(); // 부모 클래스 초기화 먼저 호출

	// 1. 이미지 로드 (경로를 본인 파일 경로로 꼭 바꾸세요!)
	// 주의: 경로에 역슬래시(\)는 두 개(\\) 써야 합니다.
	HRESULT hResult = m_image.Load(_T("C:\\주사위.jpg"));

	if (FAILED(hResult))
	{
		AfxMessageBox(_T("이미지를 불러오지 못했습니다. 경로를 확인하세요."));
	}

	// 2. 초기 좌표 설정
	m_posX = 110;
	m_posY = 500;

	// 3. 타이머 시작 (ID: 1, 30ms 간격)
	SetTimer(1, 30, NULL);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환
}



//void CAboutDlg::OnNMCustomdrawList2(NMHDR* pNMHDR, LRESULT* pResult)
//{
//	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
//	*pResult = CDRF_DODEFAULT;
//
//	switch (pLVCD->nmcd.dwDrawStage)
//	{
//	case CDDS_PREPAINT:
		// 아이템별로 그리기 단계가 오면 다시 알려달라고 요청
//		*pResult = CDRF_NOTIFYITEMDRAW;
//		break;
//
//	case CDDS_ITEMPREPAINT:
//	{
		// 현재 그리는 행(Row)의 번호
//		int nRow = (int)pLVCD->nmcd.dwItemSpec;
//
		// 이미지를 보면 '합계(6번)'와 '보너스(7번)'가 진한 색입니다.
		// 인덱스는 0부터 시작하므로 위 배열 순서에 맞춰 6, 7번을 체크합니다.
//		if (nRow == 6 || nRow == 7)
//		{
//			pLVCD->clrTextBk = RGB(100, 110, 120); // 배경색: 진한 회색 (이미지와 유사)
//			pLVCD->clrText = RGB(255, 255, 255); // 글자색: 흰색
//		}
//		else
//		{
			// 나머지 일반 행 (약간 베이지/연회색 톤)
//			pLVCD->clrTextBk = RGB(240, 235, 230);
//			pLVCD->clrText = RGB(0, 0, 0);
//		}
//
//		*pResult = CDRF_NEWFONT; // 폰트/색상 변경 사항 적용
//	}
//	break;
//	}
//}

void CAboutDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == 1) // 타이머 ID 확인
	{
		// 좌표를 오른쪽으로 5픽셀씩 이동
		m_posX += 5;

		// 화면 밖으로 나가면 다시 처음으로 (선택 사항)
		if (m_posX > 500) m_posX = 0;

		// **중요**: 화면 갱신 요청 -> OnPaint()가 호출됨
		// FALSE 파라미터는 배경을 지우지 않게 하여 깜빡임을 줄이는 팁입니다.
		Invalidate(FALSE);
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CAboutDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if (!m_image.IsNull())
	{
		m_image.Draw(dc, m_posX, m_posY);
	}
}

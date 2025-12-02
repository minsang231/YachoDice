
// 1126.h: 1126 애플리케이션의 기본 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
#error "PCH에 대해 이 파일을 포함하기 전에 'pch.h'를 포함합니다."
#endif

#include "resource.h" // 주 기호입니다.
#include <afxdb.h>

// CMy1126App:
// 이 클래스의 구현에 대해서는 1126.cpp을(를) 참조하세요.
//

class CMy1126App : public CWinApp
{
public:
	CMy1126App() noexcept;
	int m_nGameMode; // [추가] 게임 모드 저장 (1 or 2)

	// 재정의입니다.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	CString m_strCurrentUserID;
	CDatabase m_db;
	// 구현입니다.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMy1126App theApp;

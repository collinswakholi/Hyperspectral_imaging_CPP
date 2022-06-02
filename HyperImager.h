
// HyperImager.h: PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// 주 기호입니다.


// CHyperImagerApp:
// 이 클래스의 구현에 대해서는 HyperImager.cpp을(를) 참조하세요.
//

class CHyperImagerApp : public CWinApp
{
public:
	CHyperImagerApp();

// 재정의입니다.
public:
	virtual BOOL InitInstance();

// 구현입니다.

	DECLARE_MESSAGE_MAP()
};

extern CHyperImagerApp theApp;

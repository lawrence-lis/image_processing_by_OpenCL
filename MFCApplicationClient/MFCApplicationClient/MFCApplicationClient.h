
// MFCApplicationClient.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CMFCApplicationClientApp:
// Сведения о реализации этого класса: MFCApplicationClient.cpp
//

class CMFCApplicationClientApp : public CWinApp
{
public:
	CMFCApplicationClientApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	virtual ~CMFCApplicationClientApp();
	DECLARE_MESSAGE_MAP()
};

extern CMFCApplicationClientApp theApp;

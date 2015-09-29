#include "my_api.h"
#include <Uxtheme.h>
#include "app_config.h"
#include <regex>

HINSTANCE gInstance;
HINSTANCE gDllInstance;
HINSTANCE gResInstance;
bool bLockFont;
HFONT hLockedFont;
void MyAPI::initMyAPI(HINSTANCE hInstance)
{
	gInstance = GetModuleHandle(nullptr);
	gResInstance = LoadLibraryEx(L"lm_res.dll", nullptr, LOAD_LIBRARY_AS_DATAFILE);
	gDllInstance = hInstance;

	auto fontData = AppConfig::ReadConfig(L"Lock Draw Font", L"False");
	bLockFont = fontData != L"False";
	if (bLockFont)
	{
		// fontData
		std::wregex rFont(L"(.+),\\s*(\\d+)");
		std::wsmatch wideMatch;
		
		if (std::regex_match(fontData, wideMatch, rFont))
		{
			wstring wFontName = wideMatch._At(1).str();
			const wchar_t* lpFontName = wFontName.c_str();
			DWORD nFontHeight = _wtoi(wideMatch._At(2).str().c_str());

			dwprintf(L"Lock draw font: %s, %d\n", lpFontName, nFontHeight);

			hLockedFont = CreateFontW(nFontHeight, NULL, NULL, NULL, NULL, NULL, NULL,
				NULL, NULL, DEFAULT_CHARSET, NULL, NULL, NULL, lpFontName);
		} else
		{
			dprintf("Failed to extract font info.");
		}
	} else
	{
		dprintf("Font not locked.\n");
	}
}

INT_PTR WINAPI MyAPI::MyDialogBoxParamA(
	HINSTANCE hInstance,
	DWORD     lpTemplateName,
	HWND      hWndParent,
	DLGPROC   lpDialogFunc,
	LPARAM    dwInitParam
) {
	// dprintf("MyAPI::MyDialogBoxParamA\n");
	if ((hInstance == gInstance || !hInstance) && HIWORD(lpTemplateName) == 0)
	{
		dprintf("Loading dialog from lm_res.dll: %d.. ", lpTemplateName);
		INT_PTR hDialog = DialogBoxParamW(gResInstance, LPWSTR(lpTemplateName), hWndParent, lpDialogFunc, dwInitParam);
		if (hDialog != NULL && hDialog != -1) {
			dprintf("OK!\n");
			return hDialog;
		}

		dprintf("Failed, rollback.\n");
	}
	return DialogBoxParamA(hInstance, LPSTR(lpTemplateName), hWndParent, lpDialogFunc, dwInitParam);
}


BOOL WINAPI MyAPI::MyAppendMenuA(HMENU hMenu, UINT uFlags, UINT_PTR uIDNewItem, LPSTR lpNewItem)
{
	if (lpNewItem != nullptr)
	{
		auto menuItemA = string(lpNewItem);
		auto menuItemW = STR::ansi_to_unicode(menuItemA);
		auto newMenuItemW = Localization::get(menuItemW);
		return AppendMenuW(hMenu, uFlags, uIDNewItem, newMenuItemW.c_str());
	}

	return AppendMenuA(hMenu, uFlags, uIDNewItem, lpNewItem);
}

HWND WINAPI MyAPI::MyCreateWindowExA(DWORD dwExStyle, LPSTR lpClassName, LPSTR lpWindowName,
	DWORD dwStyle, int x, int y, int nWidth, int nHeight,
	HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	// return CreateWindowExA(dwExStyle, lpClassName, lpWindowName,
	//	dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);;

	bool bResetTheme = false;

	wchar_t* _lpClassNameW = STR::A2W(lpClassName, AppConfig::nCodePage);
	wchar_t* _lpWindowNameW = STR::A2W(lpWindowName, AppConfig::nCodePage);

	const wchar_t* lpWindowProxy = nullptr;
	wstring* _lpUnicode = nullptr;
	wstring _Localized(L"");

	if (_lpWindowNameW != nullptr)
	{
		_lpUnicode  = new wstring(_lpWindowNameW);
		_Localized = Localization::get(*_lpUnicode);
		// delete(_lpWindowNameW);

		lpWindowProxy = _Localized.c_str();
		// dprintf("Create Window %s\n", lpWindowName);
		// dwprintf(L"-> %s\n", _lpWindowNameW);
	} else
	{
		lpWindowProxy = _lpWindowNameW;
	}

	if (lpClassName != nullptr && _stricmp("ToolbarWindow32", lpClassName) == 0)
	{
		bResetTheme = true;
	}

	auto hWindow = CreateWindowExW(dwExStyle, _lpClassNameW, lpWindowProxy,
		dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

	SetWindowTextW(hWindow ,lpWindowProxy);
	// SendMessageW(hWindow, WM_SETTEXT, NULL, (LPARAM)lpWindowProxy);

	if (bResetTheme)
	{
		printf("Disable theme for control: 0x%08X\n", DWORD(hWindow));
		SetWindowTheme(hWindow, L"", L"");
	}

	if (_lpClassNameW  != nullptr) delete(_lpClassNameW );

	// if _lpUnicode is not null, 
	// then _lpWindowNameW shouldn't touch.
	if (_lpUnicode != nullptr)
	{
		delete _lpUnicode;
	} else if (_lpWindowNameW != nullptr)
	{
		delete(_lpWindowNameW);
	}

	return hWindow;
}

ATOM WINAPI MyAPI::MyRegisterClassExA(const WNDCLASSEXA* lpWndClass)
{
	WNDCLASSEXW lpWndClassW;
	memcpy(&lpWndClassW, lpWndClass, lpWndClass->cbSize);

	auto lpMenu = DWORD(lpWndClass->lpszMenuName);
	if (lpMenu != NULL && HIWORD(lpMenu) != 0)
	{
		lpWndClassW.lpszMenuName = STR::A2W(LPSTR(lpMenu), AppConfig::nCodePage);
	}

	auto lpClass = DWORD(lpWndClass->lpszClassName);
	if (lpClass != NULL && HIWORD(lpClass) != 0)
	{
		lpWndClassW.lpszClassName = STR::A2W(LPSTR(lpClass), AppConfig::nCodePage);
	}

	return RegisterClassExW(&lpWndClassW);
}

LRESULT WINAPI MyAPI::MySendMessageA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	string oldText;
	wstring oldTextW;
	wstring newTextW;

	// wchar_t* lpTextW = nullptr;
	switch(Msg)
	{
	case WM_SETTEXT:
		oldTextW = STR::A2W(LPSTR(lParam), CP_ACP);
		dwprintf(L"Set title: %s\n", oldTextW.c_str());
		return SendMessageW(hWnd, Msg, NULL, LPARAM(oldTextW.c_str()));

	case CB_ADDSTRING:
		if (!HIWORD(lParam)) break;

		oldText = reinterpret_cast<LPSTR>(lParam);
		oldTextW = STR::ansi_to_unicode(oldText);
		newTextW = Localization::get(oldTextW);

		return SendMessageW(hWnd, CB_ADDSTRING, NULL, LPARAM(newTextW.c_str()));

	case SB_SETTEXTA:
		if (!HIWORD(lParam)) break;

		auto highByte = HIBYTE(LOWORD(wParam));
		if (highByte & SBT_OWNERDRAW)
			break;

		oldText = reinterpret_cast<LPSTR>(lParam);
		oldTextW = STR::ansi_to_unicode(oldText);
		newTextW = Localization::get(oldTextW);

		return SendMessageW(hWnd, SB_SETTEXTW, wParam, LPARAM(newTextW.c_str()));
	}

	return SendMessageA(hWnd, Msg, wParam, lParam);
}

BOOL WINAPI MyAPI::MySetWindowTextA(HWND hWnd, LPSTR lpString)
{
	// dprintf("MySetWindowTextA\n");
	// return SendMessageA(hWnd, WM_SETTEXT, NULL, LPARAM(lpString));
	LPWSTR lpUnicode = STR::A2W(lpString, AppConfig::nCodePage);
	BOOL r = SetWindowTextW(hWnd, lpUnicode);
	delete lpUnicode;
	return r;
}

bool ctype_isdigit(char* lpString)
{
	char x;
	while (x = *lpString++)
		if (x < '0' || x > '9')
			return false;

	return true;
}

BOOL WINAPI MyAPI::MyExtTextOutA(HDC hdc, int X, int Y, UINT fuOptions,
	const RECT* lprc, LPSTR lpString, UINT cbCount, const INT* lpDx)
{
	if (cbCount < 1) return FALSE;
	if (!lpString)   return FALSE;

	if (ctype_isdigit(lpString))
	{
		return ExtTextOutA(hdc, X, Y, fuOptions, lprc, lpString, cbCount, lpDx);
	}

	auto lpUnicode = STR::A2W(lpString, AppConfig::nCodePage, cbCount);

	// dprintf("Draw text: %s\n", lpString);

	auto hOldFont = SelectObject(hdc, hLockedFont);
	auto r = ExtTextOutW(hdc, X, Y, fuOptions, lprc,
		lpUnicode, wcslen(lpUnicode), lpDx);
	SelectObject(hdc, hOldFont);

	delete lpUnicode;

	return r;
}
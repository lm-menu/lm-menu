#pragma once
#include "stdafx.h"
#include "string_util.h"
#include "menu_hook.h"
#include "localization.h"

namespace MyAPI
{
	void initMyAPI(HINSTANCE hInstance);

	INT_PTR WINAPI MyDialogBoxParamA(
		HINSTANCE hInstance,
		DWORD     lpTemplateName,
		HWND      hWndParent,
		DLGPROC   lpDialogFunc,
		LPARAM    dwInitParam
	);

	BOOL WINAPI MyAppendMenuA(HMENU hMenu, UINT uFlags, UINT_PTR uIDNewItem, LPSTR lpNewItem);

	HWND WINAPI MyCreateWindowExA(
		_In_     DWORD     dwExStyle,
		_In_opt_ LPSTR     lpClassName,
		_In_opt_ LPSTR     lpWindowName,
		_In_     DWORD     dwStyle,
		_In_     int       x,
		_In_     int       y,
		_In_     int       nWidth,
		_In_     int       nHeight,
		_In_opt_ HWND      hWndParent,
		_In_opt_ HMENU     hMenu,
		_In_opt_ HINSTANCE hInstance,
		_In_opt_ LPVOID    lpParam
	);

	ATOM WINAPI MyRegisterClassExA(
		_In_ const WNDCLASSEXA *lpWndClass
	);

	LRESULT WINAPI MySendMessageA(
		_In_ HWND   hWnd,
		_In_ UINT   Msg,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam
		);

	BOOL WINAPI MySetWindowTextA(
		_In_     HWND    hWnd,
		_In_opt_ LPSTR   lpString
		);

	BOOL WINAPI MyExtTextOutA(
		_In_       HDC     hdc,
		_In_       int     X,
		_In_       int     Y,
		_In_       UINT    fuOptions,
		_In_ const RECT    *lprc,
		_In_       LPSTR   lpString,
		_In_       UINT    cbCount,
		_In_ const INT     *lpDx
		);
}
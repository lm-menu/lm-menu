#include "stdafx.h"
#include "dllmain.h"
#include "menu_hook.h"
#include "my_api.h"
#include <VersionHelpers.h>
#include "app_config.h"

HANDLE mainThread;
void initLibrary (HINSTANCE hinstDLL)
{
	AppConfig::init();
	doHookMenu();
	MyAPI::initMyAPI(hinstDLL);
	// ResumeThread(mainThread);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		if (IsWindows7OrGreater())
		{
			initLibrary(hinstDLL);
		} else
		{
			try
			{
				initLibrary(hinstDLL);
			}
			catch (int err)
			{
				dprintf("We had an error, but got cought!\n");

				// To remove variable not used warning.
				SetLastError(err);
			}
			// Sleep(DWORD(initLibrary));
			// mainThread = GetCurrentThread();
			// CreateThread(nullptr, NULL, LPTHREAD_START_ROUTINE(initLibrary), hinstDLL, NULL, nullptr);
			// SuspendThread(mainThread);
		}
		break;
	}

	return TRUE;
}


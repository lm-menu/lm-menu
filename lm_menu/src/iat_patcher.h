#pragma once

#include "stdafx.h"

namespace IAT
{
	bool PatchFunction(char* lpLibrary, char* lpFunctionName, LPVOID myFunction);
}
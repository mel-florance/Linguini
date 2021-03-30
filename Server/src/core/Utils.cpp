#include "Utils.h"

#include <string>
#include <iostream>
#include <cstdlib>

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#endif

std::string Utils::getMimeType(const std::string& ext)
{
	std::string result = "application/unknown";

#ifdef PLATFORM_WINDOWS
	HKEY key = NULL;

	std::wstring extension = std::wstring(ext.begin(), ext.end());

	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, (LPCWSTR)extension.c_str(), 0, KEY_READ, &key) == ERROR_SUCCESS)
	{
		char buffer[1024];
		DWORD size = sizeof(buffer);

		if (RegQueryValueExA(key, "Content Type", NULL, NULL, (LPBYTE)buffer, &size) == ERROR_SUCCESS)
			result = buffer;

		RegCloseKey(key);
	}
#endif

#ifdef PLATFORM_LINUX
	result = Utils::exec("mimetype -b " + ext);
#endif

	return result;
}

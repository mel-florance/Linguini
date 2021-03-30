#include "Utils.h"
#include <Windows.h>
#include <string>
#include <iostream>

std::string Utils::getMimeType(const std::string& ext)
{
	HKEY key = NULL;
	std::string result = "application/unknown";
	std::wstring extension = std::wstring(ext.begin(), ext.end());

	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, (LPCWSTR)extension.c_str(), 0, KEY_READ, &key) == ERROR_SUCCESS)
	{
		char buffer[1024];
		DWORD size = sizeof(buffer);

		if (RegQueryValueExA(key, "Content Type", NULL, NULL, (LPBYTE)buffer, &size) == ERROR_SUCCESS)
			result = buffer;

		RegCloseKey(key);
	}

	return result;
}

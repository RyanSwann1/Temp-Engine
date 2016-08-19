#pragma once
#define RUNNING_WINDOWS
#include <iostream>
#include <string>
#include <algorithm>

namespace Utils {
#ifdef RUNNING_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Shlwapi.h>
	inline std::string GetWorkingDirectory() {
		HMODULE hModule = GetModuleHandle(nullptr);
		if (hModule) {
			char path[256];
			GetModuleFileName(hModule, path, sizeof(path));
			PathRemoveFileSpec(path);
			strcat_s(path, "\\");
			return std::string(path);
		}
		return "";
	}
#endif
}
///Javawag: missing symbols sooo that means youre missing a library sometwhere
//BonBons: ohhh
	//Javawag : shlwapi.lib says google

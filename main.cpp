#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <string>
#include <iostream>

#pragma comment(lib, "user32")
#pragma comment(lib, "gdi32")
#pragma comment(lib, "dwmapi")

enum HOTKEY_ID {
    HK_PIN = 1,
    HK_UNPIN,
    HK_TOGGLE_OPACITY,
    HK_TOGGLE_CLICKTHROUGH
};

static HWND g_pinnedWnd = NULL;
static LONG_PTR g_origExStyle = 0;
static LONG_PTR g_origStyle = 0;
static WINDOWPLACEMENT g_origPlacement = { sizeof(WINDOWPLACEMENT) };
static BYTE g_opacity = 255;
static bool g_isSemiTransparent = false;
static bool g_clickThrough = false;

void logw(const std::wstring &s) {
    std::wcout << s << std::endl;
}

bool IsWindowValidForPin(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return false;
    if (!IsWindowVisible(hwnd)) return false;

    WINDOWPLACEMENT wp;
    wp.length = sizeof(WINDOWPLACEMENT);
    if (GetWindowPlacement(hwnd, &wp) && wp.showCmd == SW_SHOWMINIMIZED) return false;

    wchar_t title[256] = {};
    GetWindowTextW(hwnd, title, sizeof(title)/sizeof(title[0]));

    return true;
}

bool PinWindow(HWND hwnd) {
    if (!IsWindowValidForPin(hwnd)) return false;

    g_origExStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    g_origStyle = GetWindowLongPtr(hwnd, GWL_STYLE);
    GetWindowPlacement(hwnd, &g_origPlacement);

    g_pinnedWnd = hwnd;
    g_isSemiTransparent = false;
    g_clickThrough = false;
    g_opacity = 255;

    ShowWindow(hwnd, SW_SHOWNORMAL);
    SetWindowPos(hwnd, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);

    SetWindowLongPtr(hwnd, GWL_EXSTYLE, g_origExStyle | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, 0, g_opacity, LWA_ALPHA);

    wchar_t buf[128];
    swprintf(buf, L"Pinned HWND: 0x%p", hwnd);
    logw(buf);

    return true;
}

void RestoreOriginalWindowState() {
    if (!g_pinnedWnd || !IsWindow(g_pinnedWnd)) return;

    SetWindowLongPtr(g_pinnedWnd, GWL_EXSTYLE, g_origExStyle);
    SetWindowLongPtr(g_pinnedWnd, GWL_STYLE, g_origStyle);
    SetWindowPlacement(g_pinnedWnd, &g_origPlacement);
    SetWindowPos(g_pinnedWnd, HWND_NOTOPMOST, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);

    wchar_t buf[128];
    swprintf(buf, L"Restored HWND: 0x%p", g_pinnedWnd);
    logw(buf);

    g_pinnedWnd = NULL;
}

void ToggleOpacity() {
    if (!g_pinnedWnd || !IsWindow(g_pinnedWnd)) return;

    BYTE opaque = 255;
    BYTE semi = 200;

    g_isSemiTransparent = !g_isSemiTransparent;
    g_opacity = g_isSemiTransparent ? semi : opaque;

    SetLayeredWindowAttributes(g_pinnedWnd, 0, g_opacity, LWA_ALPHA);

    wchar_t buf[128];
    swprintf(buf, L"Opacity now %d", g_opacity);
    logw(buf);
}

void ToggleClickThrough() {
    if (!g_pinnedWnd || !IsWindow(g_pinnedWnd)) return;

    g_clickThrough = !g_clickThrough;

    LONG_PTR ex = GetWindowLongPtr(g_pinnedWnd, GWL_EXSTYLE);

    if (g_clickThrough)
        SetWindowLongPtr(g_pinnedWnd, GWL_EXSTYLE, ex | WS_EX_TRANSPARENT);
    else
        SetWindowLongPtr(g_pinnedWnd, GWL_EXSTYLE, ex & ~WS_EX_TRANSPARENT);

    logw(g_clickThrough ? L"Click-through ON" : L"Click-through OFF");
}

bool RegisterHK(HWND hwnd, int id, UINT mods, UINT vk) {
    if (!RegisterHotKey(hwnd, id, mods, vk)) {
        wchar_t buf[128];
        swprintf(buf, L"Failed to register hotkey %d", id);
        logw(buf);
        return false;
    }
    return true;
}

void UnregisterAllHK(HWND hwnd) {
    for (int i = HK_PIN; i <= HK_TOGGLE_CLICKTHROUGH; i++)
        UnregisterHotKey(hwnd, i);
}

LRESULT CALLBACK HiddenProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_HOTKEY) {
        switch (wp) {
            case HK_PIN: PinWindow(GetForegroundWindow()); break;
            case HK_UNPIN: RestoreOriginalWindowState(); break;
            case HK_TOGGLE_OPACITY: ToggleOpacity(); break;
            case HK_TOGGLE_CLICKTHROUGH: ToggleClickThrough(); break;
        }
    }
    if (msg == WM_DESTROY) {
        UnregisterAllHK(hwnd);
        PostQuitMessage(0);
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

int wmain() {
    HINSTANCE hInst = GetModuleHandle(NULL);

    WNDCLASSW wc = {};
    wc.hInstance = hInst;
    wc.lpfnWndProc = HiddenProc;
    wc.lpszClassName = L"PinToolHiddenClass";

    RegisterClassW(&wc);

    HWND hidden = CreateWindowExW(
        0,
        L"PinToolHiddenClass",
        L"PinToolHidden",
        0,
        0, 0, 0, 0,
        HWND_MESSAGE,
        NULL,
        hInst,
        NULL
    );

    RegisterHK(hidden, HK_PIN, MOD_CONTROL|MOD_SHIFT, 'P');
    RegisterHK(hidden, HK_UNPIN, MOD_CONTROL|MOD_SHIFT, 'U');
    RegisterHK(hidden, HK_TOGGLE_OPACITY, MOD_CONTROL|MOD_SHIFT, 'O');
    RegisterHK(hidden, HK_TOGGLE_CLICKTHROUGH, MOD_CONTROL|MOD_SHIFT, 'C');

    std::wcout << L"PinTool (real window mode) running.\n";
    std::wcout << L"Hotkeys:\n";
    std::wcout << L" - Pin active window: Ctrl+Shift+P\n";
    std::wcout << L" - Unpin: Ctrl+Shift+U\n";
    std::wcout << L" - Toggle opacity: Ctrl+Shift+O\n";
    std::wcout << L" - Toggle click-through: Ctrl+Shift+C\n";
    std::wcout << L"Close this console to exit the program.\n";

    MSG msg;
    while (GetMessage(&msg, NULL, 0,0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (g_pinnedWnd && !IsWindow(g_pinnedWnd)) {
            logw(L"Pinned window closed externally. Clearing state.");
            g_pinnedWnd = NULL;
        }
    }

    return 0;
}

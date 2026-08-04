#include "WindowBase.h"

namespace Xtentix {

WindowBase::~WindowBase() {
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
}

HWND WindowBase::CreateWindowInternal(const std::wstring& className, const std::wstring& title,
                                       DWORD style, DWORD exStyle,
                                       int x, int y, int width, int height,
                                       HWND parent, HMENU menu, HINSTANCE instance) {
    WNDCLASSEXW wc{};
    if (!GetClassInfoExW(instance, className.c_str(), &wc)) {
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = &WindowBase::StaticWndProc;
        wc.hInstance = instance;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszClassName = className.c_str();
        RegisterClassExW(&wc);
    }

    m_hwnd = CreateWindowExW(exStyle, className.c_str(), title.c_str(), style,
                              x, y, width, height, parent, menu, instance, this);
    return m_hwnd;
}

LRESULT CALLBACK WindowBase::StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    WindowBase* self = nullptr;

    if (msg == WM_NCCREATE) {
        auto cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
        self = reinterpret_cast<WindowBase*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        self->m_hwnd = hwnd;
    } else {
        self = reinterpret_cast<WindowBase*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    if (self) {
        LRESULT result = 0;
        if (self->HandleMessage(msg, wParam, lParam, result)) {
            return result;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

} // namespace Xtentix

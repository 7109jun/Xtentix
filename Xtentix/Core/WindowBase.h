#pragma once
#include <windows.h>
#include <string>

namespace Xtentix {

// 모든 Win32 윈도우(메인 윈도우, 커스텀 컨트롤)가 상속하는 베이스 클래스.
// - 클래스 등록 / 인스턴스별 WndProc 디스패치를 캡슐화해서
//   파생 클래스는 HandleMessage()만 오버라이드하면 됨.
class WindowBase {
public:
    WindowBase() = default;
    virtual ~WindowBase();

    WindowBase(const WindowBase&) = delete;
    WindowBase& operator=(const WindowBase&) = delete;

    HWND Handle() const { return m_hwnd; }

protected:
    // 파생 클래스가 실제 메시지 처리를 구현. 처리한 메시지는 outResult에 값을 쓰고
    // true를 반환. 처리하지 않았으면 false를 반환해 DefWindowProc으로 넘어가게 한다.
    virtual bool HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT& outResult) = 0;

    HWND CreateWindowInternal(const std::wstring& className, const std::wstring& title,
                               DWORD style, DWORD exStyle,
                               int x, int y, int width, int height,
                               HWND parent, HMENU menu, HINSTANCE instance);

    HWND m_hwnd = nullptr;

private:
    static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

} // namespace Xtentix

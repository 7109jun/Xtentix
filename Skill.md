# Skill — Xtentix 완성형 가이드

이 문서는 Xtentix 리포지토리를 코드 중심으로 읽고 정리한 실전 스킬 노트입니다. 목표는 "개발자가 코드를 빠르게 이해하고, 확장(Extension)을 설계/구현하며, 문서·빌드·테스트를 바로 정비"할 수 있게 돕는 것입니다.

대상: 리포지토리에 새로 합류한 개발자, 기여자, 혹은 확장 기능을 만들고 싶은 개발자

요약
- 언어: C++ (C++17)
- 빌드: CMake (권장: CMake >= 3.20 + MSVC on Windows)
- 플랫폼: 현재 Windows 중심 (user32, gdi32, gdiplus, winhttp 등 사용)
- 라이선스: SSPL (상업적 사용 제한 — 주의 필요)

목차
1. 빠른 시작(빌드 및 실행)
2. 코드 구조(핵심 파일 매핑)
3. 코드 읽기 가이드(핵심 진입점과 흐름)
4. 확장(Extension) 설계: 목표·API·구현 예시
5. 테스트 및 CI 추천
6. 문서/기여 가이드라인
7. 우선순위 체크리스트

---

## 1) 빠른 시작 — 빌드 & 실행
사전 준비
- Windows (권장: Windows 10/11)
- Visual Studio 2019/2022 (MSVC)
- CMake >= 3.20

Visual Studio (권장)에서 빌드
1. 프로젝트 루트에서 터미널/PowerShell 열기
2. mkdir build && cd build
3. cmake -G "Visual Studio 17 2022" ..
4. cmake --build . --config Release

간단한 PowerShell 스크립트 예시(리포지토리에 build.ps1로 추가 권장)
```powershell name=build.ps1
$buildDir = "build"
if (-Not (Test-Path $buildDir)) { New-Item -ItemType Directory $buildDir }
Push-Location $buildDir
cmake -G "Visual Studio 17 2022" ..
cmake --build . --config Release
Pop-Location
```

실행
- 빌드 후 출력 경로(예: build/Release/Xtentix.exe)를 실행.

문제 해결 팁
- CMake가 Visual Studio generator를 못 찾으면, VS Developer Command Prompt에서 시도.
- 누락된 라이브러리(예: gdiplus)는 Windows SDK 설치 확인.

---

## 2) 코드 구조 — 핵심 파일 맵핑
(파일 목록은 Xtentix/CMakeLists.txt 기반)
- Browser/main.cpp — 진입점(main)
- Core/
  - Application.cpp — 애플리케이션 초기화, 메시지 루프, 전역 리소스
  - WindowBase.* — 윈도우 래퍼/기본 창 처리
- UI/
  - MainWindow.cpp, TabStrip.cpp, AddressBar.cpp, NewTabPage.cpp — 윈도우 UI 컴포넌트
- Tabs/
  - Tab.cpp, TabManager.cpp — 탭과 탭 관리 로직
- Rendering/
  - HtmlTokenizer.cpp, HtmlParser.cpp, Dom.cpp, Document.cpp — HTML 파싱 및 DOM 구성
  - CssParser.cpp, CssValues.cpp, StyleResolver.cpp — CSS 파싱 및 스타일 계산
  - LayoutEngine.cpp — 레이아웃 계산
  - GdiPainter.cpp — GDI+ 기반 렌더링
- Network/
  - HttpClient.cpp — HTTP 요청/응답 처리
  - RssParser.cpp — RSS 파싱 유틸
- Bookmark/
  - BookmarkManager.h/cpp — 북마크 저장/로드(기본 경로: %APPDATA%/Xtentix/bookmarks.json)
- Utils/
  - Json.cpp/h — 자체 JSON 파서/구성
  - ResourceImage.cpp — 리소스 이미지 로딩
- Resources/app.rc — 리소스(.ico 등)
- CMakeLists.txt — 빌드 설정

루트에 큰 파일: 1Xtentix.tar, 2Xtentix.tar — 용도 확인 필요(릴리즈 아티팩트라면 Releases로 이동 권장)

---

## 3) 코드 읽기 가이드 — 흐름 파악 포인트
1. 진입점: Browser/main.cpp
   - 여기서 Application 인스턴스를 만들고 초기화하는 흐름을 따라가면 앱 전체 생명주기를 이해할 수 있습니다.
2. Application 초기화: Core/Application.cpp
   - 전역 리소스(폰트, GDI+ 초기화 등)와 창 생성, 메시지 루프 등록 위치.
3. 탭/페이지 로드 흐름: TabManager / Tab
   - Tab에 URL 요청 → Network/HttpClient로 요청 → Rendering 파이프라인(HtmlTokenizer -> HtmlParser -> DOM 생성 -> StyleResolver -> LayoutEngine -> GdiPainter)
4. 이벤트 훅 포인트
   - DOM 생성 직후(파싱 완료) — 확장(extension)이 DOM에 접근해 변형할 수 있는 위치
   - 네트워크 요청 전/후 — 요청을 가로채거나 헤더를 조작할 수 있는 위치
5. 데이터 및 설정
   - 북마크: BookmarkManager(DefaultFilePath(), Load/Save) — 파일 기반 자동 저장 전략
   - 리소스: Resources 폴더 및 app.rc

읽을 때 체크리스트
- 파일별 공개 API(메서드)와 내부 구현을 먼저 파악
- I/O(파일/네트워크) 실패 케이스 확인 — 앱이 어떻게 복구/무시하는지 확인
- 플랫폼 종속 코드(Win32 API 사용)를 따로 표시(나중에 추상화 가능)

---

## 4) 확장(Extension) 설계 — 완성형 안내
현재 리포지토리에는 확장 시스템이 구현되어 있지 않으므로, "완성형" 확장 시스템을 설계하고 구현하는 방법을 제안합니다. 목표는 확장(또는 플러그인)을 통해 페이지 조작, 네트워크 가로채기, UI 확장 등을 가능하게 하는 것입니다.

핵심 설계 목표
- 안정성: 확장 실패가 브라우저 전체를 다운시키지 않도록 격리
- 단순성: 기본 C++ 인터페이스로 구현 가능
- 안전한 로드: 확장은 %APPDATA%/Xtentix/extensions 에 설치, manifest.json 으로 메타데이터 관리

선택지: 네이티브 플러그인(DLL) vs 스크립트 기반(JS)
- 네이티브(DLL): 성능 좋음, C++ API 제공. Windows 전용 로드(LoadLibrary). 샌드박스가 어렵고 충돌 위험 존재.
- 스크립트(JS): Duktape/QuickJS 같은 임베디드 JS 엔진 사용. 안전 제어, 동적 확장 가능. C++ 쪽에서 필요한 훅만 노출.

여기서는 간단한 네이티브 플러그인 인터페이스와 manifest 예시, 그리고 JS 스크립트 방식의 통합 제안을 모두 포함합니다.

확장 디렉터리 및 매니페스트
- 설치 위치: %APPDATA%\Xtentix\extensions\
- 각 확장은 폴더로 존재: extensions/my-extension/
  - manifest.json
  - dll (native) 또는 script.js (JS)

manifest.json 예시
```json name=examples/extension/manifest.json
{
  "name": "logger",
  "version": "0.1.0",
  "description": "로그를 출력하는 확장",
  "type": "native", // "native" 또는 "script"
  "entry": "logger.dll", // 또는 script.js
  "permissions": ["webRequest","domModify"]
}
```

네이티브 확장 API(헤더 예시)
```c++ name=examples/extension/IExtension.h
#pragma once

#ifdef _WIN32
#define EXT_EXPORT extern "C" __declspec(dllexport)
#else
#define EXT_EXPORT
#endif

namespace Xtentix {

struct ExtensionHost {
    // 간단한 로그 API
    void (*Log)(const char* msg);
    // 페이지 URL 반환
    const char* (*GetCurrentUrl)();
    // DOM 접근 포인터(간단화) - 실제로는 적절한 DOM 포인터/인터페이스 필요
    void* dom;
};

// 확장의 생명주기: 로드될 때 호출
EXT_EXPORT bool Extension_Initialize(ExtensionHost* host);
// 확장 해제 전 호출
EXT_EXPORT void Extension_Shutdown();
// DOM이 준비되었을 때 호출
EXT_EXPORT void Extension_OnDomReady(void* dom);

} // namespace Xtentix
```

간단한 네이티브 확장 로더(핵심 포인트)
- Application 초기화 시 extensions 폴더 스캔
- manifest.json을 읽어 타입에 따라 로드
- native: LoadLibrary / GetProcAddress("Extension_Initialize") 호출
- 실패 시 로그만 남기고 무시

로드 예시 장소: Core/Application.cpp 의 애플리케이션 초기화 루틴 직후에 확장 매니저를 초기화하고, 페이지 파싱/DOM 생성 후 Extension_OnDomReady를 호출하면 확장이 DOM을 읽거나 조작할 수 있음.

간단한 JS 스크립트 확장 통합(권장 안전성)
- QuickJS 또는 Duktape를 embed
- 스크립트는 제한된 호스트 API만 호출 (log, getCurrentUrl, addCss, addScript)
- DOM 조작은 "DOM snapshot" 또는 host->invoke 방식을 통해 안전하게 노출

manifest.json (script 예)
```json name=examples/extension/manifest-script.json
{
  "name": "inject-css",
  "version": "0.1.0",
  "type": "script",
  "entry": "inject.js",
  "permissions": ["domModify"]
}
```

inject.js (예시)
```javascript name=examples/extension/inject.js
// 간단 예: 페이지 로드 시 CSS 추가
host.onDomReady(function() {
  host.addCss("body { background: #f0f0f0; }");
});
```

권장 구현 단계(우선순위)
1. ExtensionManager 클래스 추가 (Xtentix/Extensions/ExtensionManager.*)
   - extensions 폴더 스캔, manifest 파싱, 로드/언로드 관리
2. Application 초기화 시 ExtensionManager 초기화
3. Rendering 파이프라인에서 DOM 생성 직후 확장 훅 호출
4. 네이티브 확장의 경우 예외/크래시 보호(try/catch + SEH 고려)
5. 스크립트 확장 시에는 QuickJS/Duktape 런타임을 프로세스에 내장
6. 권한 모델(permissions)을 manifest로 제한하여 보안 확보

참고: 네이티브 확장은 강력하지만 안정성 위험이 있으므로 초기에 JS 기반 확장 시스템을 도입한 후, 필요하면 네이티브 확장 지원을 검토하세요.

---

## 5) 테스트 및 CI 제안
권장 우선순위
1. 유닛 테스트(우선순위 상): Utils/Json, BookmarkManager
   - 프레임워크: GoogleTest 또는 Catch2 (단일 헤더로 편리)
2. GitHub Actions 워크플로우
   - Windows 빌드(매 PR), artifact 업로드
   - (선택) 정적 분석 단계: clang-tidy 또는 cppcheck

GitHub Actions 간단 예 (Windows 빌드)
```yaml name=.github/workflows/windows-build.yml
name: Windows Build
on: [push, pull_request]
jobs:
  build:
    runs-on: windows-latest
    steps:
      - uses: actions/checkout@v4
      - name: Setup CMake
        uses: jwlawson/actions-setup-cmake@v1
        with:
          cmake-version: '3.24.0'
      - name: Configure
        run: cmake -S . -B build -G "Visual Studio 17 2022"
      - name: Build
        run: cmake --build build --config Release
      - name: Upload artifact
        uses: actions/upload-artifact@v4
        with:
          name: Xtentix-build
          path: build/**/Xtentix.exe
```

유닛 테스트 예(BookmarkManager)
- 테스트 케이스: 빈 파일/없는 파일 상황에서 Load()가 빈 리스트를 반환, Add/Remove/Contains 동작 확인, Save 후 파일 내용 검증

---

## 6) 문서·기여 가이드(권장 파일들)
- README.md: 프로젝트 개요, 빌드/실행, 스크린샷, 라이선스 요약
- Skill.md: (이 문서)
- CONTRIBUTING.md: PR 절차, 코드 스타일, 브랜치 정책
- ISSUE_TEMPLATE.md, PULL_REQUEST_TEMPLATE.md
- SECURITY.md: 취약점 보고 방법

PR 템플릿 간단 예
```markdown name=.github/PULL_REQUEST_TEMPLATE.md
## 요약
(변경 내용 간단 설명)

## 변경 사항
- 항목 1
- 항목 2

## 빌드/테스트 방법
(로컬에서 검증하는 방법)
```

---

## 7) 우선순위 체크리스트(즉시 적용 가능)
1. README에 빌드 스니펫 및 디렉터리 구조 보강 (우선)
2. build.ps1 / build.bat 추가
3. Skill.md(완성형) 커밋 — 완료
4. ExtensionManager 스켈레톤 추가(폴더: Xtentix/Extensions)
5. 간단한 JS 기반 확장 런타임(QuickJS) 통합(스켈레톤)
6. GoogleTest 기반 Json/Bookmark 테스트 추가
7. GitHub Actions: Windows 빌드 워크플로우 추가
8. 대용량 .tar 파일 처리 결정(릴리즈 아티팩트로 이동 권장)

---

부록: 빠른 코드 스니펫(확장용 인터페이스 & manifest 예시)
- manifest.json (예시)
```json name=examples/extension/manifest.example.json
{
  "name": "example-extension",
  "version": "0.1.0",
  "type": "script",
  "entry": "inject.js",
  "permissions": ["domModify","webRequest"]
}
```

- 네이티브 확장 인터페이스 헤더 예시
```c++ name=examples/extension/ExtensionAPI.h
#pragma once

struct XtentixHost {
    void (*Log)(const char* msg);
    const char* (*GetUrl)();
    void (*AddCss)(const char* css);
};

extern "C" __declspec(dllexport) bool Extension_Initialize(XtentixHost* host);
extern "C" __declspec(dllexport) void Extension_Shutdown();
extern "C" __declspec(dllexport) void Extension_OnDomReady(void* dom);
```

---

끝. 추가로 제가 바로 구현해드릴 작업들
- README 보강(빌드 스니펫 + 디렉터리 구조) — 원하시면 지금 커밋합니다.
- build.ps1 / build.bat 추가 — 바로 커밋 가능합니다.
- Extensions/ExtensionManager 스켈레톤(C++ 파일 2~3개) 추가 — 바로 커밋 가능합니다.
- QuickJS 연동 스켈레톤 추가 — QuickJS 소스 포함은 크므로 서브모듈 또는 외부 참조 권장.

어떤 것을 먼저 진행할까요? (예: "1과 2를 추가해줘" 또는 "ExtensionManager 골격을 먼저 만들어줘")
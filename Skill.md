# Skill — Xtentix 빠른 가이드

이 문서는 Xtentix 리포지토리를 읽고 정리한 "무엇을 할 수 있는가 / 무엇을 하지 않는가 / 참고할 점 / 배워보기" 형식의 실전 스킬 노트입니다. 개발자(혹은 기여자)가 빠르게 상황을 파악하고 다음 작업을 정할 수 있게 돕습니다.

## 1) 대충 할 수 있는 거 (빠르게 적용 가능한 작업)
- 로컬 빌드 시도
  - CMake 기반 프로젝트. Windows용 라이브러리(user32, gdi32, gdiplus, winhttp 등)를 링크하므로 Windows 환경(MSVC)에서 바로 빌드 시도 가능.
  - 권장 명령(Visual Studio 사용 예):
    - mkdir build && cd build
    - cmake -G "Visual Studio 17 2022" ..
    - cmake --build . --config Release
- 북마크 기능 확인 및 간단한 수정
  - Xtentix/Bookmark/BookmarkManager.*는 JSON 직렬화 방식으로 `%APPDATA%/Xtentix/bookmarks.json`에 자동 저장/로드.
  - 파일 경로/저장 방식, 실패 시 동작(빈 목록으로 시작)을 그대로 활용해 테스트 케이스(파일 존재/손상/쓰기 실패)를 추가 가능.
- README 보강 및 문서화
  - 기존 README가 최소 정보만 포함. Skill.md를 만들어 빌드/디렉토리/데이터 위치/기여방법을 추가하면 즉시 유용성 향상.
- 리소스 확인
  - 루트에 1Xtentix.tar, 2Xtentix.tar가 있음. 릴리즈 파일인지 확인하고 Release 태그 또는 Releases 섹션으로 이동시키기.

## 2) 못하는 거 (현재 리포지토리 상태로는 바로 실행/검증하기 어렵거나 없는 항목)
- 크로스플랫폼 빌드 보장
  - CMake는 사용하지만 코드가 Windows API에 의존적이라 POSIX/리눅스/맥에서 바로 빌드되지 않음.
- 자동화된 테스트/CI
  - 테스트 코드 및 CI 설정(예: GitHub Actions)이 없음. 자동화된 회귀 검증 불가.
- 문서화된 기여 프로세스
  - CONTRIBUTING.md, PR/ISSUE 템플릿이 없어 외부 기여자가 따르기 어려움.
- 보안·시크릿 검사 자동화
  - 현재 시크릿 노출 검사(GitHub action) 등이 없음.

## 3) 참고할 점 (코드 읽을 때 유의하거나 개선 포인트)
- 빌드 설정
  - Xtentix/CMakeLists.txt에 소스 파일 목록이 하드코딩되어 있음. 파일 추가/삭제 시 CMake 수정 필요 — glob 사용은 권장하지 않지만, 모듈 단위로 CMake 파일 분리하는 패턴(하위 CMakeLists.txt) 고려.
- 플랫폼 종속성
  - target_link_libraries에 Windows 전용 라이브러리가 명시되어 있음. 멀티플랫폼을 목표로 한다면 추상화 계층(Win32 전용 코드 분리) 필요.
- 데이터 안전성 설계
  - BookmarkManager의 설계는 관점상 안전(파일 손상 시 무시, 자동 저장)이며 UX친화적임. 다만 "조용히 실패" 전략 때문에 디스크 쓰기 실패를 사용자에게 알리지 못함 — 로깅/옵션화 고려.
- Utils/Json
  - 외부 라이브러리 없이 자체 Json 구현을 사용하고 있음. 견고성(에러 케이스, 큰 파일, 비정상 입력)에 대한 단위 테스트 필요.
- 리포지토리 용량 관리
  - 루트의 .tar 파일(특히 1MB+인 2Xtentix.tar)은 Git 히스토리를 무겁게 만들 수 있음. 대형 바이너리는 Git LFS 또는 외부 릴리즈로 이동 권장.
- 라이선스(SSPL)
  - SSPL은 상업적 사용에 제약이 있으므로 README에 별도 경고와 라이선스 요약을 추가할 것.

## 4) 배워보기 (짧게 적용 가능한 학습/개선 항목)
- 최소한의 CI 설정 (권장 순서)
  1. GitHub Actions 워크플로우로 Windows 빌드 테스트 추가(매 PR마다): simple matrix로 Visual Studio 버전 테스트.
  2. 빌드 성공 시 artifact 업로드(실행 파일).  
  3. (선택) 정적분석 도구 추가(clang-tidy, cppcheck) 또는 MSVC 경고 수준 강화.
- 단위 테스트 추가
  - GoogleTest 또는 Catch2를 도입해 Utils/Json부터 테스트 작성.
  - 예시: Json 파싱(정상/비정상), BookmarkManager Load/Save 동작(파일 없음/손상/쓰기 실패 시 동작 검증).
- 문서화 및 기여 가이드
  - CONTRIBUTING.md에 로컬 빌드, 코드 스타일(예: 들여쓰기, 네이밍), PR 템플릿을 작성.
  - ISSUE_TEMPLATE.md, PULL_REQUEST_TEMPLATE.md 추가로 외부 기여 유도.
- 빌드 편의성 스크립트
  - Windows용 build.ps1 또는 build.bat 추가(1~2줄로 빌드 가능하게).
- 작은 리팩토링 학습 포인트
  - CMake: 하위 디렉터리별 CMakeLists.txt로 분리(각 모듈을 add_library로 만든 뒤 상위에서 target_link_libraries로 묶기).
  - 플랫폼 추상화: OS 관련 호출을 별도의 플랫폼 레이어에 두기(예: FileSystem, Window)의 인터페이스 도입.

## 5) 즉시 적용 가능한 체크리스트 (우선순위)
1. README 보강 (빌드/구조/데이터 경로/라이선스 경고) — 바로 가능.  
2. Skill.md 추가(이 파일) — 완료(이후 리포지토리에 커밋).  
3. 간단한 build 스크립트 추가(build.bat / build.ps1).  
4. Utils/Json에 GoogleTest 기반 테스트 1개 추가(파싱/문자열화).  
5. GitHub Actions 워크플로우: Windows 빌드 + artifact 업로드.  
6. 대용량 바이너리(1Xtentix.tar, 2Xtentix.tar) 용도 확인 후 필요 시 Git LFS 또는 Releases로 이동.

## 6) 예시 코드/명령 (빠르게 테스트할 때)
- 북마크 로컬 확인(런타임에서 확인하는 방법)
  - `%APPDATA%\Xtentix\bookmarks.json` 파일을 편집하거나 삭제해 프로그램 동작을 관찰.
- Json 유닛 테스트 스켈레톤 (의견)
  - 테스트 프레임워크: Catch2 single-header 또는 GoogleTest 사용.
  - 테스트 항목: Parse 빈 배열, Parse 잘못된 JSON -> JsonValue::Parse가 실패해도 안전한 동작 보장.

---
끝. 필요하면 이 Skill.md를 리포지토리 루트에 바로 커밋해 드릴게요(원하시면 commit message는 한국어로 남깁니다). 추가로 README의 빌드 스니펫을 제가 적용해 넣어줄까요?
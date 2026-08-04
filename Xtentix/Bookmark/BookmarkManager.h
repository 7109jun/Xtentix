#pragma once
#include <string>
#include <vector>

namespace Xtentix {

struct BookmarkEntry {
    std::string title;
    std::string url;
};

// %APPDATA%/Xtentix/bookmarks.json 에 자동 저장/로드된다 (Utils/Json 사용, 외부
// 라이브러리 없음). "사용자가 관리하지 않아도 쾌적한 브라우저" 철학에 따라
// 저장을 사용자가 신경 쓸 필요 없게, 변경될 때마다 자동으로 디스크에 반영한다.
class BookmarkManager {
public:
    void Add(const std::string& title, const std::string& url);
    void Remove(const std::string& url);
    bool Contains(const std::string& url) const;

    const std::vector<BookmarkEntry>& Entries() const { return m_entries; }

    // 시작 시 한 번 호출. 저장된 파일이 없거나 읽기 실패해도 조용히 빈 목록으로 시작.
    void Load();

    // 기본 저장 경로: %APPDATA%\Xtentix\bookmarks.json (APPDATA가 없으면 현재 폴더).
    static std::string DefaultFilePath();

private:
    void Save() const;

    std::vector<BookmarkEntry> m_entries;
};

} // namespace Xtentix

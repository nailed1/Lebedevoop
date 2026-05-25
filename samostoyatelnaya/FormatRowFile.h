#pragma once
#include <string>
#include <vector>

class FormatRowFile {
public:
    std::vector<std::string> fields;
    std::string raw;

    FormatRowFile();
    explicit FormatRowFile(const char* line);
    explicit FormatRowFile(const std::string& line);

    bool operator==(const FormatRowFile& o) const;
    bool operator<(const FormatRowFile& o) const;

    const std::string& toString() const;
    size_t fieldCount() const;
    const std::string& field(int idx) const;

private:
    void parse();
};

#include "FormatRowFile.h"
#include <stdexcept>

FormatRowFile::FormatRowFile() {}

FormatRowFile::FormatRowFile(const char* line) : raw(line) { parse(); }

FormatRowFile::FormatRowFile(const std::string& line) : raw(line) { parse(); }

bool FormatRowFile::operator==(const FormatRowFile& o) const { return raw == o.raw; }
bool FormatRowFile::operator<(const FormatRowFile& o)  const { return raw < o.raw;  }

const std::string& FormatRowFile::toString()   const { return raw; }
size_t             FormatRowFile::fieldCount() const { return fields.size(); }

const std::string& FormatRowFile::field(int idx) const {
    if (idx < 0 || idx >= (int)fields.size())
        throw std::out_of_range("field index out of range");
    return fields[idx];
}

void FormatRowFile::parse() {
    fields.clear();
    std::string tok;
    for (char c : raw) {
        if (c == ' ' || c == '\t') {
            if (!tok.empty()) { fields.push_back(tok); tok.clear(); }
        } else {
            tok += c;
        }
    }
    if (!tok.empty()) fields.push_back(tok);
}

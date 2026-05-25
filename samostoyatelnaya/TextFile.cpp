#include "TextFile.h"
#include <fstream>
#include <cstdio>
#include <set>
#include <algorithm>
#include <stdexcept>

// -------------------------------------------------------
// Конструкторы
// -------------------------------------------------------

template <class T>
TextFile<T>::TextFile() {}

template <class T>
TextFile<T>::TextFile(const char* filePath, bool write) : path_(filePath) {
    if (!write) loadFile(filePath);
}

template <class T>
TextFile<T>::TextFile(const char* dir, const char* mask) {
    // Формируем shell-команду вида: ls -1 dir/mask 2>/dev/null
    std::string cmd = std::string("ls -1 ") + dir + "/" + mask + " 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return;

    char buf[1024];
    while (fgets(buf, sizeof(buf), pipe)) {
        // убрать '\n' в конце
        std::string path(buf);
        if (!path.empty() && path.back() == '\n') path.pop_back();
        if (!path.empty()) appendFile(path.c_str());
    }
    pclose(pipe);
}

// -------------------------------------------------------
// Сохранение
// -------------------------------------------------------

template <class T>
void TextFile<T>::save(const char* filePath) const {
    std::ofstream f(filePath);
    if (!f) throw std::runtime_error(std::string("Cannot write: ") + filePath);
    for (const T& row : data) f << row.toString() << "\n";
}

template <class T>
void TextFile<T>::save() const {
    if (!path_.empty()) save(path_.c_str());
}

// -------------------------------------------------------
// Методы работы с данными
// -------------------------------------------------------

template <class T>
void TextFile<T>::appendFrom(const TextFile<T>& other) {
    for (const T& row : other.data) data.push_back(row);
}

template <class T>
TextFile<T> TextFile<T>::uniqueLines(const TextFile<T>& other) const {
    std::set<std::string> seen;
    TextFile<T> res;
    for (const T& r : data)
        if (seen.insert(r.toString()).second) res.data.push_back(r);
    for (const T& r : other.data)
        if (seen.insert(r.toString()).second) res.data.push_back(r);
    return res;
}

template <class T>
TextFile<T> TextFile<T>::commonLines(const TextFile<T>& other) const {
    std::set<std::string> inOther;
    for (const T& r : other.data) inOther.insert(r.toString());

    std::set<std::string> added;
    TextFile<T> res;
    for (const T& r : data)
        if (inOther.count(r.toString()) && added.insert(r.toString()).second)
            res.data.push_back(r);
    return res;
}

template <class T>
void TextFile<T>::sortByField(int fieldIdx, bool ascending) {
    std::sort(data.begin(), data.end(), [fieldIdx, ascending](const T& a, const T& b) {
        std::string fa = fieldIdx < (int)a.fieldCount() ? a.field(fieldIdx) : "";
        std::string fb = fieldIdx < (int)b.fieldCount() ? b.field(fieldIdx) : "";
        bool numA = false, numB = false;
        double da = 0, db = 0;
        try { da = std::stod(fa); numA = true; } catch (...) {}
        try { db = std::stod(fb); numB = true; } catch (...) {}
        if (numA && numB) return ascending ? da < db : da > db;
        return ascending ? fa < fb : fa > fb;
    });
}

template <class T>
double TextFile<T>::sumField(int fieldIdx) const {
    double sum = 0.0;
    for (const T& row : data) {
        if (fieldIdx < (int)row.fieldCount()) {
            try { sum += std::stod(row.field(fieldIdx)); } catch (...) {}
        }
    }
    return sum;
}

template <class T>
size_t TextFile<T>::size() const { return data.size(); }

// -------------------------------------------------------
// Операторы
// -------------------------------------------------------

template <class T>
TextFile<T>& TextFile<T>::operator+=(const TextFile<T>& other) {
    std::set<std::string> existing;
    for (const T& r : data) existing.insert(r.toString());
    for (const T& r : other.data)
        if (existing.insert(r.toString()).second)
            data.push_back(r);
    return *this;
}

template <class T>
TextFile<T> TextFile<T>::operator+(const TextFile<T>& other) const {
    TextFile<T> res = *this;
    res += other;
    return res;
}

template <class T>
TextFile<T>& TextFile<T>::operator-=(const TextFile<T>& other) {
    std::set<std::string> toRemove;
    for (const T& r : other.data) toRemove.insert(r.toString());
    std::vector<T> newData;
    for (const T& r : data)
        if (!toRemove.count(r.toString())) newData.push_back(r);
    data = std::move(newData);
    return *this;
}

template <class T>
TextFile<T> TextFile<T>::operator-(const TextFile<T>& other) const {
    std::set<std::string> setA, setB;
    for (const T& r : data)       setA.insert(r.toString());
    for (const T& r : other.data) setB.insert(r.toString());

    TextFile<T> res;
    for (const T& r : data)
        if (!setB.count(r.toString())) res.data.push_back(r);
    for (const T& r : other.data)
        if (!setA.count(r.toString())) res.data.push_back(r);
    return res;
}

// -------------------------------------------------------
// Приватные методы
// -------------------------------------------------------

template <class T>
void TextFile<T>::loadFile(const char* filePath) {
    std::ifstream f(filePath);
    if (!f) throw std::runtime_error(std::string("Cannot open: ") + filePath);
    std::string line;
    while (std::getline(f, line))
        if (!line.empty()) data.push_back(T(line.c_str()));
}

template <class T>
void TextFile<T>::appendFile(const char* filePath) {
    std::ifstream f(filePath);
    if (!f) return;
    std::string line;
    while (std::getline(f, line))
        if (!line.empty()) data.push_back(T(line.c_str()));
}

// -------------------------------------------------------
// Явная инстанциация
// -------------------------------------------------------
template class TextFile<FormatRowFile>;

#pragma once
#include "FormatRowFile.h"
#include <string>
#include <vector>

template <class T>
class TextFile {
public:
    std::vector<T> data;

    TextFile();

    // Открыть файл; write=true — создать пустой (не читать)
    explicit TextFile(const char* filePath, bool write = false);

    // Конструктор по маске: забирает все совпадающие файлы
    // pattern — shell-glob, например "STARLINK/*.dat"
    // Использует popen("ls -1 <pattern>") из <cstdio>
    TextFile(const char* dir, const char* mask);

    void save(const char* filePath) const;
    void save() const;

    // Дописать содержимое other в конец this
    void appendFrom(const TextFile<T>& other);

    // Уникальные строки из двух файлов (объединение без дублей)
    TextFile<T> uniqueLines(const TextFile<T>& other) const;

    // Общие строки (пересечение)
    TextFile<T> commonLines(const TextFile<T>& other) const;

    // Сортировка по колонке (fieldIdx), ascending/descending
    void sortByField(int fieldIdx, bool ascending = true);

    // Сумма чисел в колонке
    double sumField(int fieldIdx) const;

    size_t size() const;

    // += : добавить только неповторяющиеся строки
    TextFile<T>& operator+=(const TextFile<T>& other);

    // + : объединение без дублей
    TextFile<T> operator+(const TextFile<T>& other) const;

    // -= : удалить строки, присутствующие во втором файле
    TextFile<T>& operator-=(const TextFile<T>& other);

    // - : симметричная разность (строки только в одном из файлов)
    TextFile<T> operator-(const TextFile<T>& other) const;

private:
    std::string path_;

    void loadFile(const char* filePath);
    void appendFile(const char* filePath);
};

// Явные инстанциации — определены в TextFile.cpp
extern template class TextFile<FormatRowFile>;

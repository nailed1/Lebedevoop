#ifndef UTILS_H
#define UTILS_H

#include "graph.h"
#include <set>
#include <string>
#include <vector>

// Сохранить компоненту связности в файл
void saveComponentToFile(const std::set<Node*>& component, const std::string& filename);

// Сохранить все компоненты в отдельные файлы
void saveAllComponents(const std::vector<std::set<Node*>>& components, const std::string& baseFilename);

// Вывести информацию о графе
void printGraphInfo(const Graph& graph);

// Вывести информацию о компонентах
void printComponentsInfo(const std::vector<std::set<Node*>>& components);

#endif // UTILS_H
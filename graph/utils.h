#ifndef UTILS_H
#define UTILS_H

#include "graph.h"
#include <set>
#include <string>
#include <vector>

void printGraphInfo(const Graph& graph);

// Старые перегрузки (set<Node*>)
void printComponentsInfo(const std::vector<std::set<Node*>>& components);
void saveComponentToFile(const std::set<Node*>& component,
                         const std::string& filename);
void saveAllComponents(const std::vector<std::set<Node*>>& components,
                       const std::string& baseFilename);

// Новые перегрузки (Graph*)
void printComponentsInfo(const std::vector<Graph*>& components);
void saveComponentToFile(const Graph& component, const std::string& filename);
void saveAllComponents(const std::vector<Graph*>& components,
                       const std::string& baseFilename);

#endif // UTILS_H

#ifndef UTILS_H
#define UTILS_H

#include "graph.h"
#include <set>
#include <string>
#include <vector>

void printGraphInfo(const Graph& graph);
void printComponentsInfo(const std::vector<std::set<Node*>>& components);

void saveComponentToFile(const std::set<Node*>& component,
                         const std::string& filename);
void saveAllComponents(const std::vector<std::set<Node*>>& components,
                       const std::string& baseFilename);

#endif // UTILS_H

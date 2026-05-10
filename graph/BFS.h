#ifndef BFS_H
#define BFS_H

#include "graph.h"
#include <queue>
#include <set>
#include <vector>

class BFS {
    const Graph& graph;
    
public:
    BFS(const Graph& agraph);
    bool connected(Node* begin, Node* end);
    
    // Поиск всех узлов в компоненте связности
    std::set<Node*> findConnectedComponent(Node* start);
    
    // Поиск всех компонент связности в графе
    std::vector<std::set<Node*>> findAllConnectedComponents();
};

#endif // BFS_H
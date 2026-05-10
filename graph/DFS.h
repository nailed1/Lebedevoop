#ifndef DFS_H
#define DFS_H

#include "graph.h"
#include <set>

class DFS {
    const Graph& graph;
    std::set<Node*> visited;
    
    bool connected(Node* begin, Node* end, int depth);
    
public:
    DFS(const Graph& agraph);
    bool connected(Node* begin, Node* end);
};

#endif // DFS_H
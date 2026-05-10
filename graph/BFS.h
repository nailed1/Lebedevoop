#ifndef BFS_H
#define BFS_H

#include "graph.h"
#include <queue>
#include <set>

class BFS {
    const Graph& graph;
    
public:
    BFS(const Graph& agraph);
    bool connected(Node* begin, Node* end);
};

#endif // BFS_H
#ifndef DFS_H
#define DFS_H

#include "graph.h"
#include <set>

class DFS {
    [[maybe_unused]] const Graph& graph;

    static bool search(Node* current, Node* end, std::set<Node*>& visited);

public:
    explicit DFS(const Graph& graph);

    bool connected(Node* begin, Node* end);
};

#endif // DFS_H

#ifndef BFS_H
#define BFS_H

#include "graph.h"
#include <queue>
#include <set>
#include <vector>

class BFS {
    const Graph& graph;

public:
    explicit BFS(const Graph& graph);

    bool connected(Node* begin, Node* end);

    std::set<Node*>              findConnectedComponent(Node* start);
    std::vector<std::set<Node*>> findAllConnectedComponents();

    Graph*               findConnectedComponentAsGraph(Node* start);
    std::vector<Graph*>  findAllConnectedComponentGraphs();
};

#endif // BFS_H

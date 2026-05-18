#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"
#include <vector>
#include <map>

struct MarkedNode {
    Node* node;
    int   mark;
    Node* prev;

    MarkedNode(Node* node = nullptr, int mark = 0, Node* prev = nullptr);
};

class PriorityQueue {
    std::vector<MarkedNode> nodes;

public:
    void       push(Node* node, int mark, Node* prev);
    MarkedNode pop();
    bool       empty() const;
};

struct Way {
    std::vector<Node*> nodes;
    int length;

    Way();
};

class Dijkstra {
    [[maybe_unused]] const Graph& graph;

    static Way unroll(const std::map<Node*, MarkedNode>& visited,
                      Node* begin, Node* end);

public:
    explicit Dijkstra(const Graph& graph);

    Way shortestWay(Node* begin, Node* end);
};

#endif // DIJKSTRA_H

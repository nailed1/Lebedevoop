#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"
#include <vector>
#include <map>

// Структура для хранения помеченной вершины
struct MarkedNode {
    Node* node;
    int mark;       // Длина пути от начала
    Node* prev;     // Предыдущая вершина в пути
    
    MarkedNode(Node* anode = 0, int amark = 0, Node* aprev = 0);
};

// Очередь с приоритетами
class PriorityQueue {
    std::vector<MarkedNode> nodes;
    
public:
    MarkedNode pop();
    void push(Node* node, int mark, Node* prev);
    bool empty() const;
};

// Структура пути
struct Way {
    std::vector<Node*> nodes;
    int length;
    
    Way();
};

// Алгоритм Дейкстры
class Dijkstra {
    const Graph& graph;
    
    static Way unroll(std::map<Node*, MarkedNode> visited, Node* begin, Node* curr);
    
public:
    Dijkstra(const Graph& agraph);
    Way shortestWay(Node* begin, Node* end);
};

#endif // DIJKSTRA_H
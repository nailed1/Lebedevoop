#include "graph.h"
#include <algorithm>

// ========== РЕАЛИЗАЦИЯ NODE ==========

Node::Node(const std::string& aname) : name(aname) {}

const std::string& Node::getName() const {
    return name;
}

node_iterator Node::nb_begin() const {
    return neighbours.begin();
}

node_iterator Node::nb_end() const {
    return neighbours.end();
}

void Node::addNeighbour(Node* neighbour, int weight) {
    neighbours[neighbour] = weight;
}

void Node::removeNeighbour(Node* neighbour) {
    neighbours.erase(neighbour);
}

int Node::getWeight(Node* from) const {
    auto it = neighbours.find(from);
    if (it != neighbours.end()) {
        return it->second;
    }
    return 0;
}

// ========== РЕАЛИЗАЦИЯ GRAPH ==========

Graph::~Graph() {
    for (Node* n : nodes) {
        delete n;
    }
}

void Graph::addNode(Node* node) {
    nodes.insert(node);
}

void Graph::removeNode(Node* node) {
    nodes.erase(node);
    // Удаляем ссылки на этот узел у всех остальных
    for (Node* other : nodes) {
        other->removeNeighbour(node);
    }
}

void Graph::addEdge(Node* begin, Node* end, int weight) {
    if (nodes.find(begin) == nodes.end()) return;
    if (nodes.find(end) == nodes.end()) return;
    
    // Направленное ребро (как на вашей картинке со стрелками)
    begin->addNeighbour(end, weight);
}

void Graph::removeEdge(Node* begin, Node* end) {
    if (begin && end) {
        begin->removeNeighbour(end);
    }
}

node_iterator Graph::begin() const {
    return nodes.begin();
}

node_iterator Graph::end() const {
    return nodes.end();
}
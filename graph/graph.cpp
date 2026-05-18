#include "graph.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Node

Node::Node(const std::string& name) : name(name) {}

const std::string& Node::getName() const { return name; }

int Node::getNameAsInt() const { return std::stoi(name); }

neighbour_iterator Node::nb_begin() const { return neighbours.begin(); }
neighbour_iterator Node::nb_end()   const { return neighbours.end();   }

void Node::addNeighbour(Node* neighbour, int weight) {
    neighbours[neighbour] = weight;
}

void Node::removeNeighbour(Node* neighbour) {
    neighbours.erase(neighbour);
}

int Node::getWeight(Node* neighbour) const {
    auto it = neighbours.find(neighbour);
    return it != neighbours.end() ? it->second : 0;
}

// Graph

Graph::Graph(const char* filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << "\n";
        return;
    }

    std::string line;
    bool headerSkipped = false;

    while (std::getline(file, line)) {
        if (!headerSkipped) {
            headerSkipped = true;
            if (line.find("Source") != std::string::npos)
                continue;
        }

        if (line.empty()) continue;

        std::istringstream iss(line);
        int src, tgt;
        if (!(iss >> src >> tgt)) continue;

        const std::string srcName = std::to_string(src);
        const std::string tgtName = std::to_string(tgt);

        Node* srcNode = getNode(srcName);
        if (!srcNode) {
            srcNode = new Node(srcName);
            addNode(srcNode);
        }

        Node* tgtNode = getNode(tgtName);
        if (!tgtNode) {
            tgtNode = new Node(tgtName);
            addNode(tgtNode);
        }

        addEdge(srcNode, tgtNode);
    }
}

Graph::~Graph() { clear(); }

void Graph::addNode(Node* node) {
    if (!node) return;
    nodes.insert(node);
    nodeByName[node->getName()] = node;
}

void Graph::removeNode(Node* node) {
    if (!node) return;
    nodeByName.erase(node->getName());
    nodes.erase(node);
    for (Node* other : nodes)
        other->removeNeighbour(node);
}

void Graph::addEdge(Node* from, Node* to, int weight) {
    if (!from || !to) return;
    if (!nodes.count(from) || !nodes.count(to)) return;
    from->addNeighbour(to, weight);
    to->addNeighbour(from, weight);
}

void Graph::removeEdge(Node* from, Node* to) {
    if (!from || !to) return;
    from->removeNeighbour(to);
    to->removeNeighbour(from);
}

Node* Graph::getNode(const std::string& name) const {
    auto it = nodeByName.find(name);
    return it != nodeByName.end() ? it->second : nullptr;
}

Node* Graph::getNodeByIndex(int index) const {
    return getNode(std::to_string(index));
}

node_iterator Graph::begin() const { return nodes.begin(); }
node_iterator Graph::end()   const { return nodes.end();   }

size_t Graph::size() const { return nodes.size(); }

void Graph::clear() {
    for (Node* n : nodes)
        delete n;
    nodes.clear();
    nodeByName.clear();
}

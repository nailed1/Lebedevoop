#include "graph.h"
#include <fstream>
#include <sstream>
#include <iostream>

// ========== РЕАЛИЗАЦИЯ NODE ==========

Node::Node(const std::string& aname) : name(aname) {}

const std::string& Node::getName() const {
    return name;
}

int Node::getNameAsInt() const {
    return std::stoi(name);
}

neighbour_iterator Node::nb_begin() const {
    return neighbours.begin();
}

neighbour_iterator Node::nb_end() const {
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

Graph::Graph() {}

// Конструктор из файла
Graph::Graph(const char* file_name) {
    std::ifstream file(file_name);
    if (!file.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл " << file_name << std::endl;
        return;
    }
    
    std::string line;
    bool headerSkipped = false;
    
    while (std::getline(file, line)) {
        // Пропускаем заголовок
        if (!headerSkipped) {
            if (line.find("Source") != std::string::npos) {
                headerSkipped = true;
                continue;
            }
            headerSkipped = true;
        }
        
        // Пропускаем пустые строки
        if (line.empty()) continue;
        
        std::istringstream iss(line);
        int source, target;
        if (iss >> source >> target) {
            // Создаем или получаем узлы
            Node* sourceNode = getNodeByIndex(source);
            if (!sourceNode) {
                sourceNode = new Node(std::to_string(source));
                addNode(sourceNode);
            }
            
            Node* targetNode = getNodeByIndex(target);
            if (!targetNode) {
                targetNode = new Node(std::to_string(target));
                addNode(targetNode);
            }
            
            // Добавляем ребро (ненаправленное)
            addEdge(sourceNode, targetNode, 1);
        }
    }
    
    file.close();
}

Graph::~Graph() {
    clear();
}

void Graph::addNode(Node* node) {
    nodes.insert(node);
    if (node) {
        int index = node->getNameAsInt();
        nodesByIndex[index] = node;
    }
}

void Graph::removeNode(Node* node) {
    if (node) {
        int index = node->getNameAsInt();
        nodesByIndex.erase(index);
    }
    nodes.erase(node);
    // Удаляем ссылки на этот узел у всех остальных
    for (Node* other : nodes) {
        other->removeNeighbour(node);
    }
}

void Graph::addEdge(Node* begin, Node* end, int weight) {
    if (!begin || !end) return;
    if (nodes.find(begin) == nodes.end()) return;
    if (nodes.find(end) == nodes.end()) return;
    
    // Ненаправленное ребро
    begin->addNeighbour(end, weight);
    end->addNeighbour(begin, weight);
}

void Graph::removeEdge(Node* begin, Node* end) {
    if (begin && end) {
        begin->removeNeighbour(end);
        end->removeNeighbour(begin);
    }
}

Node* Graph::getNodeByIndex(int index) {
    auto it = nodesByIndex.find(index);
    if (it != nodesByIndex.end()) {
        return it->second;
    }
    return nullptr;
}

node_iterator Graph::begin() const {
    return nodes.begin();
}

node_iterator Graph::end() const {
    return nodes.end();
}

size_t Graph::size() const {
    return nodes.size();
}

void Graph::clear() {
    for (Node* n : nodes) {
        delete n;
    }
    nodes.clear();
    nodesByIndex.clear();
}
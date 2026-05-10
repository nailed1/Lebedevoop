#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <set>
#include <map>

// Объявление типа итератора
typedef std::set<Node*>::const_iterator node_iterator;

// Класс Узла графа
class Node {
    std::string name;
    std::map<Node*, int> neighbours;  // Сосед + вес ребра
    
    // Друзья получают доступ к приватным полям
    friend class Graph;
    friend class BFS;
    friend class DFS;
    friend class Dijkstra;
    
public:
    Node(const std::string& aname);
    const std::string& getName() const;
    
    // Итераторы для обхода соседей
    node_iterator nb_begin() const;
    node_iterator nb_end() const;
    
    // Методы для работы с соседями (только для друзей)
    void addNeighbour(Node* neighbour, int weight);
    void removeNeighbour(Node* neighbour);
    int getWeight(Node* from) const;
};

// Класс Графа
class Graph {
    std::set<Node*> nodes;
    
public:
    ~Graph();
    
    void addNode(Node* node);
    void removeNode(Node* node);
    void addEdge(Node* begin, Node* end, int weight);
    void removeEdge(Node* begin, Node* end);
    
    // Итераторы для обхода всех узлов
    node_iterator begin() const;
    node_iterator end() const;
};

#endif // GRAPH_H
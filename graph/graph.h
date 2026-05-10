#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <set>
#include <map>
#include <vector>

// Предварительное объявление
class Node;

// Итератор для обхода узлов графа (по set<Node*>)
typedef std::set<Node*>::const_iterator node_iterator;

// Итератор для обхода соседей узла (по map<Node*, int>)
typedef std::map<Node*, int>::const_iterator neighbour_iterator;

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
    int getNameAsInt() const;  // Преобразовать имя в число
    
    // Итераторы для обхода соседей (возвращаем neighbour_iterator!)
    neighbour_iterator nb_begin() const;
    neighbour_iterator nb_end() const;
    
    // Методы для работы с соседями (только для друзей)
    void addNeighbour(Node* neighbour, int weight);
    void removeNeighbour(Node* neighbour);
    int getWeight(Node* from) const;
};

// Класс Графа
class Graph {
    std::set<Node*> nodes;
    std::map<int, Node*> nodesByIndex;  // Для быстрого поиска по номеру
    
public:
    Graph();  // Конструктор по умолчанию
    Graph(const char* file_name);  // Конструктор из файла
    ~Graph();
    
    void addNode(Node* node);
    void removeNode(Node* node);
    void addEdge(Node* begin, Node* end, int weight = 1);
    void removeEdge(Node* begin, Node* end);
    
    // Поиск узла по индексу
    Node* getNodeByIndex(int index);
    
    // Итераторы для обхода всех узлов (возвращаем node_iterator)
    node_iterator begin() const;
    node_iterator end() const;
    
    // Получение размера
    size_t size() const;
    
    // Очистка графа
    void clear();
};

#endif // GRAPH_H
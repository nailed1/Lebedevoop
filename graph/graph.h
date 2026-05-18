#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <map>
#include <set>

class Node;

typedef std::set<Node*>::const_iterator      node_iterator;
typedef std::map<Node*, int>::const_iterator neighbour_iterator;

class Node {
    std::string name;
    std::map<Node*, int> neighbours;

public:
    explicit Node(const std::string& name);

    const std::string& getName() const;
    int getNameAsInt() const;

    neighbour_iterator nb_begin() const;
    neighbour_iterator nb_end() const;

    void addNeighbour(Node* neighbour, int weight);
    void removeNeighbour(Node* neighbour);
    int  getWeight(Node* neighbour) const;
};

class Graph {
    std::set<Node*>          nodes;
    std::map<std::string, Node*> nodeByName;

public:
    Graph() = default;
    explicit Graph(const char* filename);
    ~Graph();

    Graph(const Graph&)            = delete;
    Graph& operator=(const Graph&) = delete;

    void addNode(Node* node);
    void removeNode(Node* node);
    void addEdge(Node* from, Node* to, int weight = 1);
    void removeEdge(Node* from, Node* to);

    Node* getNode(const std::string& name) const;
    Node* getNodeByIndex(int index) const;

    node_iterator begin() const;
    node_iterator end() const;
    size_t size() const;

    void clear();
};

#endif // GRAPH_H

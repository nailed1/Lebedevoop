#include "BFS.h"

BFS::BFS(const Graph& agraph) : graph(agraph) {}

bool BFS::connected(Node* begin, Node* end) {
    std::queue<Node*> nodes_queue;
    nodes_queue.push(begin);
    std::set<Node*> visited;
    
    while (!nodes_queue.empty()) {
        Node* next = nodes_queue.front();
        nodes_queue.pop();
        
        if (end == next) return true;
        
        visited.insert(next);
        
        for (neighbour_iterator it = next->nb_begin(); it != next->nb_end(); ++it) {
            if (visited.find(it->first) == visited.end()) {
                nodes_queue.push(it->first);
            }
        }
    }
    
    return false;
}

// Поиск всех узлов в компоненте связности
std::set<Node*> BFS::findConnectedComponent(Node* start) {
    std::set<Node*> component;
    std::queue<Node*> nodes_queue;
    
    nodes_queue.push(start);
    component.insert(start);
    
    while (!nodes_queue.empty()) {
        Node* current = nodes_queue.front();
        nodes_queue.pop();
        
        for (neighbour_iterator it = current->nb_begin(); it != current->nb_end(); ++it) {
            Node* neighbor = it->first;
            if (component.find(neighbor) == component.end()) {
                component.insert(neighbor);
                nodes_queue.push(neighbor);
            }
        }
    }
    
    return component;
}

// Поиск всех компонент связности в графе
std::vector<std::set<Node*>> BFS::findAllConnectedComponents() {
    std::vector<std::set<Node*>> components;
    std::set<Node*> visited;
    
    for (node_iterator it = graph.begin(); it != graph.end(); ++it) {
        Node* node = *it;
        
        if (visited.find(node) == visited.end()) {
            // Нашли новую компоненту связности
            std::set<Node*> component = findConnectedComponent(node);
            components.push_back(component);
            
            // Добавляем все узлы компоненты в visited
            for (Node* n : component) {
                visited.insert(n);
            }
        }
    }
    
    return components;
}
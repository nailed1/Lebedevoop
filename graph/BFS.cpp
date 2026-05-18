#include "BFS.h"

BFS::BFS(const Graph& graph) : graph(graph) {}

bool BFS::connected(Node* begin, Node* end) {
    if (begin == end) return true;

    std::queue<Node*> queue;
    std::set<Node*>   visited;

    queue.push(begin);
    visited.insert(begin);

    while (!queue.empty()) {
        Node* current = queue.front();
        queue.pop();

        if (current == end) return true;

        for (auto it = current->nb_begin(); it != current->nb_end(); ++it) {
            if (visited.insert(it->first).second)
                queue.push(it->first);
        }
    }

    return false;
}

std::set<Node*> BFS::findConnectedComponent(Node* start) {
    std::set<Node*>   component;
    std::queue<Node*> queue;

    queue.push(start);
    component.insert(start);

    while (!queue.empty()) {
        Node* current = queue.front();
        queue.pop();

        for (auto it = current->nb_begin(); it != current->nb_end(); ++it) {
            if (component.insert(it->first).second)
                queue.push(it->first);
        }
    }

    return component;
}

std::vector<std::set<Node*>> BFS::findAllConnectedComponents() {
    std::vector<std::set<Node*>> components;
    std::set<Node*>              visited;

    for (Node* node : graph) {
        if (visited.count(node)) continue;

        std::set<Node*> component = findConnectedComponent(node);
        visited.insert(component.begin(), component.end());
        components.push_back(std::move(component));
    }

    return components;
}

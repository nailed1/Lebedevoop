#include "BFS.h"
#include <map>

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

static Graph* buildGraphFromComponent(const std::set<Node*>& component) {
    Graph* g = new Graph();
    std::map<Node*, Node*> nodeMap;

    for (Node* node : component) {
        Node* newNode = new Node(node->getName());
        g->addNode(newNode);
        nodeMap[node] = newNode;
    }

    for (Node* node : component) {
        for (auto it = node->nb_begin(); it != node->nb_end(); ++it) {
            Node* neighbour = it->first;
            if (component.count(neighbour) && node->getName() < neighbour->getName())
                g->addEdge(nodeMap[node], nodeMap[neighbour], it->second);
        }
    }

    return g;
}

Graph* BFS::findConnectedComponentAsGraph(Node* start) {
    return buildGraphFromComponent(findConnectedComponent(start));
}

std::vector<Graph*> BFS::findAllConnectedComponentGraphs() {
    std::vector<Graph*> result;
    std::set<Node*>     visited;

    for (Node* node : graph) {
        if (visited.count(node)) continue;

        std::set<Node*> component = findConnectedComponent(node);
        visited.insert(component.begin(), component.end());
        result.push_back(buildGraphFromComponent(component));
    }

    return result;
}

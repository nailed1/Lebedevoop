#include "DFS.h"

DFS::DFS(const Graph& graph) : graph(graph) {}

bool DFS::connected(Node* begin, Node* end) {
    std::set<Node*> visited;
    return search(begin, end, visited);
}

bool DFS::search(Node* current, Node* end, std::set<Node*>& visited) {
    if (current == end) return true;

    visited.insert(current);

    for (auto it = current->nb_begin(); it != current->nb_end(); ++it) {
        if (!visited.count(it->first) && search(it->first, end, visited))
            return true;
    }

    return false;
}

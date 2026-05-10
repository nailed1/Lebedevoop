#include "dfs.h"

DFS::DFS(const Graph& agraph) : graph(agraph) {}

bool DFS::connected(Node* begin, Node* end) {
    visited.clear();
    return connected(begin, end, 0);
}

bool DFS::connected(Node* begin, Node* end, int depth) {
    if (begin == end) return true;
    
    visited.insert(begin);
    
    for (node_iterator it = begin->nb_begin(); it != begin->nb_end(); ++it) {
        if (visited.find(it->first) == visited.end()) {
            if (connected(it->first, end, depth + 1)) {
                return true;
            }
        }
    }
    
    return false;
}
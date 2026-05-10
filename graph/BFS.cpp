#include "bfs.h"

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
        
        for (node_iterator it = next->nb_begin(); it != next->nb_end(); ++it) {
            if (visited.find(it->first) == visited.end()) {
                nodes_queue.push(it->first);
            }
        }
    }
    
    return false;
}
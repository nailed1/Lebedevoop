#include "dijkstra.h"
#include <algorithm>

// РЕАЛИЗАЦИЯ MarkedNode

MarkedNode::MarkedNode(Node* anode, int amark, Node* aprev)
    : node(anode), mark(amark), prev(aprev) {}

// РЕАЛИЗАЦИЯ PriorityQueue

MarkedNode PriorityQueue::pop() {
    MarkedNode mn = nodes.back();
    nodes.pop_back();
    return mn;
}

void PriorityQueue::push(Node* node, int mark, Node* prev) {
    MarkedNode mn(node, mark, prev);
    
    // Вставляем в отсортированный вектор (от большего к меньшему)
    std::vector<MarkedNode>::iterator it = nodes.begin();
    while (it != nodes.end() && mark < it->mark) {
        ++it;
    }
    
    if (it == nodes.end()) {
        nodes.push_back(mn);
    } else {
        nodes.insert(it, mn);
    }
}

bool PriorityQueue::empty() const {
    return nodes.empty();
}

// РЕАЛИЗАЦИЯ Way

Way::Way() : length(-1) {}

// РЕАЛИЗАЦИЯ Dijkstra

Dijkstra::Dijkstra(const Graph& agraph) : graph(agraph) {}

Way Dijkstra::shortestWay(Node* begin, Node* end) {
    PriorityQueue nodes;
    nodes.push(begin, 0, 0);
    std::map<Node*, MarkedNode> visited;
    
    while (!nodes.empty()) {
        MarkedNode next = nodes.pop();
        visited[next.node] = next;
        
        if (end == next.node) {
            return unroll(visited, begin, end);
        }
        
        for (neighbour_iterator it = next.node->nb_begin(); it != next.node->nb_end(); ++it) {
            Node* neighbor = it->first;
            int weight = it->second;
            int newMark = next.mark + weight;
            
            if (visited.find(neighbor) == visited.end()) {
                nodes.push(neighbor, newMark, next.node);
            }
        }
    }
    
    return Way();  // Пути нет
}

Way Dijkstra::unroll(std::map<Node*, MarkedNode> visited, Node* begin, Node* curr) {
    Way way;
    way.length = visited[curr].mark;
    
    while (curr != begin) {
        way.nodes.push_back(curr);
        curr = visited[curr].prev;
    }
    
    way.nodes.push_back(begin);
    std::reverse(way.nodes.begin(), way.nodes.end());
    
    return way;
}
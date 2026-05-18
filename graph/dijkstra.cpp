#include "dijkstra.h"
#include <algorithm>

// MarkedNode

MarkedNode::MarkedNode(Node* node, int mark, Node* prev)
    : node(node), mark(mark), prev(prev) {}

// PriorityQueue (min по mark — меньшее значение извлекается первым)

void PriorityQueue::push(Node* node, int mark, Node* prev) {
    // Вставляем так, чтобы вектор оставался отсортирован по убыванию:
    // минимальный mark окажется в конце и будет извлечён через pop_back.
    auto it = nodes.begin();
    while (it != nodes.end() && mark < it->mark)
        ++it;
    nodes.insert(it, MarkedNode(node, mark, prev));
}

MarkedNode PriorityQueue::pop() {
    MarkedNode mn = nodes.back();
    nodes.pop_back();
    return mn;
}

bool PriorityQueue::empty() const { return nodes.empty(); }

// ── Way ───────────────────────────────────────────────────────────────────────

Way::Way() : length(-1) {}

// ── Dijkstra ──────────────────────────────────────────────────────────────────

Dijkstra::Dijkstra(const Graph& graph) : graph(graph) {}

Way Dijkstra::shortestWay(Node* begin, Node* end) {
    PriorityQueue             queue;
    std::map<Node*, MarkedNode> visited;

    queue.push(begin, 0, nullptr);

    while (!queue.empty()) {
        MarkedNode next = queue.pop();

        // Узел уже обработан с меньшим расстоянием — пропускаем дубликат
        if (visited.count(next.node)) continue;

        visited[next.node] = next;

        if (next.node == end)
            return unroll(visited, begin, end);

        for (auto it = next.node->nb_begin(); it != next.node->nb_end(); ++it) {
            if (!visited.count(it->first))
                queue.push(it->first, next.mark + it->second, next.node);
        }
    }

    return Way(); // путь не найден
}

Way Dijkstra::unroll(const std::map<Node*, MarkedNode>& visited,
                     Node* begin, Node* end) {
    Way way;
    way.length = visited.at(end).mark;

    for (Node* curr = end; curr != begin; curr = visited.at(curr).prev)
        way.nodes.push_back(curr);

    way.nodes.push_back(begin);
    std::reverse(way.nodes.begin(), way.nodes.end());
    return way;
}

#include <string>
#include <map>
#include <set>

class Node {
    std::string name;
    std::map<Node*, int> neighbours;
    friend class Graph;
    friend class BFS;
    friend class DFS;
    friend class Dijkstra;

public:
    explicit Node(const std::string& aname) : name(aname) {}
    const std::string& getName() const { return name; }
    const std::map<Node*, int>& getNeighbours() const { return neighbours; }
};
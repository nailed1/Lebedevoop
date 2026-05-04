#include <vector>
#include <set>
#include <queue>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

class Node {
    private:
        string name;
        map<Node*, int> neighbours; //сосед и вес ребра
        
        //подключаем дружественные классы
        friend class Graph;
        friend class BFS;
        friend class DFS;
        friend class Dijkstra;

    public:
        Node(const string& aname) : name(aname) {};
        const string& getName() const {return name;}
        
        //вспомогательные для обхода
        map<Node*, int>::const_iterator nb_begin() const { return neighbours.begin(); }
        map<Node*, int>::const_iterator nb_end() const { return neighbours.end(); }
}
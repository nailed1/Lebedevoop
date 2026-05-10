#include <iostream>
#include <locale>
#include "graph.h"
#include "bfs.h"
#include "dfs.h"
#include "dijkstra.h"

int main() {
    setlocale(LC_ALL, "Russian");
    
    Graph g;
    
    // Создаём вершины 1-9
    Node* n1 = new Node("1");
    Node* n2 = new Node("2");
    Node* n3 = new Node("3");
    Node* n4 = new Node("4");
    Node* n5 = new Node("5");
    Node* n6 = new Node("6");
    Node* n7 = new Node("7");
    Node* n8 = new Node("8");
    Node* n9 = new Node("9");
    
    // Добавляем в граф
    g.addNode(n1);
    g.addNode(n2);
    g.addNode(n3);
    g.addNode(n4);
    g.addNode(n5);
    g.addNode(n6);
    g.addNode(n7);
    g.addNode(n8);
    g.addNode(n9);
    
    // Добавляем НАПРАВЛЕННЫЕ рёбра с весами (строго по стрелкам на картинке)
    
    // Из вершины 1:
    g.addEdge(n1, n2, 10);
    g.addEdge(n1, n4, 8);
    g.addEdge(n1, n3, 6);
    g.addEdge(n1, n5, 13);
    
    // Из вершины 2:
    g.addEdge(n2, n7, 11);
    g.addEdge(n2, n4, 5);
    
    // Из вершины 3:
    g.addEdge(n3, n5, 3);
    
    // Из вершины 4:
    g.addEdge(n4, n3, 2);
    g.addEdge(n4, n7, 12);
    g.addEdge(n4, n6, 7);
    g.addEdge(n4, n5, 5);
    
    // Из вершины 5:
    g.addEdge(n5, n6, 9);
    g.addEdge(n5, n9, 12);
    
    // Из вершины 6:
    g.addEdge(n6, n8, 8);
    g.addEdge(n6, n9, 10);
    
    // Из вершины 7:
    g.addEdge(n7, n6, 4);
    g.addEdge(n7, n8, 6);
    g.addEdge(n7, n9, 16);
    
    // Из вершины 8:
    g.addEdge(n8, n9, 15);
    
    // Проверяем связность
    std::cout << "BFS: 1 связана с 9? " 
              << (BFS(g).connected(n1, n9) ? "Да" : "Нет") << std::endl;
    
    std::cout << "DFS: 1 связана с 9? " 
              << (DFS(g).connected(n1, n9) ? "Да" : "Нет") << std::endl;
    
    // Находим кратчайший путь алгоритмом Дейкстры
    std::cout << "\nПоиск кратчайшего пути из 1 в 9..." << std::endl;
    Way result = Dijkstra(g).shortestWay(n1, n9);
    
    if (result.length != -1) {
        std::cout << "\nптимальный путь найден!" << std::endl;
        std::cout << "Длина пути: " << result.length << std::endl;
        std::cout << "Маршрут: ";
        for (size_t i = 0; i < result.nodes.size(); ++i) {
            std::cout << result.nodes[i]->getName();
            if (i < result.nodes.size() - 1) {
                std::cout << " -> ";
            }
        }
        std::cout << std::endl;
    } else {
        std::cout << "\nПути не существует!" << std::endl;
    }
    
    return 0;
}
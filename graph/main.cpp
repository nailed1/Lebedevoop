#include <iostream>
#include <locale>
#include <string>
#include "graph.h"
#include "BFS.h"
#include "DFS.h"
#include "dijkstra.h"
#include "utils.h"

// ========== ПРОТОТИПЫ ФУНКЦИЙ ДЛЯ ВСЕХ ЗАДАЧ ==========

// Задачи с кратчайшими путями (из лекции)
void task_shortestPathAtoI();
void task_shortestPath1to9();

// Задачи с непересекающимися графами
void task_findDisjointGraphs_testgraph();
void task_findDisjointGraphs_1000csv();

// Дополнительные задачи
void task_checkConnectivity();

// Показ меню
void showMainMenu();

// ========== ГЛАВНАЯ ФУНКЦИЯ ==========

int main() {
    setlocale(LC_ALL, "Russian");
    
    int choice;
    
    do {
        showMainMenu();
        std::cin >> choice;
        
        switch (choice) {
            case 1:
                task_shortestPathAtoI();
                break;
                
            case 2:
                task_shortestPath1to9();
                break;
                
            case 3:
                task_findDisjointGraphs_testgraph();
                break;
                
            case 4:
                task_findDisjointGraphs_1000csv();
                break;
                
            case 5:
                task_checkConnectivity();
                break;
                
            case 0:
                std::cout << "\n>>> Завершение программы..." << std::endl;
                break;
                
            default:
                std::cout << "\n>>> Неверный выбор! Попробуйте снова." << std::endl;
                break;
        }
        
        if (choice != 0) {
            std::cout << "\nНажмите Enter для продолжения...";
            std::cin.ignore();
            std::cin.get();
        }
        
    } while (choice != 0);
    
    return 0;
}

// ========== МЕНЮ ==========

void showMainMenu() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "       АЛГОРИТМЫ НА ГРАФАХ" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\n>>> ЗАДАЧИ С КРАТЧАЙШИМИ ПУТЯМИ:" << std::endl;
    std::cout << "1. Кратчайший путь из A в I (граф из лекции)" << std::endl;
    std::cout << "2. Кратчайший путь из 1 в 9 (ваш граф)" << std::endl;
    std::cout << "\n>>> ПОИСК НЕПЕРЕСЕКАЮЩИХСЯ ГРАФОВ:" << std::endl;
    std::cout << "3. Поиск компонент связности (testgraph.txt)" << std::endl;
    std::cout << "4. Поиск компонент связности (1000.csv)" << std::endl;
    std::cout << "\n>>> ДОПОЛНИТЕЛЬНЫЕ ЗАДАЧИ:" << std::endl;
    std::cout << "5. Проверка связности двух вершин" << std::endl;
    std::cout << "\n>>> 0. ВЫХОД" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Выберите задачу: ";
}

// ========== ЗАДАЧА 1: Кратчайший путь A -> I ==========

void task_shortestPathAtoI() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "   ЗАДАЧА 1: Кратчайший путь A -> I" << std::endl;
    std::cout << "========================================" << std::endl;
    
    Graph g;
    
    // Создаём вершины A-I
    Node* a = new Node("A");
    Node* b = new Node("B");
    Node* c = new Node("C");
    Node* d = new Node("D");
    Node* e = new Node("E");
    Node* f = new Node("F");
    Node* g_node = new Node("G");
    Node* h = new Node("H");
    Node* i = new Node("I");
    
    g.addNode(a); g.addNode(b); g.addNode(c);
    g.addNode(d); g.addNode(e); g.addNode(f);
    g.addNode(g_node); g.addNode(h); g.addNode(i);
    
    // Добавляем рёбра (ненаправленные)
    g.addEdge(a, b, 5);
    g.addEdge(a, c, 3);
    g.addEdge(b, d, 4);
    g.addEdge(b, e, 2);
    g.addEdge(c, e, 6);
    g.addEdge(d, f, 7);
    g.addEdge(d, g_node, 3);
    g.addEdge(e, f, 1);
    g.addEdge(e, h, 5);
    g.addEdge(f, i, 4);
    g.addEdge(g_node, i, 2);
    g.addEdge(h, i, 6);
    
    // Проверяем связность
    std::cout << "BFS: A связана с I? " 
              << (BFS(g).connected(a, i) ? "Да" : "Нет") << std::endl;
    
    std::cout << "DFS: A связана с I? " 
              << (DFS(g).connected(a, i) ? "Да" : "Нет") << std::endl;
    
    // Находим кратчайший путь
    Way result = Dijkstra(g).shortestWay(a, i);
    
    if (result.length != -1) {
        std::cout << "\n✅ Оптимальный путь найден!" << std::endl;
        std::cout << "📏 Длина пути: " << result.length << std::endl;
        std::cout << "🛤️  Маршрут: ";
        for (size_t i = 0; i < result.nodes.size(); ++i) {
            std::cout << result.nodes[i]->getName();
            if (i < result.nodes.size() - 1) {
                std::cout << " -> ";
            }
        }
        std::cout << std::endl;
    } else {
        std::cout << "\n❌ Пути не существует!" << std::endl;
    }
}

// ========== ЗАДАЧА 2: Кратчайший путь 1 -> 9 ==========

void task_shortestPath1to9() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "   ЗАДАЧА 2: Кратчайший путь 1 -> 9" << std::endl;
    std::cout << "========================================" << std::endl;
    
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
    
    g.addNode(n1); g.addNode(n2); g.addNode(n3);
    g.addNode(n4); g.addNode(n5); g.addNode(n6);
    g.addNode(n7); g.addNode(n8); g.addNode(n9);
    
    // Добавляем НАПРАВЛЕННЫЕ рёбра с весами (строго по стрелкам)
    g.addEdge(n1, n2, 10);
    g.addEdge(n1, n4, 8);
    g.addEdge(n1, n3, 6);
    g.addEdge(n1, n5, 13);
    
    g.addEdge(n2, n7, 11);
    g.addEdge(n2, n4, 5);
    
    g.addEdge(n3, n4, 2);
    g.addEdge(n3, n5, 3);
    
    g.addEdge(n4, n7, 12);
    g.addEdge(n4, n6, 7);
    g.addEdge(n4, n5, 5);
    
    g.addEdge(n5, n6, 9);
    g.addEdge(n5, n9, 12);
    
    g.addEdge(n6, n8, 8);
    g.addEdge(n6, n9, 10);
    
    g.addEdge(n7, n6, 4);
    g.addEdge(n7, n8, 6);
    g.addEdge(n7, n9, 16);
    
    g.addEdge(n8, n9, 15);
    
    // Проверяем связность
    std::cout << "BFS: 1 связана с 9? " 
              << (BFS(g).connected(n1, n9) ? "Да" : "Нет") << std::endl;
    
    std::cout << "DFS: 1 связана с 9? " 
              << (DFS(g).connected(n1, n9) ? "Да" : "Нет") << std::endl;
    
    // Находим кратчайший путь
    Way result = Dijkstra(g).shortestWay(n1, n9);
    
    if (result.length != -1) {
        std::cout << "\n✅ Оптимальный путь найден!" << std::endl;
        std::cout << "📏 Длина пути: " << result.length << std::endl;
        std::cout << "🛤️  Маршрут: ";
        for (size_t i = 0; i < result.nodes.size(); ++i) {
            std::cout << result.nodes[i]->getName();
            if (i < result.nodes.size() - 1) {
                std::cout << " -> ";
            }
        }
        std::cout << std::endl;
    } else {
        std::cout << "\n❌ Пути не существует!" << std::endl;
    }
}

// ========== ЗАДАЧА 3: Непересекающиеся графы (testgraph.txt) ==========

void task_findDisjointGraphs_testgraph() {
    std::cout << "\n========================================" << std::endl;
    std::cout << " ЗАДАЧА 3: Компоненты связности" << std::endl;
    std::cout << "         (testgraph.txt)" << std::endl;
    std::cout << "========================================" << std::endl;
    
    Graph graph("testgraph.txt");
    printGraphInfo(graph);
    
    if (graph.size() == 0) {
        std::cout << "Граф пуст или файл не найден!" << std::endl;
        return;
    }
    
    BFS bfs(graph);
    std::vector<std::set<Node*>> components = bfs.findAllConnectedComponents();
    
    printComponentsInfo(components);
    
    // Сохраняем каждую компоненту в отдельный файл
    std::cout << "\n>>> Сохранение компонент в файлы..." << std::endl;
    saveAllComponents(components, "testgraph");
    
    std::cout << "\n>>> Задача 3 выполнена успешно!" << std::endl;
}

// ========== ЗАДАЧА 4: Непересекающиеся графы (1000.csv) ==========

void task_findDisjointGraphs_1000csv() {
    std::cout << "\n========================================" << std::endl;
    std::cout << " ЗАДАЧА 4: Компоненты связности" << std::endl;
    std::cout << "         (1000.csv)" << std::endl;
    std::cout << "========================================" << std::endl;
    
    Graph graph("1000.csv");
    printGraphInfo(graph);
    
    if (graph.size() == 0) {
        std::cout << "Граф пуст или файл не найден!" << std::endl;
        return;
    }
    
    BFS bfs(graph);
    std::vector<std::set<Node*>> components = bfs.findAllConnectedComponents();
    
    printComponentsInfo(components);
    
    // Сохраняем каждую компоненту в отдельный файл
    std::cout << "\n>>> Сохранение компонент в файлы..." << std::endl;
    saveAllComponents(components, "graph1000");
    
    std::cout << "\n>>> Задача 4 выполнена успешно!" << std::endl;
}

// ========== ЗАДАЧА 5: Проверка связности двух вершин ==========

void task_checkConnectivity() {
    std::cout << "\n========================================" << std::endl;
    std::cout << " ЗАДАЧА 5: Проверка связности вершин" << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::string filename;
    std::cout << "Введите имя файла с графом: ";
    std::cin >> filename;
    
    Graph graph(filename.c_str());
    
    if (graph.size() == 0) {
        std::cout << "Граф пуст или файл не найден!" << std::endl;
        return;
    }
    
    printGraphInfo(graph);
    
    int v1, v2;
    std::cout << "Введите номер первой вершины: ";
    std::cin >> v1;
    std::cout << "Введите номер второй вершины: ";
    std::cin >> v2;
    
    Node* node1 = graph.getNodeByIndex(v1);
    Node* node2 = graph.getNodeByIndex(v2);
    
    if (!node1 || !node2) {
        std::cout << "Одна или обе вершины не найдены в графе!" << std::endl;
        return;
    }
    
    BFS bfs(graph);
    bool connected = bfs.connected(node1, node2);
    
    std::cout << "\n>>> Результат:" << std::endl;
    if (connected) {
        std::cout << "✅ Вершины " << v1 << " и " << v2 << " СВЯЗАНЫ" << std::endl;
        std::cout << "   (существует путь между ними)" << std::endl;
    } else {
        std::cout << "❌ Вершины " << v1 << " и " << v2 << " НЕ СВЯЗАНЫ" << std::endl;
        std::cout << "   (пути не существует)" << std::endl;
    }
    
    std::cout << "\n>>> Задача 5 выполнена!" << std::endl;
}
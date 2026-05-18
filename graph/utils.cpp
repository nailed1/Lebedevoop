#include "utils.h"
#include <fstream>
#include <iostream>

void printGraphInfo(const Graph& graph) {
    std::cout << "\n Информация о графе\n";
    std::cout << "Количество вершин: " << graph.size() << "\n";

    int edgeCount = 0;
    for (Node* node : graph) {
        for (auto it = node->nb_begin(); it != node->nb_end(); ++it)
            ++edgeCount;
    }

    std::cout << "Количество рёбер: " << edgeCount / 2 << "\n";
}

void printComponentsInfo(const std::vector<std::set<Node*>>& components) {
    std::cout << "\n Найдено компонент связности: " << components.size() << "\n";

    for (size_t i = 0; i < components.size(); ++i) {
        std::cout << "\nКомпонента " << (i + 1) << ":\n";
        std::cout << "  Размер: " << components[i].size() << " вершин\n";
        std::cout << "  Вершины: ";

        int count = 0;
        for (Node* node : components[i]) {
            if (count > 0) std::cout << ", ";
            std::cout << node->getName();
            if (++count >= 10) { std::cout << "..."; break; }
        }
        std::cout << "\n";
    }
}

void saveComponentToFile(const std::set<Node*>& component,
                         const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Ошибка: не удалось создать файл " << filename << "\n";
        return;
    }

    file << "Source\tTarget\n";


    for (Node* node : component) {
        for (auto it = node->nb_begin(); it != node->nb_end(); ++it) {
            Node* neighbour = it->first;
            // Записываем каждое ребро один раз (меньшее имя первым)
            if (component.count(neighbour) && node->getName() < neighbour->getName())
                file << node->getName() << "\t" << neighbour->getName() << "\n";
        }
    }

    std::cout << "  Компонента сохранена в файл: " << filename << "\n";
}

void saveAllComponents(const std::vector<std::set<Node*>>& components,
                       const std::string& baseFilename) {
    for (size_t i = 0; i < components.size(); ++i) {
        saveComponentToFile(components[i],
                            baseFilename + "_component_" + std::to_string(i + 1) + ".txt");
    }
}

// === Перегрузки для Graph* ===

void printComponentsInfo(const std::vector<Graph*>& components) {
    std::cout << "\n Найдено компонент связности: " << components.size() << "\n";

    for (size_t i = 0; i < components.size(); ++i) {
        const Graph* g = components[i];
        std::cout << "\nКомпонента " << (i + 1) << " (Graph):\n";
        std::cout << "  Вершин: " << g->size() << "\n";
        std::cout << "  Вершины: ";

        int count = 0;
        for (auto it = g->begin(); it != g->end(); ++it) {
            if (count > 0) std::cout << ", ";
            std::cout << (*it)->getName();
            if (++count >= 10) { std::cout << "..."; break; }
        }
        std::cout << "\n";
    }
}

void saveComponentToFile(const Graph& component, const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Ошибка: не удалось создать файл " << filename << "\n";
        return;
    }

    file << "Source\tTarget\n";

    for (auto it = component.begin(); it != component.end(); ++it) {
        Node* node = *it;
        for (auto nb = node->nb_begin(); nb != node->nb_end(); ++nb) {
            Node* neighbour = nb->first;
            if (node->getName() < neighbour->getName())
                file << node->getName() << "\t" << neighbour->getName() << "\n";
        }
    }

    std::cout << "  Компонента сохранена в файл: " << filename << "\n";
}

void saveAllComponents(const std::vector<Graph*>& components,
                       const std::string& baseFilename) {
    for (size_t i = 0; i < components.size(); ++i) {
        saveComponentToFile(*components[i],
                            baseFilename + "_component_" + std::to_string(i + 1) + ".txt");
    }
}

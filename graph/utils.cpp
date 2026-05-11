#include "utils.h"
#include <fstream>
#include <iostream>
#include <iomanip>

void saveComponentToFile(const std::set<Node*>& component, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка: не удалось создать файл " << filename << std::endl;
        return;
    }
    
    file << "Source\tTarget" << std::endl;
    
    // Для каждого узла в компоненте записываем его рёбра
    for (Node* node : component) {
        for (neighbour_iterator it = node->nb_begin(); it != node->nb_end(); ++it) {
            Node* neighbor = it->first;
            // Записываем только если сосед тоже в этой компоненте и чтобы не дублировать
            if (component.find(neighbor) != component.end() && 
                node->getNameAsInt() < neighbor->getNameAsInt()) {
                file << node->getName() << "\t" << neighbor->getName() << std::endl;
            }
        }
    }
    
    file.close();
    std::cout << "  Компонента сохранена в файл: " << filename << std::endl;
}

void saveAllComponents(const std::vector<std::set<Node*>>& components, const std::string& baseFilename) {
    for (size_t i = 0; i < components.size(); ++i) {
        std::string filename = baseFilename + "_component_" + std::to_string(i + 1) + ".txt";
        saveComponentToFile(components[i], filename);
    }
}

void printGraphInfo(const Graph& graph) {
    std::cout << "\n Информация о графе " << std::endl;
    std::cout << "Количество вершин: " << graph.size() << std::endl;
    
    int edgeCount = 0;
    for (node_iterator it = graph.begin(); it != graph.end(); ++it) {
        Node* node = *it;
        int count = 0;
        for (neighbour_iterator nit = node->nb_begin(); nit != node->nb_end(); ++nit) {
            count++;
        }
        edgeCount += count;
    }
    edgeCount /= 2;  // Каждое ребро посчитано дважды
    
    std::cout << "Количество рёбер: " << edgeCount << std::endl;
}

void printComponentsInfo(const std::vector<std::set<Node*>>& components) {
    std::cout << "\n Найдено компонент связности: " << components.size() << " " << std::endl;
    
    for (size_t i = 0; i < components.size(); ++i) {
        std::cout << "\nКомпонента " << (i + 1) << ":" << std::endl;
        std::cout << "  Размер: " << components[i].size() << " вершин" << std::endl;
        std::cout << "  Вершины: ";
        
        int count = 0;
        for (Node* node : components[i]) {
            if (count > 0) std::cout << ", ";
            std::cout << node->getName();
            count++;
            if (count >= 10) {  // Показываем только первые 10
                std::cout << "...";
                break;
            }
        }
        std::cout << std::endl;
    }
}
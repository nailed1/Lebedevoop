#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "Maze.h"
#include "MazeSolver.h"
#include <iostream>

namespace {
    const int GREEN_G_MIN = 170, GREEN_R_MAX = 50, GREEN_B_MAX = 80;
    const int RED_R_MIN   = 200, RED_G_MAX   = 50, RED_B_MAX   = 50;
    const int WALL_MAX    = 50;
}

int main() {
    int w, h, channels;
    unsigned char* pixels = stbi_load("Maze.bmp", &w, &h, &channels, 3);
    if (!pixels) {
        std::cerr << "Ошибка: не удалось открыть Maze.bmp\n";
        return 1;
    }

    Maze maze(w, h);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            const int idx = (y * w + x) * 3; // 1 пиксель = 3 байта RGB
            const unsigned char r = pixels[idx];
            const unsigned char g = pixels[idx + 1];
            const unsigned char b = pixels[idx + 2];

            if      (g > GREEN_G_MIN && r < GREEN_R_MAX && b < GREEN_B_MAX)
                maze.setCell(x, y, Maze::START);
            else if (r > RED_R_MIN && g < RED_G_MAX && b < RED_B_MAX)
                maze.setCell(x, y, Maze::END);
            else if (r < WALL_MAX && g < WALL_MAX && b < WALL_MAX)
                maze.setCell(x, y, Maze::WALL);
            else
                maze.setCell(x, y, Maze::EMPTY);
        }
    }

    if (!maze.hasStart() || !maze.hasEnd()) {
        std::cerr << "Ошибка: не найдена стартовая или финишная точка\n";
        stbi_image_free(pixels);
        return 1;
    }

    MazeSolver solver(maze);
    Path path = solver.shortestPath();

    if (path.found()) {
        // Рисуем путь синим, пропуская старт и финиш (сохраняем их цвета)
        for (size_t i = 1; i + 1 < path.cells.size(); ++i) {
            const Cell& c = path.cells[i];
            const int idx = (c.y * w + c.x) * 3;
            pixels[idx]     = 0;
            pixels[idx + 1] = 0;
            pixels[idx + 2] = 255;
        }
        stbi_write_bmp("resolved_maze.bmp", w, h, 3, pixels);
    } else {
        std::cerr << "Путь не найден\n";
    }

    stbi_image_free(pixels);
    return 0;
}

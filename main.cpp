#include "VariableGridMap.hpp"
#include <iostream>

void test1D() {
    std::cout << "\n--- 1D Grid Test ---\n";
    std::vector<float> origin = {0.0f};
    std::vector<std::tuple<float, float, float>> regionDefs = {
        {0.0f, 3.0f, 0.2f},
        {3.0f, 6.0f, 0.5f}
    };

    VariableGridMap gridMap(origin, regionDefs, 6.0f, 0.0f, 0.0f);

    std::vector<float> pos = {1.0f};
    gridMap.setValueAtCell(pos, 7.0f);

    auto node = gridMap.getGridCell(pos);
    if (node) {
        std::cout << "Node at (" << node->position[0] << ") with value: " << node->value << "\n";
    }

    gridMap.saveCSV("grid_output_1d.csv");
    std::cout << "Saved 1D grid to grid_output_1d.csv\n";
}

void test2D() {
    std::cout << "\n--- 2D Grid Test ---\n";
    std::vector<float> origin = {0.0f, 0.0f};
    std::vector<std::tuple<float, float, float>> regionDefs = {
        {0.0f, 5.0f, 0.2f},
        {5.0f, 10.0f, 0.5f}
    };

    VariableGridMap gridMap(origin, regionDefs, 10.0f, 10.0f, 0.0f);

    std::vector<float> pos1 = {0.5f, 0.5f};
    gridMap.setValueAtCell(pos1, 10.0f);

    std::vector<float> pos2 = {-1.0f, 0.2f};
    gridMap.setValueAtCell(pos2, 5.0f);

    auto node = gridMap.getGridCell(pos1);
    if (node) {
        std::cout << "Node at (" << node->position[0] << ", " << node->position[1] << ") with value: " << node->value << "\n";
        auto neighbors = gridMap.getNeighbors(node);
        std::cout << "Neighbors:\n";
        for (const auto& n : neighbors) {
            std::cout << " - (" << n->position[0] << ", " << n->position[1] << "), value: " << n->value << "\n";
        }
    }

    gridMap.saveCSV("grid_output_2d.csv");
    std::cout << "Saved 2D grid to grid_output_2d.csv\n";
}

void test3D() {
    std::cout << "\n--- 3D Grid Test ---\n";
    std::vector<float> origin = {0.0f, 0.0f, 0.0f};
    std::vector<std::tuple<float, float, float>> regionDefs = {
        {0.0f, 4.0f, 0.3f},
        {4.0f, 8.0f, 0.7f}
    };

    VariableGridMap gridMap(origin, regionDefs, 8.0f, 8.0f, 8.0f);

    std::vector<float> pos = {0.4f, -0.4f, 0.6f};
    gridMap.setValueAtCell(pos, 9.5f);

    auto node = gridMap.getGridCell(pos);
    if (node) {
        std::cout << "Node at (";
        for (size_t i = 0; i < node->position.size(); ++i) {
            std::cout << node->position[i];
            if (i < node->position.size() - 1) std::cout << ", ";
        }
        std::cout << ") with value: " << node->value << "\n";

        auto neighbors = gridMap.getNeighbors(node);
        std::cout << "Neighbors:\n";
        for (const auto& n : neighbors) {
            std::cout << " - (";
            for (size_t i = 0; i < n->position.size(); ++i) {
                std::cout << n->position[i];
                if (i < n->position.size() - 1) std::cout << ", ";
            }
            std::cout << "), value: " << n->value << "\n";
        }
    }

    gridMap.saveCSV("grid_output_3d.csv");
    std::cout << "Saved 3D grid to grid_output_3d.csv\n";
}

int main() {
    test1D();
    test2D();
    test3D();
    return 0;
}

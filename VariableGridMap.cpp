#include "VariableGridMap.hpp"
#include <cmath>
#include <fstream>
#include <sstream>
#include <numeric>
#include <functional>
#include <iostream>

VariableGridMap::VariableGridMap(std::vector<float> origin,
    std::vector<std::tuple<float, float, float>> regionDefs,
    float grid_size_x, float grid_size_y, float grid_size_z)
    : regionDefs_(std::move(regionDefs)), origin_(std::move(origin)),
      grid_size_x_(grid_size_x), grid_size_y_(grid_size_y), grid_size_z_(grid_size_z) {

    float res = getResolutionForPos(origin_);
    std::vector<float> snapped(origin_.size());
    for (size_t i = 0; i < origin_.size(); ++i)
        snapped[i] = std::round(origin_[i] / res) * res;

    root_ = std::make_shared<GridNode>(snapped, res);
    std::string key = posToKey(snapped, res);
    grid_[key] = root_;
    generateGridAround(snapped);
}

void VariableGridMap::updateRobotPosition(const std::vector<float>& new_center) {
    origin_ = new_center;
    grid_.clear();

    float res = getResolutionForPos(new_center);
    std::vector<float> snapped(new_center.size());
    for (size_t i = 0; i < new_center.size(); ++i)
        snapped[i] = std::round(new_center[i] / res) * res;

    root_ = std::make_shared<GridNode>(snapped, res);
    std::string key = posToKey(snapped, res);
    grid_[key] = root_;
    generateGridAround(snapped);
}

void VariableGridMap::generateGridAround(const std::vector<float>& center) {
    // Ranges for N-dimensions
    std::vector<float> ranges(center.size(), 1.0f);  // Default for >3D
    if (center.size() > 0) ranges[0] = grid_size_x_;
    if (center.size() > 1) ranges[1] = grid_size_y_;
    if (center.size() > 2) ranges[2] = grid_size_z_;

    std::function<void(std::vector<float>&, int)> recurse;
    recurse = [&](std::vector<float>& current, int dim) {
        if (dim == center.size()) {
            float res = getResolutionForPos(current);
            if (res <= 0.0f) return;

            std::vector<float> snapped(current.size());
            for (size_t i = 0; i < current.size(); ++i)
                snapped[i] = std::round(current[i] / res) * res;

            std::string key = posToKey(snapped, res);
            if (grid_.count(key) == 0) {
                grid_[key] = std::make_shared<GridNode>(snapped, res, 0.0f);
            }
            return;
        }

        // Use fine step (e.g. 0.1f) in real-world coordinates
        float min_val = center[dim] - ranges[dim];
        float max_val = center[dim] + ranges[dim];
        for (float val = min_val; val <= max_val; val += 0.1f) {
            current[dim] = val;
            recurse(current, dim + 1);
        }
    };

    std::vector<float> current(center.size(), 0.0f);
    recurse(current, 0);
}



float VariableGridMap::getResolutionForPos(const std::vector<float>& pos) const {
    float dist = 0.0f;
    for (size_t i = 0; i < pos.size(); ++i) {
        float delta = pos[i] - origin_[i];
        dist += delta * delta;
    }
    dist = std::sqrt(dist);

    for (const auto& [minD, maxD, res] : regionDefs_) {
        if (dist >= minD && dist <= maxD) return res;
    }
    return -1.0f;
}
std::string VariableGridMap::posToKey(const std::vector<float>& pos, float resolution) const {
    std::ostringstream oss;
    for (float p : pos)
        oss << std::round(p / resolution) << "_";
    return oss.str();
}

std::shared_ptr<GridNode> VariableGridMap::getGridCell(const std::vector<float>& pos) {
    float res = getResolutionForPos(pos);
    if (res < 0) return nullptr;
    std::vector<float> snapped(pos.size());
    for (size_t i = 0; i < pos.size(); ++i)
        snapped[i] = std::round(pos[i] / res) * res;

    std::string key = posToKey(snapped, res);
    if (grid_.count(key) == 0) {
        grid_[key] = std::make_shared<GridNode>(snapped, res);
    }
    return grid_[key];
}

bool VariableGridMap::setValueAtCell(const std::vector<float>& pos, float val) {
    auto node = getGridCell(pos);
    if (!node) return false;
    node->value = val;
    return true;
}

std::vector<std::shared_ptr<GridNode>> VariableGridMap::getNeighbors(const std::shared_ptr<GridNode>& node) {
    std::vector<std::shared_ptr<GridNode>> neighbors;
    auto offsets = generateOffsets(node->position.size());

    for (const auto& off : offsets) {
        std::vector<float> newPos(node->position.size());
        for (size_t i = 0; i < newPos.size(); ++i)
            newPos[i] = node->position[i] + off[i] * node->resolution;

        auto neighbor = getGridCell(newPos);
        if (neighbor) neighbors.push_back(neighbor);
    }

    return neighbors;
}

std::vector<std::vector<int>> VariableGridMap::generateOffsets(int dim) const {
    std::vector<std::vector<int>> result;
    std::function<void(std::vector<int>&, int)> dfs = [&](std::vector<int>& curr, int d) {
        if (d == dim) {
            if (std::any_of(curr.begin(), curr.end(), [](int x) { return x != 0; }))
                result.push_back(curr);
            return;
        }
        for (int i = -1; i <= 1; ++i) {
            curr[d] = i;
            dfs(curr, d + 1);
        }
    };
    std::vector<int> current(dim, 0);
    dfs(current, 0);
    return result;
}

void VariableGridMap::saveCSV(const std::string& filename) {
    std::ofstream file(filename);

    // Header
    if (!grid_.empty()) {
        const auto& any_node = grid_.begin()->second;
        for (size_t i = 0; i < any_node->position.size(); ++i) {
            file << "dim" << i;
            if (i < any_node->position.size() - 1)
                file << ",";
        }
        file << ",resolution,value\n";
    }

    // Rows
    for (const auto& [key, node] : grid_) {
        for (size_t i = 0; i < node->position.size(); ++i) {
            file << node->position[i];
            if (i < node->position.size() - 1)
                file << ",";
        }
        file << "," << node->resolution << "," << node->value << "\n";
    }
}

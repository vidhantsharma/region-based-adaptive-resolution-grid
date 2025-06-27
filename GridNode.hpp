#pragma once
#include <memory>
#include <vector>

struct GridNode {
    std::vector<float> position;
    float resolution;
    float value;

    std::weak_ptr<GridNode> parent;
    std::vector<std::shared_ptr<GridNode>> children;

    GridNode(std::vector<float> pos, float res, float val = 0.0f)
        : position(std::move(pos)), resolution(res), value(val) {}
};

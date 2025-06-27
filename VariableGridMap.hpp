#pragma once

#include "GridNode.hpp"
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

class VariableGridMap {
public:
    VariableGridMap(std::vector<float> origin,
                    std::vector<std::tuple<float, float, float>> regionDefs,
                    float grid_size_x, float grid_size_y, float grid_size_z);

    void updateRobotPosition(const std::vector<float>& new_center);
    void generateGridAround(const std::vector<float>& center);
    std::shared_ptr<GridNode> getGridCell(const std::vector<float>& pos);
    bool setValueAtCell(const std::vector<float>& pos, float val);
    std::vector<std::shared_ptr<GridNode>> getNeighbors(const std::shared_ptr<GridNode>& node);
    void saveCSV(const std::string& filename);

private:
    float getResolutionForPos(const std::vector<float>& pos) const;
    std::string posToKey(const std::vector<float>& pos, float resolution) const;
    std::vector<std::vector<int>> generateOffsets(int dim) const;

    std::unordered_map<std::string, std::shared_ptr<GridNode>> grid_;
    std::vector<std::tuple<float, float, float>> regionDefs_;
    std::vector<float> origin_;
    float grid_size_x_;
    float grid_size_y_;
    float grid_size_z_;
    std::shared_ptr<GridNode> root_;
};

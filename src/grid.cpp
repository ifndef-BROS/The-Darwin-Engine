#include "grid.h"
#include <stdexcept>

namespace sim {

Grid::Grid(unsigned int width, unsigned int height)
    : width_(width), height_(height) {
    if (width == 0 || height == 0) {
        throw std::invalid_argument("Grid dimensions must be positive.");
    }
    grid_.resize(height_);
    for (auto& row : grid_) {
        row.resize(width_);
    }
}

void Grid::clear() {
    for (unsigned int y = 0; y < height_; ++y) {
        for (unsigned int x = 0; x < width_; ++x) {
            grid_[y][x].clear();
        }
    }
}

void Grid::place(Indiv* indiv) {
    // Placeholder - not used in current RL training
}

const std::vector<Indiv*>& Grid::getContents(unsigned int x, unsigned int y) const {
    if (x >= width_ || y >= height_) {
        throw std::out_of_range("Grid coordinates are out of bounds.");
    }
    return grid_[y][x];
}

void Grid::wrapCoordinates(unsigned int& x, unsigned int& y) const {
    x = x % width_;
    y = y % height_;
}

} // namespace sim

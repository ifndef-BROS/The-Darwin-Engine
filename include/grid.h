#ifndef GRID_H
#define GRID_H

#include <vector>
#include "indiv.h"

namespace sim {

class Grid {
public:
    Grid(unsigned int width, unsigned int height);
    void clear();
    void place(Indiv* indiv);
    const std::vector<Indiv*>& getContents(unsigned int x, unsigned int y) const;
    unsigned int getWidth() const { return width_; }
    unsigned int getHeight() const { return height_; }
    void wrapCoordinates(unsigned int& x, unsigned int& y) const;

private:
    unsigned int width_;
    unsigned int height_;
    std::vector<std::vector<std::vector<Indiv*>>> grid_;
};

} // namespace sim

#endif // GRID_H

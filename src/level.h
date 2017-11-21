#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include <string>
#include <vector>
#include <utility>

namespace world {

// Encapsulates a world
class Level {
public:
    // Load the level from the JSON file given by `filename'.
    Level(std::string filename);

    // Get the size of the level
    auto size() const { return std::make_pair(width, depth); }

    // Get the height at a point
    float altitude(float x, float y);

private:
    unsigned width;
    unsigned depth;
    std::vector<double> heightmap;
};

}

#endif

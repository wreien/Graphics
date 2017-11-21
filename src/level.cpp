#include "level.h"
#include <json/json.h>

#include <fstream>
#include <algorithm>
#include <iterator>

#include <iostream>
#include <cstdlib>

namespace world {

Level::Level(std::string filename) {
    Json::Value root;
    (std::ifstream { filename }) >> root;

    width = root.get("width", 0).asUInt();
    depth = root.get("depth", 0).asUInt();

    if (width < 5 || depth < 5) {
        std::cerr << "Invalid size for " << filename << ": "
                  << width << ", " << depth << std::endl;
        std::exit(1);
    }

    Json::Value alts = root["altitude"];
    heightmap.reserve(width * depth);
    std::transform(std::begin(alts), std::end(alts), std::back_inserter(heightmap),
        [](auto node) { return node.asFloat(); });

    if (heightmap.size() != width * depth) {
        std::cerr << "Invalid altitude data given for " << filename << std::endl;
        std::exit(1);
    }
}

float Level::altitude(float x, float z) {
    return heightmap[static_cast<unsigned>(x) * depth + static_cast<unsigned>(z)];
}

}

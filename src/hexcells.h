#pragma once

#include <nlohmann/json.hpp>

#include "level.h"

namespace hexcells {
using coord_t = hex::Hex;
using state_t = bool;

struct level;

struct level *level1();
} // namespace hexcells


#pragma once

#include <memory>
#include <nlohmann/json.hpp>

#include "level.h"

namespace hexcells {
using coord_t = hex::Hex;
using state_t = bool;

struct level;

struct level1 : ::level<coord_t, state_t> {
  level1(::level<coord_t, state_t>::data data)
      : ::level<coord_t, state_t>(data){};
};

std::shared_ptr<::level<coord_t, state_t>> make_level(unsigned level_num);

} // namespace hexcells

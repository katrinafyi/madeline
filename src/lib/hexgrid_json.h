#pragma once

#include <nlohmann/json.hpp>

#include "hexgrid.h"

NLOHMANN_JSON_NAMESPACE_BEGIN
template <> struct adl_serializer<hex::Hex> {
  static void to_json(nlohmann::json &j, const hex::Hex &value) {
    j = nlohmann::json::array({value.q, value.r, value.s});
  }

  static void from_json(const nlohmann::json &j, hex::Hex &value) {
    assert(j.size() == 3);
    value = hex::Hex{(int)j[0], (int)j[1], (int)j[2]};
  }
};
NLOHMANN_JSON_NAMESPACE_END


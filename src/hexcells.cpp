#include <iostream>
#include <nlohmann/json.hpp>

#include "lib/hexgrid.h"
#include "level.h"

namespace hexcells {

using coord_t = hex::Hex;
using state_t = bool;

::level<coord_t, state_t>::data builder1() {
  coord_t pos{0, 0, 0};
  std::map<coord_t, state_t> coords{{pos, false}};
  std::map<coord_t, unsigned> pending;
  std::set<::level<coord_t, state_t>::fact> facts;

  // clang-format off
  auto n  = [&]() { pos = hex::hex_add(pos, {0, -1, 1}); };
  auto ne = [&]() { pos = hex::hex_add(pos, {1, -1, 0}); };
  auto se = [&]() { pos = hex::hex_add(pos, {1, 0, -1}); };
  auto s  = [&]() { pos = hex::hex_add(pos, {0, 1, -1}); };
  auto sw = [&]() { pos = hex::hex_add(pos, {-1, 1, 0}); };
  auto nw = [&]() { pos = hex::hex_add(pos, {-1, 0, 1}); };
  // clang-format on

  auto num = [&](unsigned n) {
    assert(!coords.contains(pos));
    coords[pos] = false;
    pending[pos] = n;
  };
  auto mine = [&]() {
    assert(!coords.contains(pos));
    coords[pos] = true;
  };

  if (1) {
    num(0);
    n();
    num(0);
    n();
    num(1);
    n();
    mine();
    se();
    num(1);
    s();
    num(1);
    s();
    num(1);
    ne();
    mine();
    n();
    num(1);
    se();
    num(1);
    se();
    num(0);
    ne();
    num(1);
    ne();
    mine();
    s();
    num(1);
    se();
    mine();
    n();
    num(2);
    n();
    num(2);
    se();
    mine();
    s();
    num(1);
    ne();
    num(1);
    ne();
    num(0);
    se();
    num(1);
    se();
    num(2);
    n();
    mine();
    ne();
    num(2);
    s();
    mine();
    num(1);
    se();
    num(0);
    n();
    num(1);
    n();
    num(1);
    n();
    num(0);
  }

  for (const auto &[c, count] : pending) {
    std::set<coord_t> cs{};
    for (int i = 0; i < 6; i++) {
      coord_t other = hex::hex_neighbor(c, i);
      if (coords.contains(other)) {
        cs.insert(other);
      }
    }
    facts.insert({{c}, cs, "asdf"});
  }

  return {coords, facts};
}

struct level1 : ::level<coord_t, state_t> {

  void reset() override {}
};

struct level1 *level1() {

  ::level<coord_t, state_t>::data x = builder1();

  std::cout << nlohmann::json{x} << std::endl;
  // hexcells_level a{};
  // static_assert(!std::is_abstract_v<struct level1>);
  return nullptr;
}

}; // namespace hexcells

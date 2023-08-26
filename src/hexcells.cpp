#include <format>
#include <iostream>
#include <nlohmann/json.hpp>

#include "hexcells.h"
#include "level.h"
#include "lib/hexgrid.h"

namespace hexcells {

using coord_t = hex::Hex;
using state_t = bool;

::data<coord_t, state_t> builder1() {
  coord_t pos{0, 0, 0};
  std::map<coord_t, state_t> coords{{pos, false}};
  std::map<coord_t, unsigned> pending;
  std::set<fact<coord_t>> facts;
  std::set<coord_t> initials;

  // clang-format off
  auto n  = [&]() { pos = hex::hex_add(pos, {0, -1, 1}); };
  auto ne = [&]() { pos = hex::hex_add(pos, {1, -1, 0}); };
  auto se = [&]() { pos = hex::hex_add(pos, {1, 0, -1}); };
  auto s  = [&]() { pos = hex::hex_add(pos, {0, 1, -1}); };
  auto sw = [&]() { pos = hex::hex_add(pos, {-1, 1, 0}); };
  auto nw = [&]() { pos = hex::hex_add(pos, {-1, 0, 1}); };

  auto initial = [&]() { initials.insert(pos); };

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
    initial();
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
    initial();
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
    initial();
  }

  for (const auto &[c, count] : pending) {
    std::set<coord_t> cs{};
    for (int i = 0; i < 6; i++) {
      coord_t other = hex::hex_neighbor(c, i);
      if (coords.contains(other)) {
        cs.insert(other);
      }
    }

    facts.insert({{c}, cs, std::format("= {}", pending.at(c))});
  }

  return {coords, facts, initials};
}

std::shared_ptr<::level<coord_t, state_t>> make_level(unsigned level_num) {

  level1::data x = builder1();

  std::cout << nlohmann::json{x} << std::endl;
  // hexcells_level a{};
  static_assert(!std::is_abstract_v<struct level1>);
  return std::make_shared<struct level1>(x);
}

}; // namespace hexcells

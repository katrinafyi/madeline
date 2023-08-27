#pragma once
#include <concepts>
#include <map>
#include <set>
#include <string>
#include <utility>

#include <nlohmann/json.hpp>

#include "imgui.h"
#include "lib/json.h"
#include <z3++.h>

template <typename K, typename V>
constexpr std::set<K> keys_of_map(const std::map<K, V> &data) {
  std::set<K> cs;
  for (const auto &x : data)
    cs.insert(x.first);
  return cs;
}

enum struct cmp { LT = -1, EQ = 0, GT = +1 };

template <typename C> struct fact {
  std::set<C> hiders; // the center cell

  std::set<C> coords; // participiating cells (adjacent)
  enum cmp cmp;
  unsigned rhs;

  // fact(const fact &) = delete;
  // fact &operator=(const fact &) = delete;

  z3::expr encode(z3::solver s, z3::context c) {}

  friend std::strong_ordering operator<=>(const fact &l,
                                          const fact &r) = default;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(fact, hiders, coords, cmp, rhs);
};

template <typename C, typename S> struct data {
  std::map<C, S> coords;
  std::set<struct fact<C>> facts;
  std::set<C> initial;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(data, coords, facts, initial);
};

template <typename C, typename S>
  requires std::copyable<S>
struct level {
public:
  using data = ::data<C, S>;
  using fact = ::fact<C>;

protected:
  const data _data;
  const std::set<C> _coords;
  std::map<C, bool> _revealed;

  constexpr std::map<C, bool> _states_init(const data &data) {}

public:
  int hint_number(const C coord) const {
    int n = 0;
    for (auto x : facts()) {
      if (x.hiders.contains(coord)) {
        for (auto y : x.coords) {
          if (_data.coords.at(y)) {
            n++;
          }
        }
      }
    }
    return n;
  }

  level(data data) : _data(data), _coords(keys_of_map(data.coords)) { reset(); }

  level(const level &) = delete;
  level &operator=(const level &) = delete;

  virtual void reset() {
    _revealed.clear();
    for (auto &x : _data.coords) {
      _revealed[x.first] = _data.initial.contains(x.first);
    }
  };

  virtual void update() {}

  virtual std::set<C> coords() const & { return _coords; }
  virtual S state(const C &c) const & { return _data.coords.at(c); }
  virtual bool solved(const C &c) const & { return _revealed.at(c); }

  virtual const std::set<fact> facts() const & { return _data.facts; }
  virtual const std::set<fact> facts_of_coord(const C &c) const & {

    return facts();
  }
  virtual bool is_known(const fact &fact) const & {
    for (const auto &h : fact.hiders) {
      if (!solved(h))
        return false;
    }
    return true;
  }

  // guess that the coordinate is a given state. returns true iff correct, and
  // refreshes the known facts.
  virtual bool guess(const C &c, const S s) {
    if (state(c) != s) {
      return false;
    }
    _revealed.at(c) = true;
    return true;
  }

  virtual ~level() = default;
};

template <typename Level, typename C, typename S>
  requires std::derived_from<Level, struct level<C, S>>
struct layout {
  // draws via imgui side-effects. assumed to run in imgui mode.
  virtual void draw(const C &c) = 0;
};

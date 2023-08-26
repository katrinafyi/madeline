#pragma once
#include <concepts>
#include <map>
#include <set>
#include <string>
#include <utility>

#include <nlohmann/json.hpp>

#include "imgui.h"
#include "lib/hexgrid_json.h"

template <typename C, typename S>
  requires std::copyable<S>
struct level {

public:
  struct fact {
    std::set<C> hiders;

    std::set<C> coords;
    std::string func; // smtlib s-expr head.

    // fact(const fact &) = delete;
    // fact &operator=(const fact &) = delete;

    friend std::strong_ordering operator<=>(const fact &l,
                                            const fact &r) = default;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(fact, hiders, coords, func);
  };

  struct data {
    std::map<C, S> coords;
    std::set<struct fact> facts;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(data, coords, facts);
  };

protected:
  const struct data datum;
  std::map<C, std::set<struct fact>> _facts_by_coord;

public:
  level(struct data data) : datum(data){};

  level(const level &) = delete;
  level &operator=(const level &) = delete;

  virtual void reset() = 0;
  virtual void update() {
    _facts_by_coord.clear();
    for (const auto &c : coords()) {
      _facts_by_coord[c] = {};
      for (const auto &f : facts()) {
        if (f.coords.contains(c)) {
          _facts_by_coord[c].insert(f);
        }
      }
    }
  }

  virtual std::set<const C> coords() const & = 0;
  virtual S state(const C &c) const & = 0;
  virtual bool solved(const C &c) = 0;

  virtual const std::set<const struct fact> facts() const & {
    return datum.facts;
  }
  virtual const std::set<const struct fact> facts_of_coord(const C &c) const & {
    return facts();
  }
  virtual bool is_known(const struct fact &fact) const & {
    for (const auto &h : fact.hiders) {
      if (!solved(h))
        return false;
    }
    return true;
  }

  // guess that the coordinate is a given state. returns true iff correct, and
  // refreshes the known facts.
  virtual bool guess(const C &c, const S state) {
    if (state(c) != state) {
      return false;
    }
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

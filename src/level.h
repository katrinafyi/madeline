#include <concepts>
#include <set>
#include <string>
#include <utility>

#include "imgui.h"

template <typename C, typename S>
  requires std::is_enum_v<S>
struct level {

protected:
  struct fact {
    std::set<C> coords;
    std::string func; // smtlib s-expr head.

    // fact(const fact &) = delete;
    // fact &operator=(const fact &) = delete;

    // friend std::strong_ordering operator<=>(const fact &l, const fact &r) {
    //   return &l <=> &r;
    // }
  };

public:
  level(){};

  level(const level &) = delete;
  level &operator=(const level &) = delete;

  virtual void init() & = 0;

  virtual std::set<const C> coords() const & = 0;
  virtual S state(const C &c) const & = 0;

  virtual const std::set<const struct fact> facts() const & = 0;
  virtual const std::set<const struct fact>
  facts_of_coord(const C &c) const & = 0;
  virtual bool is_known(const struct fact &fact) const & = 0;

  // guess that the coordinate is a given state. returns true iff correct, and
  // refreshes the known facts.
  virtual bool guess(const C &c, const S state) & = 0;

  virtual ~level() = default;
};

template <typename Level, typename C, typename S>
  requires std::derived_from<Level, struct level<C, S>>
struct layout {
  // draws via imgui side-effects. assumed to run in imgui mode.
  virtual void draw(const C &c) = 0;
};

#include <vector>

#include "ui.h"

namespace ui {

template <> const std::vector<bool> enum_values<bool>() {
  static std::vector<bool> vals{false, true};
  return vals;
}
template <> const char *enum_name(bool b) {
  return b ? "mine" : "clear";
}

template <> const std::vector<::cmp> enum_values<::cmp>() {
  static std::vector<::cmp> vals{::cmp::LT, ::cmp::EQ, ::cmp::GT};
  return vals;
}
template <> const char *enum_name(::cmp b) {
  switch (b) {
    case ::cmp::LT: return "<";
    case ::cmp::EQ: return "=";
    case ::cmp::GT: return ">";
  }
  assert(false);
  return "(invalid)";
}
} // namespace ui

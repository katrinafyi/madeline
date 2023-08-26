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
} // namespace ui

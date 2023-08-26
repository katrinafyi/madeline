#pragma once

#include <string>
#include <vector>

namespace ui {

template <typename T> const std::vector<T> enum_values();
template <typename T> const char *enum_name(const T t);

} // namespace ui

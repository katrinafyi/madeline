#pragma once

#include <format>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

#include "imgui.h"
#include "level.h"

namespace ui {

namespace im = ImGui;
using json = nlohmann::json;

template <typename T> const std::vector<T> enum_values();
template <typename T> const char *enum_name(const T t);

// forward decl.
template <typename C, typename S> struct coord_widget;
template <typename C, typename S> struct fact_widget;
template <typename C, typename S> struct proof_widget;

// main ui state.
template <typename C, typename S> struct state {
  // the initial coord of the current proof
  std::optional<C> selected{};
  // the currently hovered hexagon
  std::optional<C> hovered{};
  // a cell to highlight, due to hovering in the proof window.
  std::optional<C> highlight{};

  proof_widget<C, S> *active_prover = nullptr;
  std::map<C, proof_widget<C, S>> proofs{};

  std::shared_ptr<::level<C, S>> level_ptr;
};

template <typename C, typename S> struct coord_widget {
  state<C, S> &ui_state;

  const C coord;

  void render() {
    im::SmallButton(nlohmann::json(coord).dump().c_str());
    im::SameLine();
  }
};

template <typename C, typename S> struct fact_widget {
  state<C, S> &ui_state;

  const ::fact<C> fact;

  void render() {
    im::Text("fact revealed by");
    im::SameLine();

    for (auto &c : fact.hiders) {
      coord_widget{ui_state, c}.render();
    }

    // note: inequality is reversed
    im::Text(
        std::format(": {} {}", fact.rhs, enum_name((::cmp) - (int)fact.cmp))
            .c_str());
    im::SameLine();
    bool first = true;
    for (auto &c : fact.coords) {
      if (!first) {
        im::Text(",");
        im::SameLine();
      }
      first = false;
      coord_widget{ui_state, c}.render();
    }
    im::Text("");
  }
};

template <typename C, typename S> struct proof_widget {

  state<C, S> &ui_state;

  C focus;
  S goal;

  std::vector<::fact<C>> facts;

  void render() {
    im::Text("proof ");
    im::SameLine();
    im::SmallButton(nlohmann::json(focus).dump().c_str());
    im::SameLine();
    im::Text("=");
    im::SameLine();

    if (ImGui::BeginPopupContextItem("target_popup")) {
      for (S x : ui::enum_values<S>()) {
        if (ImGui::Selectable(ui::enum_name(x))) {
          goal = x;
        }
      }
      ImGui::EndPopup();
    }

    if (im::SmallButton(ui::enum_name(goal))) {
      ImGui::OpenPopup("target_popup");
    }

    im::Indent();

    im::Text("using");
    im::Indent();
    for (const auto &f : facts) {
      fact_widget{ui_state, f}.render();
    }

    im::Unindent();

    im::Unindent();

    im::Text("qed");
  }
};

} // namespace ui

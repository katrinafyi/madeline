#pragma once

#include <format>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>
#include <z3++.h>

#include "hexcells.h"
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
    if (im::IsItemHovered()) {
      ui_state.highlight = coord;
    } else if (ui_state.highlight == coord) {
      ui_state.highlight = std::nullopt;
    }
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

  bool check() {

    z3::context c;

    std::map<hexcells::coord_t, z3::expr> vals{};

    // add all things
    for (auto &hex : ui_state.level_ptr->coords()) {
      vals.insert({hex, c.int_const(hex.str().c_str())});
    }

    z3::expr concerning = vals.at(focus);

    z3::expr conjecture = concerning == goal;

    z3::solver s(c);

    std::cout << s.to_smt2();
    // add facts

    for (auto &[coord, expr] : vals) {
      if (ui_state.level_ptr->is_known(coord)) {
        s.add(expr == c.int_val(ui_state.level_ptr->solved(coord)));
      } else {
        s.add(0 <= expr && expr <= 1);
      }
    }

    for (auto &x : facts) {

      if (x.hiders.size() != 1 || x.coords.size() < 1) {
        std::cout << "BIG FAIL" << __LINE__ << "@" << __FILE_NAME__
                  << std::endl;
        continue;
      }

      z3::expr lhs = c.int_val(0);
      for (C i : x.coords) {
        lhs = lhs + vals.at(i);
      }

      switch (x.cmp) {
      case cmp::EQ:
        s.add(lhs == c.int_val(x.rhs));
        break;
      case cmp::LT:
        s.add(lhs < c.int_val(x.rhs));
        break;
      case cmp::GT:
        s.add(lhs > c.int_val(x.rhs));
        break;
      }
      std::cout << s.to_smt2();
    }
    std::cout << s.to_smt2();
    s.add(!conjecture);
    std::cout << s.to_smt2();

    switch (s.check()) {
    case z3::unsat:
      std::cout << "Proof is valid\n";
      return true;
    case z3::sat:
      std::cout << "Proof is not valid\n";
      return false;
    case z3::unknown:
      std::cout << "unknown\n";
      return false;
    }
  }

  void render() {
    im::Text("proof ");
    im::SameLine();
    coord_widget{ui_state, focus}.render();
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
      goal = !goal;
      // ImGui::OpenPopup("target_popup");
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
      ImGui::SetTooltip("toggle the goal's state.");

    im::Indent();

    im::Text("using");
    im::Indent();
    for (const auto &f : facts) {
      fact_widget{ui_state, f}.render();
    }

    im::Unindent();

    im::Unindent();

    if (im::SmallButton("qed?")) {
      if (check() && ui_state.level_ptr->guess(focus, goal)) {
        std::cout << "correct!" << std::endl;
        ui_state.active_prover = nullptr;
      } else {
        std::cout << "wrong!" << std::endl;
      }
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
      ImGui::SetTooltip("attempt the proof, good luck.");
    }
  }
};

} // namespace ui

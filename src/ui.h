#pragma once

#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <sstream>
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
  // in reveal-all mode? (for debugging)
  bool reveal;
  // the initial coord of the current proof
  std::optional<C> selected{};
  // the currently hovered hexagon
  std::optional<C> hovered{};
  // a cell to highlight, due to hovering in the proof window.
  std::optional<C> highlight{};

  std::string prover_output;
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

  const ::fact<C> &fact;

  void render() {
    im::Text("fact revealed by");
    im::SameLine();

    for (auto &c : fact.hiders) {
      coord_widget{ui_state, c}.render();
    }
    std::stringstream s{};

    s << ": " << fact.rhs << " " << enum_name((::cmp) - (int)fact.cmp);

    // note: inequality is reversed
    im::Text(s.str().c_str());
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

  std::vector<std::reference_wrapper<const ::fact<C>>> facts;

  bool check(std::ostream &out) {
    z3::context c;

    std::map<hexcells::coord_t, z3::expr> vals{};

    // add all things
    for (auto &hex : ui_state.level_ptr->coords()) {
      vals.insert({hex, c.int_const(hex.str().c_str())});
    }

    z3::expr concerning = vals.at(focus);

    z3::expr conjecture = concerning == goal;

    z3::solver s(c);

    // std::cout << s.to_smt2();
    // add facts

    for (auto &[coord, expr] : vals) {
      if (ui_state.level_ptr->solved(coord)) {
        s.add(expr == c.int_val(ui_state.level_ptr->state(coord)));
      } else {
        s.add(0 <= expr && expr <= 1);
      }
    }

    for (auto &r : facts) {
      auto& x = r.get();

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
      // auto str = s.to_smt2();
      // std::cout << str;
    }
    // std::cout << s.to_smt2();
    s.add(!conjecture);
    out << s.to_smt2();
    // ui_state.prover_output.append(s.to_smt2());

    switch (s.check()) {
    case z3::unsat:
      out << "Proof is valid\n";
      return true;
    case z3::sat:
      out << "Proof is not valid\n";
      return false;
    case z3::unknown:
      out << "Unknown\n";
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

    if (im::SmallButton(ui::enum_name(goal)) ||
        im::IsKeyReleased(ImGuiMod_Shift)) {
      goal = !goal;
      // ImGui::OpenPopup("target_popup");
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
      ImGui::SetTooltip("toggle the goal's state.");

    im::Indent();

    im::Text("using");
    im::Indent();
    for (const auto &f : facts) {
      fact_widget{ui_state, f.get()}.render();
    }

    im::Unindent();

    im::Unindent();

    if (im::SmallButton("qed?") || im::IsKeyReleased(ImGuiKey_Space)) {
      std::stringstream ss{};
      bool smt_result = check(ss);
      ui_state.prover_output = std::move(ss).str();
      if (smt_result && ui_state.level_ptr->guess(focus, goal)) {
        std::cout << "correct!" << std::endl;
        ui_state.active_prover = nullptr;
      } else {
        std::cout << "wrong!" << std::endl;
      }
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
      ImGui::SetTooltip("attempt the proof, good luck. (hotkey: space)");
    }
  }
};

} // namespace ui

#pragma once

#include "imgui.h"
#include <format>
#include <nlohmann/json.hpp>

#include "level.h"
#include "ui.h"

namespace prover {

namespace im = ImGui;

template <typename C, typename S> struct coord_widget {
  const std::shared_ptr<::level<C, S>> level_ptr;

  const C coord;

  void render() {
    im::SmallButton(nlohmann::json(coord).dump().c_str());
    im::SameLine();
  }
};

template <typename C, typename S> struct fact_widget {
  const std::shared_ptr<::level<C, S>> level_ptr;

  const ::fact<C> fact;

  void render() {
    im::Text("fact revealed by");
    im::SameLine();
    coord_widget{level_ptr}.render();
    im::Text(std::format(": ({}", fact.func).c_str());
    im::SameLine();
    bool first = true;
    for (auto &c : fact.coords) {
      if (!first) {
        im::Text(",");
        im::SameLine();
      }
      first = false;
      coord_widget{level_ptr, c}.render();
    }
    im::Text(")");
  }
};

template <typename C, typename S> struct proof_widget {

  const std::shared_ptr<::level<C, S>> level_ptr;

  const C focus;
  S goal;

  const std::vector<::fact<C>> facts;

  void render() {

    im::Text("asd");

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
      fact_widget{level_ptr, f}.render();
    }

    im::Unindent();

    im::Unindent();

    im::Text("qed");
  }
};

} // namespace prover

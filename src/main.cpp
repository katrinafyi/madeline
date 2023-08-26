#include "hello_imgui/hello_imgui.h"

#include "hexcells.h"
#include "imgui.h"
#include "imgui_theme.h"
#include "level.h"
#include "lib/hexgrid.h"
#include "prover.h"
#include <memory>

namespace im = ImGui;

static std::shared_ptr<::level<hexcells::coord_t, hexcells::state_t>> level_ptr;

struct GUI {
  static void gui() {
    static ImVector<ImVec2> points;
    static ImVec2 scrolling(50.0f, 250.0f);
    static bool opt_enable_grid = true;
    static bool opt_enable_context_menu = true;
    constexpr auto CANVAS_BG_COL = IM_COL32(50, 50, 50, 255);
    constexpr auto HEX_REVEALED_COL = IM_COL32(200, 50, 50, 255);
    constexpr auto HEX_HIDDEN_COL = IM_COL32(100, 50, 50, 255);
    constexpr auto HEX_HIDDEN_HOVERED_COL = IM_COL32(90, 40, 40, 255);

    ImGui::Checkbox("Enable grid", &opt_enable_grid);
    ImGui::Checkbox("Enable context menu", &opt_enable_context_menu);
    ImGui::Text("Mouse Left: drag to add lines,\nMouse Right: drag to scroll, "
                "click for context menu.");

    // Typically you would use a BeginChild()/EndChild() pair to benefit from
    // a clipping region + own scrolling. Here we demonstrate that this can be
    // replaced by simple offsetting + custom drawing +
    // PushClipRect/PopClipRect() calls. To use a child window instead we
    // could use, e.g:
    //      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); //
    //      Disable padding ImGui::PushStyleColor(ImGuiCol_ChildBg,
    //      IM_COL32(50, 50, 50, 255));  // Set a background color
    //      ImGui::BeginChild("canvas", ImVec2(0.0f, 0.0f), true,
    //      ImGuiWindowFlags_NoMove); ImGui::PopStyleColor();
    //      ImGui::PopStyleVar();
    //      [...]
    //      ImGui::EndChild();

    // Using InvisibleButton() as a convenience 1) it will advance the layout
    // cursor and 2) allows us to use IsItemHovered()/IsItemActive()
    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos(); // ImDrawList API uses
                                                    // screen coordinates!
    ImVec2 canvas_sz =
        ImGui::GetContentRegionAvail(); // Resize canvas to what's available
    if (canvas_sz.x < 50.0f)
      canvas_sz.x = 50.0f;
    if (canvas_sz.y < 50.0f)
      canvas_sz.y = 50.0f;
    ImVec2 canvas_p1 =
        ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

    // Draw border and background color
    ImGuiIO &io = ImGui::GetIO();
    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    draw_list->AddRectFilled(canvas_p0, canvas_p1, CANVAS_BG_COL);
    draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

    // This will catch our interactions
    ImGui::InvisibleButton("canvas", canvas_sz,
                           ImGuiButtonFlags_MouseButtonLeft |
                               ImGuiButtonFlags_MouseButtonRight);
    const bool is_hovered = ImGui::IsItemHovered(); // Hovered
    const bool is_active = ImGui::IsItemActive();   // Held
    const ImVec2 origin(canvas_p0.x + scrolling.x,
                        canvas_p0.y + scrolling.y); // Lock scrolled origin
    const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x,
                                     io.MousePos.y - origin.y);

    hex::Layout layout{
        hex::layout_flat,
        hex::Point(25, 25),
        hex::Point(origin.x, origin.y),
    };

    ImVec2 cursor = ImGui::GetMousePos();
    auto selected = hex::cube_round(
        hex::pixel_to_hex(layout, hex::Point(cursor.x, cursor.y)));

    draw_list->PushClipRect(canvas_p0, canvas_p1, true);
    bool any_selected = false;
    for (auto &x : level_ptr->coords()) {
      hex::Point pixels = hex::hex_to_pixel(layout, x);
      auto solved = level_ptr->solved(x);

      auto col = HEX_HIDDEN_COL;
      if (x == selected) {
        col = HEX_HIDDEN_HOVERED_COL;
        any_selected = true;
      }
      if (solved) {
        col = HEX_REVEALED_COL;
      }

      draw_list->AddNgonFilled(ImVec2(pixels.x, pixels.y), 20, col, 6);
      if (solved) {
        int number = level_ptr->hint_number(x);

        char number_buf[20];
        snprintf(number_buf, 19, "%d", number);
        draw_list->AddText(ImVec2(pixels.x, pixels.y), ImColor(0, 0, 0, 255),
                           number_buf, number_buf + strlen(number_buf));
      }
    }
    draw_list->PopClipRect();
    if (any_selected) {
      level_ptr->GUIstate.hovered = selected;
    } else {
      level_ptr->GUIstate.hovered = {};
    }

    // Pan (we use a zero mouse threshold when there's no context menu)
    // You may decide to make that threshold dynamic based on whether the
    // mouse is hovering something etc.
    const float mouse_threshold_for_pan =
        opt_enable_context_menu ? -1.0f : 0.0f;
    if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Left,
                                            mouse_threshold_for_pan)) {
      scrolling.x += io.MouseDelta.x;
      scrolling.y += io.MouseDelta.y;
    }

    // Context menu (under default mouse threshold)
    ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
    if (opt_enable_context_menu && drag_delta.x == 0.0f &&
        drag_delta.y == 0.0f) {
      ImGui::OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonLeft);
    } else {
      // draw_list->AddNgonFilled(ImVec2(canvas_p0.x, canvas_p0.y), 20,
      //                          ImColor(255, 0, 0), 6);
    }
    // not dragging
    if (ImGui::BeginPopup("context")) {
      if (ImGui::MenuItem("Remove one", NULL, false, points.Size > 0)) {
        points.resize(points.size() - 2);
      }
      if (ImGui::MenuItem("Remove all", NULL, false, points.Size > 0)) {
        points.clear();
      }
      ImGui::EndPopup();
    }

    // Draw grid + all lines in the canvas
    draw_list->PushClipRect(canvas_p0, canvas_p1, true);
    if (opt_enable_grid) {
      const float GRID_STEP = 64.0f;
      for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x;
           x += GRID_STEP)
        draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y),
                           ImVec2(canvas_p0.x + x, canvas_p1.y),
                           IM_COL32(200, 200, 200, 40));

      for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y;
           y += GRID_STEP)
        draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y),
                           ImVec2(canvas_p1.x, canvas_p0.y + y),
                           IM_COL32(200, 200, 200, 40));
    }

    draw_list->PopClipRect();
  }
};

int main(int, char *[]) {

  level_ptr = hexcells::make_level(1);

  HelloImGui::RunnerParams params;

  HelloImGui::DockableWindow dock_left;
  dock_left.label = "Left";
  dock_left.dockSpaceName = "MainDockSpace";
  dock_left.GuiFunction = &GUI::gui;

  HelloImGui::DockableWindow dock_right;
  dock_right.label = "Right";
  dock_right.dockSpaceName = "Right";
  dock_right.GuiFunction = &GUI::gui;

  prover::proof_widget<hexcells::coord_t, hexcells::state_t> prover_gui{
      level_ptr, {}, true, {*level_ptr->facts().cbegin()}};
  HelloImGui::DockableWindow dock_prover;
  dock_prover.label = "Prover";
  dock_prover.dockSpaceName = "Right";
  dock_prover.GuiFunction = [&] { prover_gui.render(); };

  params.dockingParams.dockableWindows = {dock_left, dock_right, dock_prover};

  params.callbacks.ShowMenus = [] {};

  params.dockingParams.dockingSplits = {
      {"MainDockSpace", "Right", ImGuiDir_Right, 0.35f},
  };

  params.imGuiWindowParams.defaultImGuiWindowType =
      HelloImGui::DefaultImGuiWindowType::ProvideFullScreenDockSpace;
  params.imGuiWindowParams.showMenuBar = true;
  params.imGuiWindowParams.showStatusBar = true;
  params.imGuiWindowParams.configWindowsMoveFromTitleBarOnly = false;
  params.imGuiWindowParams.showMenu_View_Themes = true;

  HelloImGui::Run(params);
  return 0;
}

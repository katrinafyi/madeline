#include "hello_imgui/hello_imgui.h"

#include "level.h"
#include "hexcells.h"

namespace im = ImGui;

void gui() {

  im::Begin("hi");
  im::Text("textaaaaaaaaaaaaaaaaaaaaaa");
  im::End();
}

int main(int, char *[]) {

  hexcells::make_level(1);

  HelloImGui::RunnerParams runner;

  HelloImGui::Run(&gui,     // Gui code
                  "Hello!", // Window title
                  false      // Window size auto
  );
  return 0;
}

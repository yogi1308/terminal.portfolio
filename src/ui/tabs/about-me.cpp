#include "about-me.hpp" 
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

Component AboutMeTab() {
  return Renderer([] {
    return vbox({
      text("About Me") | bold | color(Color::Cyan),
      separator(),
      paragraph("Hello! I am a developer building a portfolio in a terminal."),
      text("I love C++, Linux, and SSH servers."),
    }) | flex;
  });
}
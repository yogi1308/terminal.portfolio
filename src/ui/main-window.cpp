#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>
 
std::string window(int width, int height) {
  using namespace ftxui;
 
  Element document = hbox({
    text("left")   | border,
    text("middle") | border | flex,
    text("right")  | border,
  });
 
  auto screen = Screen::Create(Dimension::Fixed(width), Dimension::Fixed(height));
  Render(screen, document);
  std::string main_window = screen.ToString();
  return main_window;
}
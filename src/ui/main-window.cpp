#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>
 
std::string window() {
    using namespace ftxui;
 
    //   Element document = hbox({
    //     text("left")   | border,
    //     text("middle") | border | flex,
    //     text("right")  | border,
    //   });
 
    auto screen = Screen::Create(Dimension::Full(), Dimension::Full());
    auto message = text("Hello!") | border;
    //   Render(screen, document);
    Render(screen, message);
    std::string main_window = screen.ToString();
    return "\x1B[2J\x1B[H" + main_window;
}
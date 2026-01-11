#pragma once
#include <ftxui/dom/elements.hpp>
#include "state.hpp"

using namespace ftxui;

inline Element HomePage() {
    return vbox({
        center(text(" WELCOME ") | bold | color(Color::Green)),
        separator(),
        paragraph("I build TUI apps in C++ and Go.") | center,
    }) | center | flex;
}

// Replicates pkg/tui/shop.go (List View)
inline Element ProjectsPage(const AppState& state) {
    auto item = [&](int index, std::string title, std::string desc) {
        bool selected = (state.selected_project_index == index);
        
        // The cursor indicator (>)
        std::string prefix = selected ? "> " : "  ";
        
        auto content = hbox({
            text(prefix) | color(Color::Yellow) | bold,
            text(title) | (selected ? bold : dim),
            filler(),
            text(desc) | color(Color::GrayLight)
        });

        if (selected) {
            return content | bgcolor(Color::GrayDark); // Highlight row
        }
        return content;
    };

    return vbox({
        text(" PROJECTS ") | bold | center,
        text(" "),
        item(0, "Terminal Shop Clone", "C++, libssh, FTXUI"),
        item(1, "SSH Server", "Networking, Sockets"),
        item(2, "Bubble Tea Port", "Go, TUI Framework"),
    }) | border | flex;
}

inline Element ContactPage() {
    return vbox({
        text("GitHub:  github.com/yogi"),
        text("Email:   yogi@example.com"),
    }) | center | flex;
}
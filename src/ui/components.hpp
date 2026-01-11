#pragma once
#include <ftxui/dom/elements.hpp>
#include "state.hpp"

using namespace ftxui;

// Replicates pkg/tui/header.go style
inline Element Header(const AppState& state) {
    auto tab = [&](std::string key, std::string label, Page page) {
        bool active = (state.current_page == page);
        // Style: Active gets color, Inactive is dimmed
        auto style = active ? (bold | color(Color::Cyan)) : dim;
        return hbox({
            text("[" + key + "]") | color(Color::Yellow),
            text(label) | style
        }) | center;
    };

    // Responsive: If small, show minimal header
    if (state.is_mobile()) {
        return text("terminal.portfolio") | bold | center | border;
    }

    return hbox({
        // Left: Logo
        text("terminal") | bold, 
        text(".portfolio") | color(Color::GrayLight),
        
        filler(), // Pushes tabs to the right
        
        // Right: Navigation
        tab("h", "ome", PAGE_HOME),
        text(" "),
        tab("p", "rojects", PAGE_PROJECTS),
        text(" "),
        tab("a", "bout", PAGE_ABOUT),
        text(" "),
        tab("c", "ontact", PAGE_CONTACT),
    });
}

// Replicates pkg/tui/footer.go
inline Element Footer(const AppState& state) {
    // Dynamic footer content based on page
    std::string hints = " [q] quit ";
    
    if (state.current_page == PAGE_PROJECTS) {
        hints = " [↑/↓] navigate  [enter] open " + hints;
    }

    return hbox({
        text(" ssh terminal.portfolio ") | color(Color::GrayDark),
        filler(),
        text(hints) | color(Color::Magenta)
    });
}
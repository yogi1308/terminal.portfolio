#pragma once
#include <ftxui/dom/elements.hpp>
#include "state.hpp"
#include "components.hpp"
#include "pages.hpp"

using namespace ftxui;

inline Element AppLayout(const AppState& state) {
    Element content;
    
    switch (state.current_page) {
        case PAGE_HOME:     content = HomePage(); break;
        case PAGE_PROJECTS: content = ProjectsPage(state); break;
        case PAGE_ABOUT:    content = HomePage(); break; 
        case PAGE_CONTACT:  content = ContactPage(); break;
    }

    return vbox({
        Header(state),
        content | flex, // Flex ensures it fills the middle
        Footer(state)
    });
}
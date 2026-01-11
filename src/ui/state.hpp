#pragma once
#include <string>

enum Page {
    PAGE_HOME,
    PAGE_PROJECTS,
    PAGE_ABOUT,
    PAGE_CONTACT
};

struct AppState {
    // Navigation
    Page current_page = PAGE_HOME;
    int selected_project_index = 0; // Tracks which project is highlighted
    bool quit = false;

    // Responsive Design (From root.go)
    int width = 80;
    int height = 24;

    // Helper to detect small screens (Phone vs Desktop)
    bool is_mobile() const { return width < 50; }
};
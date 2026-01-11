#include "main-window.hpp"
#include "tabs/about-me.hpp" 

#include <ftxui/dom/elements.hpp>

using namespace ftxui;

PortfolioUI::PortfolioUI(int width, int height) : width_(width), height_(height) {
    // 1. Initialize the Data
    tab_entries_ = {
        "About Me",
        "Experience",
        "Contact",
    };

    // 2. Define the Tab Menu
    auto menu = Menu(&tab_entries_, &tab_index_, MenuOption::Vertical());

    // 3. Define the Tab Content
    auto content = Container::Tab({
        AboutMeTab(),
        Renderer([] { return text("Experience Section (TODO)"); }),
        Renderer([] { return text("Contact Section (TODO)"); }),
    }, &tab_index_);

    // 4. Combine them
    auto layout = Container::Horizontal({
        menu,
        content | flex,
    });

    // 5. Create the final Renderer
    main_component_ = Renderer(layout, [=] {
        return window(text(" Portfolio "), 
            hbox({
                menu->Render() | vcenter, // Center menu items vertically in their column
                separator(),
                content->Render() | flex,
            })
        ) 
        | size(WIDTH, GREATER_THAN, 60)   // Minimum width for good looks
        | size(HEIGHT, GREATER_THAN, 20)  // Minimum height for good looks
        | center;                         // <--- THIS CENTERS THE WINDOW
    });
}

std::string PortfolioUI::Render() {
    auto screen = Screen::Create(Dimension::Fixed(width_), Dimension::Fixed(height_));
    ftxui::Render(screen, main_component_->Render());
    return screen.ToString();
}

std::string PortfolioUI::Update(char input) {
    Event event;
    // Map Input to Event
    if (input == 'A') event = Event::ArrowUp;
    else if (input == 'B') event = Event::ArrowDown;
    else if (input == 'C') event = Event::ArrowRight;
    else if (input == 'D') event = Event::ArrowLeft;
    else if (input == 13)  event = Event::Return;
    else event = Event::Character(input);

    main_component_->OnEvent(event);
    return Render();
}
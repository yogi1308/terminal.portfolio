#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <string>
#include <vector> // Required for std::vector
#include <memory>
#include <ftxui/component/component.hpp>
#include <ftxui/screen/screen.hpp>

class PortfolioUI {
public:
    PortfolioUI(int width, int height);
    
    std::string Update(char input);
    std::string Render();

private:
    int width_;
    int height_;
    int tab_index_ = 0;
    
    // DATA: This vector must live as long as the Menu component uses it!
    std::vector<std::string> tab_entries_; 

    ftxui::Component main_component_;
};

#endif
#include <iostream>
#include "server/server.hpp" // Linking your logic

int main() {
    std::cout << "Initializing Portfolio Server..." << std::endl;
    
    server(); // This calls the logic you wrote in server.cpp
    
    return 0;
}
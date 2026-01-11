#include <iostream>
#include <libssh/libssh.h>
#include <libssh/server.h>

#include "../../ui/main-window.hpp"
#include "routes.hpp"

const std::string ANSI_ALT_SCREEN = "\033[?1049h\033[H"; 
const std::string ANSI_NORMAL_SCREEN = "\033[?1049l";
const std::string ANSI_CURSOR_HIDE = "\033[?25l";
const std::string ANSI_CURSOR_SHOW = "\033[?25h";

void routes(ssh_channel channel, int width, int height) {
    if (!channel) return;

    // 1. Initialize the UI State
    PortfolioUI ui(width, height);

    // 2. Send setup codes
    std::string setup = ANSI_ALT_SCREEN + ANSI_CURSOR_HIDE;
    ssh_channel_write(channel, setup.c_str(), setup.length());

    // 3. Send Initial Frame
    std::string frame = ui.Render() + "\r"; // \r ensures carriage return
    ssh_channel_write(channel, frame.c_str(), frame.length());

    char buffer[4]; // Buffer for reading escape sequences
    int nbytes;

    // 4. Main Event Loop
    while ((nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0)) > 0) {
        
        // Handle 'q' or Ctrl+C to quit
        if (buffer[0] == 'q' || buffer[0] == 3) break;

        // Simple ANSI parsing for Arrow Keys
        // Arrow keys usually come as: ESC [ A (Up), ESC [ B (Down)
        char input = buffer[0];
        if (nbytes >= 3 && buffer[0] == 27 && buffer[1] == '[') {
            input = buffer[2]; // 'A', 'B', 'C', or 'D'
        }

        // Update UI and get new string
        std::string update = ui.Update(input);
        
        // Clear screen and redraw (simplest method for SSH)
        // \033[H moves cursor to top-left
        std::string output = "\033[H" + update + "\r";
        ssh_channel_write(channel, output.c_str(), output.length());
    }

    // 5. Cleanup
    std::string exit_seq = ANSI_NORMAL_SCREEN + ANSI_CURSOR_SHOW;
    ssh_channel_write(channel, exit_seq.c_str(), exit_seq.length());

    ssh_channel_close(channel);
    ssh_channel_send_eof(channel);
    ssh_channel_free(channel);
}
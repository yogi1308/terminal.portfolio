#include <iostream>

#include <libssh/libssh.h>
#include <libssh/server.h>

#include "../../ui/main-window.hpp"

#include "routes.hpp"


const std::string ANSI_ALT_SCREEN_ENTER = "\033[?1049h"; // Switch to alternate buffer (clears screen)
const std::string ANSI_ALT_SCREEN_EXIT  = "\033[?1049l"; // Switch back to main buffer (restores history)
const std::string ANSI_CURSOR_HIDE      = "\033[?25l";   // Hide the cursor
const std::string ANSI_CURSOR_SHOW      = "\033[?25h";   // Show the cursor
const std::string ANSI_CLEAR            = "\033[2J\033[H"; // Clear and move Home (fallback)


void routes(ssh_channel channel, int width, int height) {
    if (channel) {
        std::string init_sequence = ANSI_ALT_SCREEN_ENTER + ANSI_CURSOR_HIDE + ANSI_CLEAR;
        ssh_channel_write(channel, init_sequence.c_str(), init_sequence.length());

        std::string ui_output = window(width, height) + "\r";
        ssh_channel_write(channel, ui_output.c_str(), ui_output.length());

        char buffer[1];
        // This 'read' blocks THIS thread, but not the main server!
        while (ssh_channel_read(channel, buffer, 1, 0) > 0) {
            if (buffer[0] == 'q' || buffer[0] == 3 || buffer[0] == 4) break;
            // else if (buffer[0] == 'c') {
            //     std::string switched = "C Entered\n\r";
            //     ssh_channel_write(channel, switched.c_str(), switched.length());
            // }
        }

        std::string exit_sequence = ANSI_ALT_SCREEN_EXIT + ANSI_CURSOR_SHOW;
        ssh_channel_write(channel, exit_sequence.c_str(), exit_sequence.length());

        ssh_channel_close(channel);
        ssh_channel_send_eof(channel);
        ssh_channel_free(channel);
    }
}
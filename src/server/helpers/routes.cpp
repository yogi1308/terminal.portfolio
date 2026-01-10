#include <iostream>

#include <libssh/libssh.h>
#include <libssh/server.h>

#include "../../ui/main-window.hpp"

#include "routes.hpp"


void routes(ssh_channel channel) {
    if (channel) {
        std::string ui_output = window() + "\r\n";
        ssh_channel_write(channel, ui_output.c_str(), ui_output.length());

        char buffer[1];
        // This 'read' blocks THIS thread, but not the main server!
        while (ssh_channel_read(channel, buffer, 1, 0) > 0) {
            if (buffer[0] == 'q' || buffer[0] == 3 || buffer[0] == 4) break;
            else if (buffer[0] == 'c') {
                std::string switched = "C Entered\n\r";
                ssh_channel_write(channel, switched.c_str(), switched.length());
            }
        }
        ssh_channel_close(channel);
        ssh_channel_send_eof(channel);
        ssh_channel_free(channel);
    }
}
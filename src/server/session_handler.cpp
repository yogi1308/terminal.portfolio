#include <iostream>
#include <libssh/libssh.h>
#include <libssh/server.h>
#include "../ui/main-window.hpp"
#include "session_handler.hpp"

void handle_client_session(ssh_session session) {
    std::cout << "[Thread] New session thread started." << std::endl;

    if (ssh_handle_key_exchange(session) != SSH_OK) {
        std::cerr << "Handshake failed: " << ssh_get_error(session) << std::endl;
        ssh_free(session);
        return;
    }

    ssh_message msg;
    ssh_channel channel = nullptr;
    bool shell_started = false;

    // 2. Handle Auth & Setup 
    while (!shell_started && (msg = ssh_message_get(session))) {
        int type = ssh_message_type(msg);
        int subtype = ssh_message_subtype(msg);

        if (type == SSH_REQUEST_AUTH && subtype == SSH_AUTH_METHOD_NONE) {
            ssh_message_auth_reply_success(msg, 0);
        } 
        else if (type == SSH_REQUEST_CHANNEL_OPEN && subtype == SSH_CHANNEL_SESSION) {
            channel = ssh_message_channel_request_open_reply_accept(msg);
        } 
        else if (type == SSH_REQUEST_CHANNEL && subtype == SSH_CHANNEL_REQUEST_PTY) {
            ssh_message_channel_request_reply_success(msg);
        } 
        else if (type == SSH_REQUEST_CHANNEL && subtype == SSH_CHANNEL_REQUEST_SHELL) {
            ssh_message_channel_request_reply_success(msg);
            shell_started = true; 
        } 
        else {
            ssh_message_reply_default(msg);
        }
        ssh_message_free(msg);
    }

    // 3. Interaction Loop
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

    // 4. Cleanup when done
    ssh_disconnect(session);
    ssh_free(session);
    std::cout << "[Thread] Session finished and closed." << std::endl;
}
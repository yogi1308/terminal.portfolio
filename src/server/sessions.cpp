#include <iostream>
#include <libssh/libssh.h>
#include <libssh/server.h>
#include "sessions.hpp"
#include "../ui/main-window.hpp"

void sessions_handler(ssh_session session)
{
    std::cout << "New User logged in";

    if (ssh_handle_key_exchange(session) != SSH_OK)
    { // Perform cryptographic key exchange
        std::cerr << "Handshake failed: " << ssh_get_error(session) << std::endl;
        ssh_free(session);
        return;
    }

    ssh_message msg;
    bool shell_started = false;
    ssh_channel channel = nullptr;

    while (!shell_started && (msg = ssh_message_get(session)))
    { // Process SSH messages until shell starts
        int type = ssh_message_type(msg);
        int subtype = ssh_message_subtype(msg);

        if (type == SSH_REQUEST_AUTH && subtype == SSH_AUTH_METHOD_NONE)
        { // Allow authentication without credentials
            ssh_message_auth_reply_success(msg, 0);
        }
        else if (type == SSH_REQUEST_CHANNEL_OPEN && subtype == SSH_CHANNEL_SESSION)
        { // Accept session channel requests
            channel = ssh_message_channel_request_open_reply_accept(msg);
        }
        else if (type == SSH_REQUEST_CHANNEL && subtype == SSH_CHANNEL_REQUEST_PTY)
        { // Grant pseudo-terminal (PTY) requests
            ssh_message_channel_request_reply_success(msg);
        }
        else if (type == SSH_REQUEST_CHANNEL && subtype == SSH_CHANNEL_REQUEST_SHELL)
        { // Start shell and exit setup loop
            ssh_message_channel_request_reply_success(msg);
            shell_started = true;
        }
        else
        { // Handle unknown messages with default reply
            ssh_message_reply_default(msg);
        }
        ssh_message_free(msg);
    }

    if (channel != nullptr)
    {
        if (channel)
        {
            std::string ui_output = window();                                  // Generate UI output
            ssh_channel_write(channel, ui_output.c_str(), ui_output.length()); // Send output to client
            char buffer[1];
            // This 'read' blocks THIS thread, but not the main server!
            while (ssh_channel_read(channel, buffer, 1, 0) > 0)
            {
                if (buffer[0] == 'q' || buffer[0] == 3 || buffer[0] == 4)
                    break;
                // else if (buffer[0] == 'c') {
                //     std::string switched = "C Entered\n\r";
                //     ssh_channel_write(channel, switched.c_str(), switched.length());
                // }
                else {
                    ssh_channel_write(channel, ui_output.c_str(), ui_output.length());
                }
            }
            ssh_channel_close(channel); // Clean up channel resources
            ssh_channel_send_eof(channel);
            ssh_channel_free(channel);
        }
    }
    else
    {
        std::cerr << "Client failed to setup shell." << std::endl;
    }

    ssh_disconnect(session); // Disconnect and free session
    ssh_free(session);
}
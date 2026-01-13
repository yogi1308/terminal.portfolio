#include <iostream>
#include <libssh/libssh.h>
#include <libssh/server.h>
#include <libssh/callbacks.h>
#include "sessions.hpp"
#include "../ui/main-window.hpp"

struct SessionData
{
    bool window_resized = false;
    int width = 80;
    int height = 24;

    char tab = 'a';
    bool tab_changed = false;

    int subtab = 0;

    bool quit = false;
};

int window_change_cb(ssh_session session, ssh_channel channel, int width, int height, int pxwidth, int pxheight, void *userdata)
{
    // 1. Cast the generic 'void*' back to 'SessionData*'
    SessionData *data = static_cast<SessionData *>(userdata);

    // 2. Update the variables inside the struct using '->'
    data->width = width;
    data->height = height;
    data->window_resized = true; // Tell the main loop to redraw!

    return SSH_OK;
}

// This function runs whenever the user types something
int data_cb(ssh_session session, ssh_channel channel, void *data, uint32_t len, int is_stderr, void *userdata)
{
    SessionData *ctx = static_cast<SessionData *>(userdata);
    char *buffer = (char *)data;

    // We only care about the first character for now
    if (len > 0)
    {
        if (buffer[0] == 'q' || buffer[0] == 3 || buffer[0] == 4) {ctx->quit = true;}
        else if (buffer[0] == 'c' || buffer[0] == 'a' || buffer[0] == 'e') {
            ctx->tab = buffer[0]; // Store the key press
            ctx->tab_changed = true; // Set the flag to TRUE
        }
    }

    return len; // Important: Return the number of bytes processed
}

void sessions_handler(ssh_session session)
{
    std::cout << "New User logged in";
    
    if (ssh_handle_key_exchange(session) != SSH_OK)
    { // Perform cryptographic key exchange
        std::cerr << "Handshake failed: " << ssh_get_error(session) << std::endl;
        ssh_free(session);
        return;
    }
    
    SessionData data;
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
            data.width = ssh_message_channel_request_pty_width(msg);
            data.height = ssh_message_channel_request_pty_height(msg);
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
        struct ssh_channel_callbacks_struct cb = {0};

        // FIX: Initialize the struct FIRST, before assigning members
        ssh_callbacks_init(&cb);
        cb.userdata = &data;
        cb.channel_pty_window_change_function = window_change_cb;
        cb.channel_data_function = data_cb;

        ssh_set_channel_callbacks(channel, &cb);

        ssh_event event = ssh_event_new();
        ssh_event_add_session(event, session);

        // Initial Render
        std::string ui_output = window(data.width, data.height);
        ssh_channel_write(channel, ui_output.c_str(), ui_output.length());

        // FIX 1: Loop while NOT quit (and channel open).
        // Do NOT put !data.window_resized here, or it will quit on resize!
        while (!data.quit && ssh_channel_is_open(channel))
        {
            // Block until an event occurs
            int rc = ssh_event_dopoll(event, -1);
            if (rc == SSH_ERROR)
                break;

            // FIX 2: Check the quit flag set by the callback
            if (data.quit)
                break;

            // FIX 3: Move writing INSIDE the if blocks.
            // Only write if something actually changed.

            if (data.window_resized)
            {
                data.window_resized = false;
                std::cout << "Resized to: " << data.width << "x" << data.height << std::endl;

                // Clear screen + Redraw
                ui_output = window(data.width, data.height);
                ssh_channel_write(channel, ui_output.c_str(), ui_output.length());
            }
            else if (data.tab_changed)
            {
                data.tab_changed = false;
                std::cout << "User pressed: " << data.tab << std::endl;

                // Redraw
                ui_output = window(data.width, data.height);
                ssh_channel_write(channel, ui_output.c_str(), ui_output.length());
            }
        }
        std::cout << "User disconnected";
        ssh_channel_close(channel); // Clean up channel resources
        ssh_channel_send_eof(channel);
        ssh_channel_free(channel);
    }
    else
    {
        std::cerr << "Client failed to setup shell." << std::endl;
    }

    ssh_disconnect(session); // Disconnect and free session
    ssh_free(session);
}
#include <iostream>

#include <libssh/libssh.h>
#include <libssh/server.h>

#include "server.hpp"


std::string SERVER_HOST = "0.0.0.0";
int SERVER_PORT = 8000;

void server()
{
    ssh_bind sshbind = ssh_bind_new();

    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDADDR, SERVER_HOST.c_str());
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDPORT, &SERVER_PORT);
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_RSAKEY, "../ssh_host_rsa_key");
    
    if (ssh_bind_listen(sshbind) != SSH_OK) {
        std::cerr << "Error listening to socket: " << ssh_get_error(sshbind) << std::endl;
        return;
    }

    std::cout << "Started SSH server on port " << SERVER_PORT << std::endl;

    while (true) {
        // 1. Create a placeholder for the new visitor
        ssh_session session = ssh_new();
    
        // 2. Wait here until someone connects
        if (ssh_bind_accept(sshbind, session) == SSH_ERROR) {
            std::cerr << "Error accepting connection: " << ssh_get_error(sshbind) << std::endl;
            ssh_free(session); // Clean up if it failed
            continue; // Go back to start of loop to wait for the next try
        }
    
        std::cout << "New connection accepted!" << std::endl;
    
        // 3. Perform the security handshake
        if (ssh_handle_key_exchange(session) != SSH_OK) {
            std::cerr << "Handshake failed: " << ssh_get_error(session) << std::endl;
        } else {
            std::cout << "Handshake successful." << std::endl;
        }

        ssh_message msg;
        ssh_channel channel = nullptr;

        // Loop to handle requests (Auth then Channel)
        bool shell_started = false;
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
                shell_started = true; // Now we can finally send "Hello"!
            } 
            else {
                ssh_message_reply_default(msg); // Decline other requests (like SFTP)
            }

            ssh_message_free(msg);
        }

        // 3. Send data and wait for input
        if (channel) {
            std::string greeting = "Hello from your C++ Server!\n\r";
            ssh_channel_write(channel, greeting.c_str(), greeting.length());

            char buffer[1];
            while (ssh_channel_read(channel, buffer, 1, 0) > 0) {
                if (buffer[0] == 'q') break;
                else if (buffer[0] == 'c') {
                    std::string switched = "C Entered\n\r";
                    ssh_channel_write(channel, switched.c_str(), switched.length());
                }
            }
        }

    
        // For now, we just close the connection so we can loop again
        ssh_disconnect(session);
        ssh_free(session);
    }
}

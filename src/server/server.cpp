#include <iostream>
#include <thread>
#include <libssh/libssh.h>
#include <libssh/server.h>
#include "server.hpp"
#include "sessions.hpp"

std::string SERVER_HOST = "0.0.0.0";
int SERVER_PORT = 8000;

void server() {
    ssh_bind sshbind = ssh_bind_new();
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDADDR, SERVER_HOST.c_str());
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDPORT, &SERVER_PORT);
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_RSAKEY, "../ssh_host_rsa_key"); // Initialize server and configure options

    if (ssh_bind_listen(sshbind) < 0) {                                            // Start listening for incoming connections
        std::cerr << "Error listening: " << ssh_get_error(sshbind) << std::endl;
        return;
    }

    std::cout << "SSH server listening on " << SERVER_PORT << std::endl;

    while (true) {
        ssh_session session = ssh_new();
        
        if (ssh_bind_accept(sshbind, session) == SSH_ERROR) {                      // Wait for a connection (Blocking)
            std::cerr << "Error accepting: " << ssh_get_error(sshbind) << std::endl;
            ssh_free(session);
            continue;
        }

        std::thread(sessions_handler, session).detach();
    }
}
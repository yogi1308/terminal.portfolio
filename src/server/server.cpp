#include <iostream>
#include <thread>
#include <libssh/libssh.h>
#include <libssh/server.h>
#include "session_handler.hpp"
#include "server.hpp"

std::string SERVER_HOST = "0.0.0.0";
int SERVER_PORT = 8000;

void server() {
    ssh_bind sshbind = ssh_bind_new();
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDADDR, SERVER_HOST.c_str());
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDPORT, &SERVER_PORT);
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_RSAKEY, "../ssh_host_rsa_key");

    if (ssh_bind_listen(sshbind) < 0) {
        std::cerr << "Error listening: " << ssh_get_error(sshbind) << std::endl;
        return;
    }

    std::cout << "Multi-threaded SSH server listening on " << SERVER_PORT << std::endl;

    while (true) {
        ssh_session session = ssh_new();
        
        // Wait for a connection (Blocking)
        if (ssh_bind_accept(sshbind, session) == SSH_ERROR) {
            std::cerr << "Error accepting: " << ssh_get_error(sshbind) << std::endl;
            ssh_free(session);
            continue;
        }

        // Instead of handling it here, we launch a thread to run 'handle_client_session'
        // .detach() tells the OS to run it in background and clean up when done.
        std::thread(handle_client_session, session).detach();
    }
}